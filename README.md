# NtfsVolumeWatch

This library uses NTFS USN Journal records to capture various file system events:
https://learn.microsoft.com/en-us/windows/win32/api/winioctl/ni-winioctl-fsctl_query_usn_journal

This only works on NTFS (possible ReFS?) volumes, that have USN enabled, check with fsutil usn queryjournal C:´´´
to enable it on a volume use ´´´fsutil usn createjournal D:´´´

Make sure to run as administrator/elevated.

If you just need to capture changes to a specific folder, then look into ReadDirectoryChanges:
https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readdirectorychangesw

Created this, because the above, was missing events, when you changed multiple files at the same time.

You can find a description of the different events you can subscribe to here (Reason):
https://learn.microsoft.com/en-us/windows/win32/api/winioctl/ns-winioctl-usn_record_v3

The API does not support getting the full path, only the filename is returned, you could extend it to return the NTFS FILE_ID, and look up that in the NTFS MFT.

Events might get fired multiple times due to how the NTFS/USN Journal works, so probably requires some filtering on the consuming side.