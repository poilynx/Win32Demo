#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>

VOID EnumFile(LPWSTR szPath) {
	WCHAR szPPath[MAX_PATH];
	WIN32_FIND_DATAW WFD;
	HANDLE hItem;

	lstrcpyW(szPPath,szPath);
	lstrcatW(szPPath,L"\\*");
	hItem=FindFirstFileW(szPPath,&WFD);
	
	if (hItem == INVALID_HANDLE_VALUE) 
		return;

	do {
		lstrcpyW(szPPath,szPath);
		lstrcatW(szPPath,L"\\");
		lstrcatW(szPPath,WFD.cFileName);
		if ((WFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY ) {
			if (wcscmp(WFD.cFileName,L".")&&wcscmp(WFD.cFileName,L".."))
				EnumFile(szPPath);
		} else {
			wprintf(L"%ls\n",szPPath);
		}
	} while (FindNextFileW(hItem,&WFD));
	FindClose(hItem);
	return;
}



int main(int argc, char *argv[]) {
	EnumFile(L"C:\\Windows\\");
	return 0;
}
