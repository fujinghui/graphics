#include <windows.h>
#include "fclass.h"
#include "femy.h"

#pragma comment(lib, "gouzi.lib")

int hot_key_id = GlobalAddAtom(TEXT("femy"));

FWindow f;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){

	MSG msg = { 0 };
	f.FInitWindowClass();
	f.FSetWndProc(WndProc);
	f.FSetHInstance(hInstance);
	f.FRegisterWindowClass();
	f.FCreateWindow();
	f.FShowWindow(nShowCmd);

	//SetKeyHook(1, 0, f.hWnd);
	if (!RegisterHotKey(f.hWnd, hot_key_id, 0, VK_LEFT))	//注册一个系统热键
	{
		MessageBox(NULL, _T("系统热键注册失败"), _T("error"), MB_OK);
	}
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	/*
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}
	*/
	//SetKeyHook(0, 0, f.hWnd);

	UnregisterHotKey(f.hWnd, hot_key_id);
	f.FUnRegisterWindowClass();

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	static  HWND hEditText;
	switch (message){
	case WM_CREATE:
		hEditText = CreateWindow(_T("edit"), _T("0"), WS_CHILD | WS_VISIBLE, 0, 0, 100, 30, hwnd, NULL, f.getInstance(), 0);
		break;
	case WM_HOTKEY:
		POINT point;
		RECT rect;
		wchar_t text[100];

		//keybd_event(VK_LEFT, 0, 0, 0);
		//keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);


		GetCursorPos(&point);
		GetClientRect(hwnd, &rect);
		wsprintf(text, _T("x:%d y:%d\0"), point.x, point.y);
		SetWindowText(hEditText, text);
		
		if (wParam == hot_key_id)
			;// MessageBox(hwnd, TEXT("rejain"), TEXT("title"), MB_OK);
		//return DefWindowProc(hwnd, message, wParam, lParam);
		InvalidateRect(hwnd, &rect, TRUE);
		
		
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
			PostQuitMessage(0);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	return 0;
}