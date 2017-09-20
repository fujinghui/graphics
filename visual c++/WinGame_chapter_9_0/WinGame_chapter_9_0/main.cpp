#include "femy.h"
#include "fclass.h"
#include "fgame.h"

#pragma comment(lib, "Msimg32.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void GameInit(HWND hwnd);
void GamePaint(HWND hwnd);
void GameDestroy(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdLine){
	FWindow femy;
	MSG msg = { 0 };
	int itNow = 0, itPre = 0;
	femy.FInitWindowClass();
	femy.FSetWndProc(WndProc);
	femy.FSetIcon((HICON)LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE));
	femy.FRegisterWindowClass();
	femy.FCreateWindow();
	femy.FShowWindow(nCmdLine);
	femy.TMoveWindow(100, 100, WINDOW_WIDTH, WINDOW_HEIGHT);
	GameInit(femy.hWnd);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			//MessageBox(NULL, L"tt", L"title", MB_OK);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			itNow = GetTickCount();
			if (itNow - itPre >= 20)
			{
				//Draw
				//MessageBox(NULL, L"tt", L"title", MB_OK);
				GamePaint(femy.hWnd);
				itPre = itNow;
			}
		}
	}
	GameDestroy(femy.hWnd);

	return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	static HDC hdc;
	static PAINTSTRUCT ps;
	static int x, y;
	static int mouse_x, mouse_y;
	switch (message)
	{
	case WM_CREATE:
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
	}

	return 0;
}

void GameInit(HWND hwnd){

}

void GamePaint(HWND hwnd){

}

void GameDestroy(HWND hwnd){

}