#define _WINNT_WIN32 0500
#include<stdio.h> 
#include<windows.h> 
#include<tlhelp32.h> 

int main(int argc, char * argv[])
{
	BOOL bMore;
	PROCESSENTRY32W pe32 = {sizeof(PROCESSENTRY32W)};
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		wprintf(L"CreateToolhelp32Snapshot failed %d\n",GetLastError());
		return -1;
	}
	bMore = Process32FirstW(hProcessSnap, &pe32);  
	while(bMore)
	{
		wprintf(L"%5d %ls\n", pe32.th32ProcessID, pe32.szExeFile);
		bMore = Process32NextW(hProcessSnap, &pe32); 
	}
	CloseHandle(hProcessSnap); 
	return 0;
}

