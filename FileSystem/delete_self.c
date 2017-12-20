#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>

BOOL DeleteMySelf(void)
{
    TCHAR szModule [MAX_PATH],
          szComspec[MAX_PATH],
          szParams [MAX_PATH];
	int ret;

    if((GetModuleFileName(0,szModule,MAX_PATH) !=0 ) &&
       (GetShortPathName(szModule,szModule,MAX_PATH) !=0 ) &&
       (GetEnvironmentVariable(TEXT("COMSPEC"),szComspec,MAX_PATH)!=0))
    {
		STARTUPINFO si={0};
		PROCESS_INFORMATION pi={0};

        lstrcpy(szParams,TEXT(" /c del "));
        lstrcat(szParams, szModule);
        lstrcat(szParams, TEXT(" > nul"));
        lstrcat(szComspec, szParams);

        
        si.cb = sizeof(STARTUPINFO);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        SetPriorityClass(GetCurrentProcess(),
                REALTIME_PRIORITY_CLASS);
        SetThreadPriority(GetCurrentThread(),
            THREAD_PRIORITY_TIME_CRITICAL);

        if(CreateProcess(NULL, szComspec, NULL, FALSE, 0,CREATE_SUSPENDED |
                    CREATE_NO_WINDOW, 0, 0, &si, &pi)) {
			
            SetPriorityClass(pi.hProcess,IDLE_PRIORITY_CLASS);
			SetThreadPriority(pi.hThread,THREAD_PRIORITY_IDLE);
            ResumeThread(pi.hThread);
        } else {
            SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
            SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
        }

		ret = TerminateProcess(GetCurrentProcess(),0);
    }
    
    return FALSE;
}
int main(int argc, char *argv[]) {
	DeleteMySelf();
	return 0;
}
