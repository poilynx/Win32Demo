#define _WIN32_WINNT 0x0500


#include <windows.h>
#include <stdio.h>
BOOL ExeCmd(LPWSTR  szCmd)
{
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
    HANDLE hRead, hWrite;
	PROCESS_INFORMATION pi={0};
	STARTUPINFOW si = {sizeof(STARTUPINFOW)};
	BYTE bLine[1024];
    DWORD dwRead = 0;
    if (CreatePipe(&hRead, &hWrite, &sa, 0) == 0)
    {
		wprintf(L"CreatePipe failed %d\n",GetLastError());
        return FALSE;
    }


    GetStartupInfoW(&si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
	

    if (CreateProcessW(NULL, szCmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)  == 0) {
		wprintf(L"CreateProcess failed %d\n",GetLastError());
        return FALSE;
    }
	
    CloseHandle(hWrite);
	

    while (ReadFile(hRead, bLine, 1024 - 1, &dwRead, NULL))
    {
		printf(bLine);
    }
    CloseHandle(hRead);
	return TRUE;
}

int main(int argc, char *argv[])
{
	WCHAR szBuf[4096];//unsafe
	ExeCmd("ping www.baidu.com",szBuf);
	return 0;
}