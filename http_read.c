#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>
#pragma comment(lib, "wininet.lib")
int main(int argc, char *argv[])
{
	HINTERNET hConnect,hSession;
    CHAR szBuffer[5];

    TCHAR szStr[100]="http://example.com";
    DWORD dwSize=0;
    DWORD dwDownloaded;
    if(szBuffer == NULL)
    {
        printf("malloc failed \n");
		return -1;
    }
    hSession = InternetOpen(TEXT("testWinINet"), PRE_CONFIG_INTERNET_ACCESS, NULL, INTERNET_INVALID_PORT_NUMBER, 0);
    hConnect = InternetOpenUrl(hSession,szStr,NULL,0,INTERNET_FLAG_DONT_CACHE,0);
    while (InternetReadFile(hConnect,szBuffer,sizeof(szBuffer)-1,&dwDownloaded))
    {
        if (0==dwDownloaded) break;
        szBuffer[dwDownloaded]=0;
		printf(szBuffer);
    }
	return 0;
}