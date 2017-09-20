// 动态库.cpp : 定义 DLL 应用程序的导出函数
#include "stdafx.h"
//
DWORD score = 0;
LRESULT CALLBACK KeyboradProc(int code, WPARAM wParam, LPARAM lParam){

	DWORD game_id = 0;
	DWORD dwRead = 0;
	HWND hwnd_game;
	HANDLE handle_game;
	switch (wParam)
	{
	case VK_UP:
		hwnd_game = FindWindowA(NULL, "俄罗斯方块-雅道版");
		if (hwnd_game == NULL)
		{
			MessageBoxA(NULL, "未找到游戏", "Error", MB_OK);
			return CallNextHookEx(0, code, wParam, lParam);
		}
		GetWindowThreadProcessId(hwnd_game, &game_id);
		handle_game = OpenProcess(PROCESS_ALL_ACCESS, FALSE, game_id);			//打开此进程
		if (handle_game == NULL)
		{
			MessageBoxA(NULL, "打开进程错误", "Error", MB_OK);
			return CallNextHookEx(0, code, wParam, lParam);
		}
		WriteProcessMemory(handle_game, (LPVOID)0x0040DF38, &score, 4, &dwRead);
		score++;
		CloseHandle(handle_game);
		break;
	case VK_DOWN:

		break;
	case VK_LEFT:

		break;
	case VK_RIGHT:

		break;
	default:

		break;
	}
	
	
	return CallNextHookEx(0, code, wParam, lParam);
}
extern "C" _declspec(dllexport) HHOOK HookGame(){
	HWND game_hwnd = FindWindow(NULL, L"俄罗斯方块-雅道版");
	if (game_hwnd == NULL)
	{
		MessageBoxA(NULL, "未找到游戏", "Error", MB_OK);
		return NULL;
	}
	DWORD tid = GetWindowThreadProcessId(game_hwnd, NULL);		//获取进程的身份证号
	return ::SetWindowsHookEx(WH_KEYBOARD, &KeyboradProc, ::GetModuleHandle(L"动态库.dll"), tid);
}

