#include "pch.h"

#include "Manager.h"
#include <vector>

using namespace NtfsVolumeWatch;

Manager::Manager()
{
	running = false;
	syncContext = SynchronizationContext::Current;
}

Manager::~Manager()
{
	this->!Manager();
}

Manager::!Manager()
{
	StopMonitoring();
}

void Manager::StartMonitoring(String^ volume)
{
	if (running)
		return;

	running = true;
	monitoringVolume = volume;

	monitoringThread = gcnew Thread(gcnew ThreadStart(this, &Manager::Monitor));
	monitoringThread->IsBackground = true;
	monitoringThread->Start();
}

void Manager::StopMonitoring()
{
	if (!running)
		return;

	running = false;
	if (monitoringThread != nullptr && monitoringThread->IsAlive)
	{
		monitoringThread->Join();
		monitoringThread = nullptr;
	}
}

void Manager::Monitor()
{
	// In case we just get passed in for example C: instead of \\.\C:
	if (!monitoringVolume->StartsWith("\\\\.\\"))
		monitoringVolume = "\\\\.\\" + monitoringVolume;

	// Convert the managed string to an unmanaged string
	IntPtr unmanagedVolumePath = Marshal::StringToHGlobalUni(monitoringVolume);
	wchar_t* wVolumePath = static_cast<wchar_t*>(unmanagedVolumePath.ToPointer());

	// Open a handle to the volume
	HANDLE hVolume = CreateFile(
		wVolumePath,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,// | FILE_FLAG_OVERLAPPED,
		NULL
	);

	// Free the unmanaged string
	Marshal::FreeHGlobal(unmanagedVolumePath);

	// Check for errors opening the volume
	if (hVolume == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		this->LogMessage("Error opening volume: " + error);

		running = false;
		return;
	}

	// Query the USN Journal data
	USN_JOURNAL_DATA_V2 usnJournalData = { 0 };
	DWORD bytesReturned = 0;

	BOOL success = DeviceIoControl(
		hVolume,
		FSCTL_QUERY_USN_JOURNAL,
		nullptr,
		0,
		&usnJournalData,
		sizeof(usnJournalData),
		&bytesReturned,
		nullptr
	);

	// Check for errors querying the USN Journal data
	if (!success)
	{
		DWORD error = GetLastError();
		CloseHandle(hVolume);

		this->LogMessage("Error querying USN Journal data: " + error);

		running = false;
		return;
	}

	// Set the start USN to the next USN, to only get events that occur after we start monitoring
	USN nextUsn = usnJournalData.NextUsn;

	while (running)
	{
		READ_USN_JOURNAL_DATA_V1 readData = { 0 };
		readData.StartUsn = nextUsn;
		readData.ReturnOnlyOnClose = FALSE;
		readData.Timeout = 1000;
		readData.BytesToWaitFor = 0;
		readData.ReasonMask = currentReasonMask;
		readData.UsnJournalID = usnJournalData.UsnJournalID;
		// Required for Win10/11
		readData.MinMajorVersion = 3;
		readData.MaxMajorVersion = 4;

		// Buffer to store the USN data
		constexpr size_t BUFFER_SIZE = 1024 * 1024; // 1MB
		std::vector<uint8_t> buffer(BUFFER_SIZE);

		DWORD bytesReturnedControl = 0;
		success = DeviceIoControl(
			hVolume,
			FSCTL_READ_USN_JOURNAL,
			&readData,
			sizeof(readData),
			buffer.data(),
			static_cast<DWORD>(buffer.size()),
			&bytesReturnedControl,
			nullptr
		);

		if (!success)
		{
			DWORD error = GetLastError();
			CloseHandle(hVolume);

			this->LogMessage("Error reading USN Journal data: " + error);

			running = false;
			return;
		}

		if (bytesReturnedControl < sizeof(USN))
		{
			this->LogMessage("Insufficient data returned from USN Journal");
		}
		else
		{
			USN* pnewStartUsn = reinterpret_cast<USN*>(buffer.data());
			nextUsn = *pnewStartUsn;

			// Handle USN Journal rollover
			if (nextUsn < usnJournalData.FirstUsn)
			{
				LogMessage("USN Journal has rolled over. Resetting nextUsn to FirstUsn.");
				nextUsn = usnJournalData.FirstUsn;
			}

			if (bytesReturnedControl > sizeof(USN))
			{
				BYTE* pRecordData = buffer.data() + sizeof(USN);
				BYTE* pBufferEnd = buffer.data() + bytesReturnedControl;

				while (pRecordData + sizeof(USN_RECORD_V3) <= pBufferEnd)
				{
					PUSN_RECORD_V3 recordV3 = reinterpret_cast<PUSN_RECORD_V3>(pRecordData);

					// Validate the entire record is within the buffer
					if (pRecordData + recordV3->RecordLength > pBufferEnd)
					{
						this->LogMessage("Record length exceeds buffer size, truncated record discarded");
						break;
					}

					// Extract the fields we want from the record
					ULONGLONG thisRecordUsn = recordV3->Usn;
					DWORD reason = recordV3->Reason;
					FILE_ID_128 fileRef = recordV3->FileReferenceNumber;
					FILE_ID_128 parentFileRef = recordV3->ParentFileReferenceNumber;
					DWORD majorVersion = recordV3->MajorVersion;

					// Try to extract the filename
					int nameLenWchars = recordV3->FileNameLength / sizeof(WCHAR);
					std::wstring fileName(recordV3->FileName, recordV3->FileName + nameLenWchars);

					String^ fileNameManaged = gcnew String(fileName.c_str());

					// Check if it's a V4 record
					if (majorVersion == 4)
					{
						PUSN_RECORD_V4 recordV4 = reinterpret_cast<PUSN_RECORD_V4>(recordV3);

						// Extract the fields we want from the record
					}
					else if (majorVersion == 3)
					{
						// Extract the fields we want from the record
					}
					else
					{
						this->LogMessage("Unsupported USN record version: " + majorVersion);
					}

					// Raise an event for the record
					RaiseEvent(reason, fileNameManaged);

					// Advance to the next record
					pRecordData += recordV3->RecordLength;
				}
			}
		}

		Thread::Sleep(100);
	}

	CloseHandle(hVolume);
}

