#include <Windows.h>
#include <stdio.h>
HINSTANCE g_Hinstance = NULL;
HHOOK g_Hook = NULL;
BOOL SetHook();
BOOL UnSetHook();


static int Loop = 0;
static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam){
	KBDLLHOOKSTRUCT *pkh = (KBDLLHOOKSTRUCT*)lParam;
	printf("afafda");
	if (nCode == HC_ACTION)
	{
		if (GetAsyncKeyState('1') & 0x8000)
		{
			printf("----");
		}
		BOOL bCtrlKey = GetAsyncKeyState(VK_CONTROL) >> ((sizeof(SHORT)* 8) - 1);
		printf("key:%d\n", pkh->vkCode);
		if (pkh->vkCode == VK_ESCAPE)
		{
			Loop = 0;
		}
	}
	return CallNextHookEx(g_Hook, nCode, wParam, lParam);
}
BOOL SetHook(){
	if (g_Hinstance || g_Hook)
		return TRUE;
	g_Hinstance = (HINSTANCE)GetModuleHandle(NULL);
	g_Hook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, g_Hinstance, 0);
	if (!g_Hook)
	{
		printf("Set Hook error!error code:%d\n", GetLastError());
		system("pause");
		return FALSE;
	}
	return TRUE;
}
BOOL UnSetHook(){
	if (g_Hook){
		UnhookWindowsHookEx(g_Hook);
		g_Hook = NULL;
	}
	return TRUE;
}
int main(int argc, int *argv[]){
	
	if (!SetHook())
		return -1;
	while (Loop < 50){
		printf("-");
		Loop++;
		Sleep(100);
	}
	UnSetHook();
}