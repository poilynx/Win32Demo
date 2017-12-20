#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

int main(int argc, char *argv[]) {
	WCHAR szBuf[MAX_PATH];
	GetModuleFileNameW(NULL,szBuf,MAX_PATH);
	printf("Module FileName\t %ls\n", szBuf);

	GetSystemDirectoryW(szBuf,MAX_PATH);
	printf("System Path\t %ls\n", szBuf);


	return 0;
}
