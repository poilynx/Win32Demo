#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
	HANDLE hMutex=CreateMutex(NULL,FALSE,"HasStarted");
	if (hMutex) {
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			wprintf(L"Process already loaded.");
			getchar();
		} else {
			wprintf(L"Hello world");
			getchar();
		}
	}
	return 0;
}