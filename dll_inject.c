#define _WINNT_WIN32 0500
#include <windows.h>
#include <conio.h>
#include <stdio.h>
int EnableDebugPriv(const char * name)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);
    LookupPrivilegeValue(NULL, name, &luid) ;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    tp.Privileges[0].Luid = luid;
    AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    return 0;
}
BOOL InjectDll(LPSTR szDllFullPath, const DWORD dwRemoteProcessId)
{
    HANDLE hRemoteProcess;
	LPBYTE pszLibFileRemote;
	DWORD dwID;     
	LPVOID pFunc = LoadLibraryA;
	HANDLE hRemoteThread;

    EnableDebugPriv(SE_DEBUG_NAME);

    hRemoteProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwRemoteProcessId );
	if(hRemoteProcess == INVALID_HANDLE_VALUE) {
		wprintf(L"OpenProcess failed %d\n",GetLastError());
	}
    pszLibFileRemote = 
		(char *) VirtualAllocEx( hRemoteProcess, NULL, lstrlenA(szDllFullPath)+1, 	MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hRemoteProcess, pszLibFileRemote, (void *) szDllFullPath, lstrlenA(szDllFullPath)+1, NULL);

	hRemoteThread = 
		CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, pszLibFileRemote, 0, &dwID );
	if (!hRemoteThread)
	{
		wprintf(L"CreateRemoteThread failed %d\n",GetLastError());
		return FALSE;
	}
    CloseHandle(hRemoteProcess);
    CloseHandle(hRemoteThread);
    return TRUE;
}


int main(int argc,char* argv[])
{
	WCHAR szShell[MAX_PATH]=L"C:\\Windows\\SysWOW64\\cmd.exe";
	PROCESS_INFORMATION pi={0};
	STARTUPINFOW si={0};
	CreateProcessW(0,szShell,0,0,FALSE,0,0,0,&si,&pi);
	Sleep(1000);
	InjectDll("E:\\Workspace\\TestDll\\Debug\\TestDll.dll", pi.dwProcessId);
	return 0;
}