void Manager::RaiseEvent(DWORD reason, String^ fileName)
{
	if ((reason & USN_REASON_DATA_OVERWRITE) != 0 && OnDataOverwriteEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnDataOverwriteCallback), fileName);
		}
		else
		{
			OnDataOverwriteEvent(fileName);
		}
	}

	if ((reason & USN_REASON_DATA_EXTEND) != 0 && OnDataExtendEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnDataExtendCallback), fileName);
		}
		else
		{
			OnDataExtendEvent(fileName);
		}
	}

	if ((reason & USN_REASON_DATA_TRUNCATION) != 0 && OnDataTruncationEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnDataTruncationCallback), fileName);
		}
		else
		{
			OnDataTruncationEvent(fileName);
		}
	}

	if ((reason & USN_REASON_NAMED_DATA_OVERWRITE) != 0 && OnNamedDataOverwriteEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnNamedDataOverwriteCallback), fileName);
		}
		else
		{
			OnNamedDataOverwriteEvent(fileName);
		}
	}

	if ((reason & USN_REASON_NAMED_DATA_EXTEND) != 0 && OnNamedDataExtendEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnNamedDataExtendCallback), fileName);
		}
		else
		{
			OnNamedDataExtendEvent(fileName);
		}
	}

	if ((reason & USN_REASON_NAMED_DATA_TRUNCATION) != 0 && OnNamedDataTruncationEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnNamedDataTruncationCallback), fileName);
		}
		else
		{
			OnNamedDataTruncationEvent(fileName);
		}
	}


	if ((reason & USN_REASON_FILE_CREATE) != 0 && OnCreateEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnCreateCallback), fileName);
		}
		else
		{
			OnCreateEvent(fileName);
		}
	}

	if ((reason & USN_REASON_FILE_DELETE) != 0 && OnDeleteEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnDeleteCallback), fileName);
		}
		else
		{
			OnDeleteEvent(fileName);
		}
	}

	if ((reason & USN_REASON_EA_CHANGE) != 0 && OnEaChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnEaChangeCallback), fileName);
		}
		else
		{
			OnEaChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_SECURITY_CHANGE) != 0 && OnSecurityChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnSecurityChangeCallback), fileName);
		}
		else
		{
			OnSecurityChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_HARD_LINK_CHANGE) != 0 && OnHardLinkChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnHardLinkChangeCallback), fileName);
		}
		else
		{
			OnHardLinkChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_RENAME_OLD_NAME) != 0 && OnRenameOldNameEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnRenameOldNameCallback), fileName);
		}
		else
		{
			OnRenameOldNameEvent(fileName);
		}
	}

	if ((reason & USN_REASON_RENAME_NEW_NAME) != 0 && OnRenameNewNameEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnRenameNewNameCallback), fileName);
		}
		else
		{
			OnRenameNewNameEvent(fileName);
		}
	}

	if ((reason & USN_REASON_INDEXABLE_CHANGE) != 0 && OnIndexableChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnIndexableChangeCallback), fileName);
		}
		else
		{
			OnIndexableChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_BASIC_INFO_CHANGE) != 0 && OnBasicInfoChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnBasicInfoChangeCallback), fileName);
		}
		else
		{
			OnBasicInfoChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_HARD_LINK_CHANGE) != 0 && OnHardLinkChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnHardLinkChangeCallback), fileName);
		}
		else
		{
			OnHardLinkChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_COMPRESSION_CHANGE) != 0 && OnCompressionChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnCompressionChangeCallback), fileName);
		}
		else
		{
			OnCompressionChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_ENCRYPTION_CHANGE) != 0 && OnEncryptionChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnEncryptionChangeCallback), fileName);
		}
		else
		{
			OnEncryptionChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_OBJECT_ID_CHANGE) != 0 && OnObjectIdChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnObjectIdChangeCallback), fileName);
		}
		else
		{
			OnObjectIdChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_REPARSE_POINT_CHANGE) != 0 && OnReparsePointChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnReparsePointChangeCallback), fileName);
		}
		else
		{
			OnReparsePointChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_STREAM_CHANGE) != 0 && OnStreamChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnStreamChangeCallback), fileName);
		}
		else
		{
			OnStreamChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_TRANSACTED_CHANGE) != 0 && OnTransactedChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnTransactedChangeCallback), fileName);
		}
		else
		{
			OnTransactedChangeEvent(fileName);
		}
	}

	if ((reason & USN_REASON_INTEGRITY_CHANGE) != 0 && OnIntegrityChangeEvent != nullptr)
	{
		if (syncContext != nullptr)
		{
			syncContext->Post(gcnew SendOrPostCallback(this, &Manager::OnIntegrityChangeCallback), fileName);
		}
		else
		{
			OnIntegrityChangeEvent(fileName);
		}
	}
}

