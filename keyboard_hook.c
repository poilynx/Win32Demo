#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>
HHOOK hHook=NULL;

LRESULT CALLBACK KeyProc(int code,WPARAM wParam,LPARAM lParam)
{
	PKBDLLHOOKSTRUCT pKeyBd=(PKBDLLHOOKSTRUCT)lParam;
	if(wParam==WM_KEYUP)
	{
		wprintf(L"KEYUP (%X)\n",pKeyBd->vkCode);
	}else if(wParam==WM_KEYDOWN)
	{
		wprintf(L"KEYDN (%X)\n",pKeyBd->vkCode);
	}
		CallNextHookEx(hHook,code,wParam,lParam);
	return 1;

}
int main(int argc,char* argv[])
{
	MSG msg;
	HINSTANCE hInstance=(HINSTANCE)GetModuleHandle(NULL);
	hHook=SetWindowsHookEx(WH_KEYBOARD_LL, KeyProc, hInstance, 0);
	
	while(GetMessage(&msg ,NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnhookWindowsHookEx(hHook);
	return 0;
}
