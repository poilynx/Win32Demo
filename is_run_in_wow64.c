#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

BOOL IsWow64()
{
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    BOOL bIsWow64 = FALSE;

    fnIsWow64Process = 
		(LPFN_ISWOW64PROCESS)GetProcAddress(
			GetModuleHandle((L"kernel32")), 
			"IsWow64Process");

    if (NULL != fnIsWow64Process)
        fnIsWow64Process(GetCurrentProcess(),&bIsWow64);

    return bIsWow64;
}

int main(int argc, char *argv[]) {
	if(IsWow64())
		printf("Run in wow64\n");
	else
		printf("Not run in wow64\n");

	return 0;
}
