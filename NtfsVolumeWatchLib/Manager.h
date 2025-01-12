#pragma once

#include <windows.h>
#include <string>

using namespace System;
using namespace System::Threading;
using namespace System::Runtime::InteropServices;
using namespace System::ComponentModel;

namespace NtfsVolumeWatch {

	public delegate void USNEventHandler(String^ fileName);

	public ref class Manager
	{
	public:

		event USNEventHandler^ OnDataOverwrite
		{
			void add(USNEventHandler^ handler)
			{
				OnDataOverwriteEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnDataOverwriteEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnDataExtend
		{
			void add(USNEventHandler^ handler)
			{
				OnDataExtendEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnDataExtendEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnDataTruncation
		{
			void add(USNEventHandler^ handler)
			{
				OnDataTruncationEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnDataTruncationEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnNamedDataOverwrite
		{
			void add(USNEventHandler^ handler)
			{
				OnNamedDataOverwriteEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnNamedDataOverwriteEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnNamedDataExtend
		{
			void add(USNEventHandler^ handler)
			{
				OnNamedDataExtendEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnNamedDataExtendEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnNamedDataTruncation
		{
			void add(USNEventHandler^ handler)
			{
				OnNamedDataTruncationEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnNamedDataTruncationEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnCreate
		{
			void add(USNEventHandler^ handler)
			{
				OnCreateEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnCreateEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnDelete
		{
			void add(USNEventHandler^ handler)
			{
				OnDeleteEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnDeleteEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnEaChange
		{
			void add(USNEventHandler^ handler)
			{
				OnEaChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnEaChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnSecurityChange
		{
			void add(USNEventHandler^ handler)
			{
				OnSecurityChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnSecurityChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnRenameOldName
		{
			void add(USNEventHandler^ handler)
			{
				OnRenameOldNameEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnRenameOldNameEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnRenameNewName
		{
			void add(USNEventHandler^ handler)
			{
				OnRenameNewNameEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnRenameNewNameEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnIndexableChange
		{
			void add(USNEventHandler^ handler)
			{
				OnIndexableChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnIndexableChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnBasicInfoChange
		{
			void add(USNEventHandler^ handler)
			{
				OnBasicInfoChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnBasicInfoChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnHardLinkChange
		{
			void add(USNEventHandler^ handler)
			{
				OnHardLinkChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnHardLinkChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnCompressionChange
		{
			void add(USNEventHandler^ handler)
			{
				OnCompressionChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnCompressionChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnEncryptionChange
		{
			void add(USNEventHandler^ handler)
			{
				OnEncryptionChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnEncryptionChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnObjectIdChange
		{
			void add(USNEventHandler^ handler)
			{
				OnObjectIdChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnObjectIdChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnReparsePointChange
		{
			void add(USNEventHandler^ handler)
			{
				OnReparsePointChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnReparsePointChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnStreamChange
		{
			void add(USNEventHandler^ handler)
			{
				OnStreamChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnStreamChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnTransactedChange
		{
			void add(USNEventHandler^ handler)
			{
				OnTransactedChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnTransactedChangeEvent -= handler;
				UpdateReasonMask();
			}
		}

		event USNEventHandler^ OnIntegrityChange
		{
			void add(USNEventHandler^ handler)
			{
				OnIntegrityChangeEvent += handler;
				UpdateReasonMask();
			}
			void remove(USNEventHandler^ handler)
			{
				OnIntegrityChangeEvent -= handler;
				UpdateReasonMask();
			}
		}		

		Manager();
		~Manager();
		!Manager();

		void StartMonitoring(String^ volume);
		void StopMonitoring();

		void EnableDebugging();

	private:

		bool DebugEnabled = false;
		void LogMessage(String^ message);
				
		USNEventHandler^ OnDataOverwriteEvent;
		USNEventHandler^ OnDataExtendEvent;
		USNEventHandler^ OnDataTruncationEvent;
		USNEventHandler^ OnNamedDataOverwriteEvent;
		USNEventHandler^ OnNamedDataExtendEvent;
		USNEventHandler^ OnNamedDataTruncationEvent;
		USNEventHandler^ OnDeleteEvent;
		USNEventHandler^ OnCreateEvent;
		USNEventHandler^ OnEaChangeEvent;
		USNEventHandler^ OnSecurityChangeEvent;
		USNEventHandler^ OnRenameOldNameEvent;
		USNEventHandler^ OnRenameNewNameEvent;
		USNEventHandler^ OnIndexableChangeEvent;
		USNEventHandler^ OnBasicInfoChangeEvent;
		USNEventHandler^ OnHardLinkChangeEvent;
		USNEventHandler^ OnCompressionChangeEvent;
		USNEventHandler^ OnEncryptionChangeEvent;
		USNEventHandler^ OnObjectIdChangeEvent;
		USNEventHandler^ OnReparsePointChangeEvent;
		USNEventHandler^ OnStreamChangeEvent;
		USNEventHandler^ OnTransactedChangeEvent;
		USNEventHandler^ OnIntegrityChangeEvent;

		void OnDataOverwriteCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnDataOverwriteEvent(fileName);
		}

		void OnDataExtendCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnDataExtendEvent(fileName);
		}

		void OnDataTruncationCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnDataTruncationEvent(fileName);
		}

		void OnNamedDataOverwriteCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnNamedDataOverwriteEvent(fileName);
		}

		void OnNamedDataExtendCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnNamedDataExtendEvent(fileName);
		}

		void OnNamedDataTruncationCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnNamedDataTruncationEvent(fileName);
		}

		void OnDeleteCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnDeleteEvent(fileName);
		}

		void OnCreateCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnCreateEvent(fileName);
		}

		void OnEaChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnEaChangeEvent(fileName);
		}

		void OnSecurityChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnSecurityChangeEvent(fileName);
		}

		void OnRenameOldNameCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnRenameOldNameEvent(fileName);
		}

		void OnRenameNewNameCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnRenameNewNameEvent(fileName);
		}

		void OnIndexableChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnIndexableChangeEvent(fileName);
		}

		void OnBasicInfoChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnBasicInfoChangeEvent(fileName);
		}

		void OnHardLinkChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnHardLinkChangeEvent(fileName);
		}

		void OnCompressionChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnCompressionChangeEvent(fileName);
		}

		void OnEncryptionChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnEncryptionChangeEvent(fileName);
		}

		void OnObjectIdChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnObjectIdChangeEvent(fileName);
		}

		void OnReparsePointChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnReparsePointChangeEvent(fileName);
		}

		void OnStreamChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnStreamChangeEvent(fileName);
		}

		void OnTransactedChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnTransactedChangeEvent(fileName);
		}

		void OnIntegrityChangeCallback(Object^ state)
		{
			String^ fileName = safe_cast<String^>(state);
			OnIntegrityChangeEvent(fileName);
		}

		DWORD currentReasonMask = 0;
		void UpdateReasonMask();

		String^ monitoringVolume;

		void Monitor();
		void RaiseEvent(DWORD reason, String^ fileName);

		Thread^ monitoringThread;
		volatile bool running;

		SynchronizationContext^ syncContext;
	};
}
