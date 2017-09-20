#include "femy.h"
#include "fclass.h"
#pragma  comment(lib, "Msimg32.lib")
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void GamePaint(HWND hwnd);
void GameInit(HWND hwnd);
void GameDestroy(HWND hwnd);

#define SPIRIT_COUNT 10
#define SPIRIT_IMAGE_COUNT 4
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
typedef struct tagSpirit{
	int x, y;
	int direction;
	int count, max;
}Spirit;
Spirit spirit[SPIRIT_COUNT];
const TCHAR spirit_res[SPIRIT_IMAGE_COUNT][15] = {L"11.bmp", L"22.bmp", L"33.bmp", L"44.bmp"};
HBITMAP spirit_hBitmap[SPIRIT_IMAGE_COUNT];
HBITMAP background;
HBITMAP bmp;
HDC hMemHdc;
HDC hMdc;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdLine){
	FWindow femy;
	int itNow = 0, itPre = 0;
	MSG msg = { 0 };
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
	GameDestroy(msg.hwnd);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	static HDC hdc;
	static PAINTSTRUCT ps;
	switch (message){
	case WM_CREATE:

		break;
		/*
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
		//GamePaint(hwnd);
		break;
		*/
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		else if (wParam == VK_UP)
		{

		}
		else if (wParam == VK_DOWN)
		{

		}
		else if (wParam == VK_LEFT)
		{
		}
		else if (wParam == VK_RIGHT)
		{

		}
		break;
	case WM_KEYUP:
		break;
	case WM_SYSKEYDOWN:

		break;
	case WM_SYSKEYUP:

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
void GamePaint(HWND hwnd){
	HDC hdc = GetDC(hwnd);
	int index;
	//SelectObject(hMemHdc, background);
	BitBlt(hMdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemHdc, 0, 0, SRCCOPY);
	for (index = 0; index < SPIRIT_COUNT; index++)
	{
		SelectObject(hMemHdc, spirit_hBitmap[spirit[index].direction]);
		TransparentBlt(hMdc, spirit[index].x, spirit[index].y, 96, 96, hMemHdc, 0, 0, 96, 96, RGB(0, 0, 0));
	//	BitBlt(hdc, spirit[index].x, spirit[index].y, 96, 96, hMemHdc, 0, 0, SRCCOPY);
		//BitBlt(hdc, spirit[index].x, spirit[index].y, 96, 96, )
		switch (spirit[index].direction)
		{
		case 0:
			spirit[index].y += 6;
			break;
		case 1:
			spirit[index].x -= 6;
			break;
		case 2:
			spirit[index].x += 6;
			break;
		case 3:
			spirit[index].y -= 6;
			break;
		}
		if (spirit[index].count >= spirit[index].max)
		{
			spirit[index].direction = rand() % 4;
			spirit[index].count = 0;
		}
		else
		{
			spirit[index].count++;
		}
		if (spirit[index].x < 0)	spirit[index].x = 0;
		if (spirit[index].x >= WINDOW_WIDTH - 96)	spirit[index].x = WINDOW_WIDTH - 96;
		if (spirit[index].y < 0)	spirit[index].y = 0;
		if (spirit[index].y > WINDOW_HEIGHT - 96)	spirit[index].y = WINDOW_HEIGHT - 96;
	}
	MoveToEx(hMdc, 0, 0, NULL);
	LineTo(hMdc, 500, 500);
	BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMdc, 0, 0, SRCCOPY);
	ReleaseDC(hwnd, hdc);
}
void GameInit(HWND hwnd){
	int index;
	HDC hdc = GetDC(hwnd);
	for (index = 0; index < SPIRIT_IMAGE_COUNT; index++)
	{
		spirit_hBitmap[index] = (HBITMAP)LoadImage(NULL, spirit_res[index], IMAGE_BITMAP, 384, 96, LR_LOADFROMFILE);
		spirit[index].x = 0;// rand() % WINDOW_WIDTH;
		spirit[index].y = 0;// rand() % WINDOW_HEIGHT;
		spirit[index].direction = rand() % 4;
		spirit[index].count = 0;
		spirit[index].max = 20;
	}
	background = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
	hMemHdc = CreateCompatibleDC(hdc);
	hMdc = CreateCompatibleDC(hdc);
	bmp = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hMdc, bmp);
	ReleaseDC(hwnd, hdc);
}
void GameDestroy(HWND hwnd){
	HDC hdc = GetDC(hwnd);
	int i;
	DeleteObject(background);
	for (i = 0; i < SPIRIT_IMAGE_COUNT; i++)
	{
		DeleteObject(spirit_hBitmap[i]);
	}
	DeleteDC(hMdc);
	DeleteDC(hMemHdc);
	
	ReleaseDC(hwnd, hdc);
}