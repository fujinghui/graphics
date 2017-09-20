#include "femy.h"
#include <windows.h>
#include <stdio.h>
#pragma data_seg("YCIShared")
HWND g_hWndCaller = NULL;
HHOOK g_hHook = NULL;
#pragma data_seg();

LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wParam, LPARAM lParam){
	KBDLLHOOKSTRUCT *pkh = (KBDLLHOOKSTRUCT*)lParam;
	//TCHAR text[100];
	//wsprintf(text, TEXT("code:%d\0"), pkh->vkCode);
	if (pkh->vkCode == VK_ESCAPE)
		MessageBox(NULL, TEXT("fadfafdadfadf"), TEXT("tiandao"), MB_OK);
	if (nCode < 0 || nCode == HC_NOREMOVE)
	{
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}
	if (lParam & 0x40000000)
	{
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}
	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}
HMODULE WINAPI ModuleFromAddress(PVOID pv){
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
	{
		return (HMODULE)mbi.AllocationBase;
	}
	else
	{
		return NULL;
	}
}
//设置导出函数
void __declspec(dllexport) HelloWorld(){
	printf("hello world");
}
int __declspec(dllexport) SetKeyHook(int bInstall, DWORD dwThreadId, HWND hWndCaller){
	BOOL bOk;
	g_hWndCaller = hWndCaller;
	if (bInstall)
	{
		g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyHookProc, ModuleFromAddress(KeyHookProc), dwThreadId);
		bOk = (g_hHook != NULL);
	}
	else
	{
		bOk = ::UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
	return bOk;
}