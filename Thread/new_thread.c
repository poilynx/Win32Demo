#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>

DWORD WINAPI FunProc(LPVOID pParam)
{
        wprintf(L"%ls\n",(LPWSTR)pParam);
		return 0;
}

void main()
{
         HANDLE hThread;
         DWORD  dwThreadId = 0;

         hThread=CreateThread(0, 0, FunProc, L"New Thread...", 0, &dwThreadId);
		 SetThreadPriority(hThread, THREAD_PRIORITY_IDLE);
		 wprintf(L"Main thread...\n");
		 Sleep(100);//Wait for a moment;
         CloseHandle(hThread);
}