void Manager::LogMessage(String^ message)
{
	if (DebugEnabled)
		Console::WriteLine(message);
}

void Manager::UpdateReasonMask()
{
	// Update the reason mask based on the events we want to monitor
	DWORD reasonMask = 0;

	if (OnDataOverwriteEvent != nullptr)
		reasonMask |= USN_REASON_DATA_OVERWRITE;
	if (OnDataExtendEvent != nullptr)
		reasonMask |= USN_REASON_DATA_EXTEND;
	if (OnDataTruncationEvent != nullptr)
		reasonMask |= USN_REASON_DATA_TRUNCATION;
	if (OnNamedDataOverwriteEvent != nullptr)
		reasonMask |= USN_REASON_NAMED_DATA_OVERWRITE;
	if (OnNamedDataExtendEvent != nullptr)
		reasonMask |= USN_REASON_NAMED_DATA_EXTEND;
	if (OnNamedDataTruncationEvent != nullptr)
		reasonMask |= USN_REASON_NAMED_DATA_TRUNCATION;
	if (OnCreateEvent != nullptr)
		reasonMask |= USN_REASON_FILE_CREATE;
	if (OnDeleteEvent != nullptr)
		reasonMask |= USN_REASON_FILE_DELETE;
	if (OnEaChangeEvent != nullptr)
		reasonMask |= USN_REASON_EA_CHANGE;
	if (OnSecurityChangeEvent != nullptr)
		reasonMask |= USN_REASON_SECURITY_CHANGE;
	if (OnRenameOldNameEvent != nullptr)
		reasonMask |= USN_REASON_RENAME_OLD_NAME;
	if (OnRenameNewNameEvent != nullptr)
		reasonMask |= USN_REASON_RENAME_NEW_NAME;
	if (OnIndexableChangeEvent != nullptr)
		reasonMask |= USN_REASON_INDEXABLE_CHANGE;
	if (OnBasicInfoChangeEvent != nullptr)
		reasonMask |= USN_REASON_BASIC_INFO_CHANGE;
	if (OnHardLinkChangeEvent != nullptr)
		reasonMask |= USN_REASON_HARD_LINK_CHANGE;
	if (OnCompressionChangeEvent != nullptr)
		reasonMask |= USN_REASON_COMPRESSION_CHANGE;
	if (OnEncryptionChangeEvent != nullptr)
		reasonMask |= USN_REASON_ENCRYPTION_CHANGE;
	if (OnObjectIdChangeEvent != nullptr)
		reasonMask |= USN_REASON_OBJECT_ID_CHANGE;
	if (OnReparsePointChangeEvent != nullptr)
		reasonMask |= USN_REASON_REPARSE_POINT_CHANGE;
	if (OnStreamChangeEvent != nullptr)
		reasonMask |= USN_REASON_STREAM_CHANGE;
	if (OnTransactedChangeEvent != nullptr)
		reasonMask |= USN_REASON_TRANSACTED_CHANGE;
	if (OnIntegrityChangeEvent != nullptr)
		reasonMask |= USN_REASON_INTEGRITY_CHANGE;
	
	currentReasonMask = reasonMask;
}

void Manager::EnableDebugging()
{
	DebugEnabled = true;
}