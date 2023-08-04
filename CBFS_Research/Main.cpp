#include<iostream>
#include<Windows.h>
#include<string>
#include<assert.h>
#include<tchar.h>
#include<conio.h>
#include<stdio.h>

#ifdef _UNICODE
#include "C:/Users/TienCong/Documents/CBFS Filter 2022 C++ Edition/include/unicode/cbfsfilter.h"
#else
#include "../../include/cbfsfilter.h"
#endif

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#define FALSE	0
#define TRUE	1
#define ERROR_ACCESS_DENIED              5L
#define EVENT_TIME_OUT					5000

LPCTSTR g_Guid = _T("{713CC6CE-B3E2-4fd9-838D-E28F558F6866}");
CRITICAL_SECTION    g_LogListLock = { 0, };

void AddToLog(CBMonitor* Watcher, LPCTSTR Operation, LPCTSTR FileName, DWORD Status) {
	//EnterCriticalSection(&g_LogListLock);
	_tprintf(_T("%s - %s (Operation Status: %d)\n"), Operation, FileName, Status);
	//LeaveCriticalSection(&g_LogListLock);
}



class FileWatcher : public CBMonitor
{
public:
	FileWatcher() : CBMonitor()
	{
	}
	INT FireNotifyReadFile(CBMonitorNotifyReadFileEventParams* e) {
		AddToLog(this, _T("NotifyReadFile"), e->FileName,0);
		return 0;
	}

	INT FireNotifyRenameOrMoveFile(CBMonitorNotifyRenameOrMoveFileEventParams* e) {
		AddToLog(this, _T("NotifyRenameOrMoveFile"), e->FileName,0);
		return 0;
	}

	

}; FileWatcher g_CBMonitor;

void SetFilter(LPCTSTR PathToMonitor, LPCTSTR FileNameMask)
{
	int l = _tcslen(PathToMonitor);
	int path_len = l;

	if (l == 0)
	{
		printf("Nothing to monitor\n");
		exit(3);
	}

	if (FileNameMask != NULL)
	{
		l += _tcslen(FileNameMask);
	}
	else {
		l += 1; // for '*'
	}

	l += 2; // for the seperator and the trailing \0

	LPTSTR mask_buf = (LPTSTR)malloc(l * sizeof(TCHAR));
	_tcscpy(mask_buf, PathToMonitor);

	if (PathToMonitor[path_len - 1] != '\\')
	{
		_tcscat(mask_buf, _T("\\"));
	}
	if (FileNameMask != NULL) {
		_tcscat(mask_buf, FileNameMask);
	}
	else {
		_tcscat(mask_buf, _T("*"));
	}

	//Add filtering	rule 
	g_CBMonitor.AddFilterRule(mask_buf,
		cbfConstants::FS_NE_READ |
		cbfConstants::FS_NE_WRITE |
		cbfConstants::FS_NE_DELETE |
		cbfConstants::FS_NE_CREATE |
		cbfConstants::FS_NE_RENAME |
		cbfConstants::FS_NE_CREATE_HARD_LINK |
		cbfConstants::FS_NE_SET_SIZES |
		cbfConstants::FS_NE_DELETE |
		cbfConstants::FS_NE_ENUMERATE_DIRECTORY |
		cbfConstants::FS_NE_OPEN |
		cbfConstants::FS_NE_CLOSE |
		cbfConstants::FS_NE_CLEANUP |
		cbfConstants::FS_NE_SET_SECURITY);

	// Initialize and start the filter
	int retVal = g_CBMonitor.SetProcessFailedRequests(TRUE);
	if (0 == retVal) {
		retVal = g_CBMonitor.Initialize(g_Guid);
	}
	if (0 == retVal) {
		retVal = g_CBMonitor.StartFilter();
	}

	if (retVal != 0) {
		_tprintf(_T("Error %d while trying to start filtering\n"), retVal);
		exit(retVal);
	}
	_tprintf(_T("Start the monitor with the rule mask '%s'\n"), mask_buf);
}

void DeleteFilter() {
	g_CBMonitor.StopFilter(TRUE);
}

int main() {
	wchar_t path[] = L"D:\\test";
	LPTSTR PathToMonitor = path;
	LPTSTR FilenameMask = NULL;

	try {
		SetFilter(PathToMonitor, FilenameMask);
		_getch();
		DeleteFilter();
	}
	catch (int e)
	{
	}

	if (PathToMonitor) {
		free(PathToMonitor);
	}
	if (FilenameMask) {
		free(FilenameMask);
	}
	return 0;
}


