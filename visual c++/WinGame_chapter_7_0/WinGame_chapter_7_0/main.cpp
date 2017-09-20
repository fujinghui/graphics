#include "femy.h"
#include "fclass.h"
#pragma  comment(lib, "Msimg32.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void GamePaint(HWND hwnd);
void GameInit(HWND hwnd);
void GameDestroy(HWND hwnd);
typedef struct tagCharacter{
	int x, y;
	int mx, my;
}Character;
Character character;
HBITMAP hBitmap_Character[4];	//0:up 1:down 2:left 3:right
HBITMAP hBitmap_Background;
HDC hMemDC;
HDC hMemTempDC;
HBITMAP bmp;
int action_max = 8;
int action_index = 0;			//action index
int action_state = 0;
int direction = 0;				//4:stop
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
	static int x, y;
	static int wx, wy;
	static POINT wPoint;
	static RECT rect;
	switch (message){
	case WM_CREATE:
		ShowCursor(true);			//show cursor/hidden cursor
		//SetCapture(hwnd);
		//ReleaseCapture(hwnd);
		rect.left = 100; rect.top = 100;
		rect.right = 300; rect.bottom = 300;
		ClipCursor(&rect);
		GetWindowRect(hwnd, &rect);
		GetClientRect(hwnd, &rect);
		break;
	case WM_KEYDOWN:
		wPoint.x = 100;
		wPoint.y = 100;
		ClientToScreen(hwnd, &wPoint);
		SetCursorPos(wPoint.x, wPoint.y);

		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		else if (wParam == VK_UP)
		{
		//	character.y -= 6;
			direction = 0;
			action_state = 1;
		}
		else if (wParam == VK_DOWN)
		{
		//	character.y += 6;
			direction = 1;
			action_state = 1;
		}
		else if (wParam == VK_LEFT)
		{
		//	character.x -= 6;
			direction = 2;
			action_state = 1;
		}
		else if (wParam == VK_RIGHT)
		{
		//	character.x += 6;
			direction = 3;
			action_state = 1;
		}
		break;

	case WM_LBUTTONDOWN:
	//	x = LOWORD(lParam);
	//	y = HIWORD(lParam);
	//	break;
	case WM_LBUTTONUP:
	//	break;
	case WM_MBUTTONDOWN:
	//	break;
	case WM_MBUTTONUP:
	//	break;
	case WM_RBUTTONDOWN:
	//	break;
	case WM_RBUTTONUP:

	//	break;
	case WM_KEYUP:
		action_state = 0;
	//	break;
	case WM_MOUSEMOVE:
	//	break;
	case WM_MOUSEWHEEL:
	//	break;
		if (wParam & MK_LBUTTON && wParam & MK_SHIFT)// && wParam & MK_RBUTTON)
			;// MessageBox(NULL, _T("test"), _T("title"), MB_OK);
		else if (wParam & MK_LBUTTON & wParam & MK_CONTROL)
			;
	//		MessageBox(NULL, L"tiandao", L"title", MB_OK);
		break;
	case WM_WINDOWPOSCHANGED:
		wx = LOWORD(lParam);
		wy = HIWORD(lParam);
		//MessageBox(NULL, L"position", L"title", MB_OK);
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

void GameInit(HWND hwnd){
	HDC hdc = GetDC(hwnd);
	//加载人物资源
	hBitmap_Character[0] = (HBITMAP)LoadImage(NULL, L"go1.bmp", IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);
	hBitmap_Character[1] = (HBITMAP)LoadImage(NULL, L"go2.bmp", IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);
	hBitmap_Character[2] = (HBITMAP)LoadImage(NULL, L"go3.bmp", IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);
	hBitmap_Character[3] = (HBITMAP)LoadImage(NULL, L"go4.bmp", IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);

	hBitmap_Background = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);

	bmp = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	hMemTempDC = CreateCompatibleDC(hdc);

	SelectObject(hMemTempDC, bmp);

	hMemDC = CreateCompatibleDC(hdc);

	ReleaseDC(hwnd, hdc);
}
void GamePaint(HWND hwnd){
	HDC hdc = GetDC(hwnd);
	if (action_state)
	{
		action_index++;
		if (action_index >= action_max)
			action_index = 0;
		switch (direction)
		{
		case 0: character.y -= 6; break;
		case 1: character.y += 6; break;
		case 2: character.x -= 6; break;
		case 3: character.x += 6; break;
		}
	}
	else
	{
		
	}
	SelectObject(hMemDC, hBitmap_Background);
	BitBlt(hMemTempDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hBitmap_Character[direction]);
	BitBlt(hMemTempDC, character.x, character.y, 60, 108, hMemDC, action_index * 60, 108, SRCAND);
	BitBlt(hMemTempDC, character.x, character.y, 60, 108, hMemDC, action_index * 60, 0, SRCPAINT);

	//将临时存放图像的内存区的内容复制到屏幕显存中
	BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemTempDC, 0, 0, SRCCOPY);
	ReleaseDC(hwnd, hdc);
}
void GameDestroy(HWND hwnd){

}