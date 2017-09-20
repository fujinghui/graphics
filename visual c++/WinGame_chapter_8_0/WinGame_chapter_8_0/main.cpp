#include "femy.h"
#include "fclass.h"

#pragma comment(lib, "Msimg32.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void GameInit(HWND hwnd);
void GamePaint(HWND hwnd);
void GameDestroy(HWND hwnd);
typedef struct tagBall{
	int x, y;
	int vx, vy;
	int ax, ay;
	int width, height;
}Ball;
/* 游戏资源定义 */
#define BALL_MAX_COUNT 1000
HBITMAP hBitmap_Background;
HBITMAP hBitmap_TempMemBitmap;
HBITMAP hBitmap_Ball;
HDC hMemDC, hMemTempBackgroundDC;
Ball ball;
Ball balls[BALL_MAX_COUNT];

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
	GameDestroy(msg.hwnd);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	static HDC hdc;
	static PAINTSTRUCT ps;
	static int x, y;
	static int mouse_x, mouse_y;
	static POINT point;
	static POINT lt, rb;
	static RECT rect;
	static int time_id = 100;
	static int i;
	switch (message)
	{
	case WM_CREATE:
		SetTimer(hwnd, time_id, 20, NULL);
		break;
	case WM_TIMER:
		for (i = 0; i < BALL_MAX_COUNT; i++)
		{
			balls[i].x += balls[i].vx; balls[i].y += balls[i].vy;
			balls[i].vx += balls[i].ax; balls[i].vy += balls[i].ay;
			if (balls[i].y > WINDOW_HEIGHT - balls[i].height)
			{
				balls[i].y = WINDOW_HEIGHT - balls[i].height;
				balls[i].vy = -(int)balls[i].vy*0.9;
			}
			if (balls[i].x > WINDOW_WIDTH - balls[i].width)
			{
				balls[i].x = WINDOW_WIDTH - balls[i].width;
				balls[i].vx = -balls[i].vx;
			}
			else if (balls[i].x < 0)
			{
				balls[i].x = 0;
				balls[i].vx = -balls[i].vx;
			}
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
			PostQuitMessage(0);
		}
		break;
	case WM_DESTROY:
		KillTimer(hwnd, time_id);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	return 0;
}
void GameInit(HWND hwnd){
	HDC hdc = GetDC(hwnd);
	int i;
	hBitmap_Background = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	hBitmap_Ball = (HBITMAP)LoadImage(NULL, L"star.bmp", IMAGE_BITMAP, 205, 205, LR_LOADFROMFILE);

	hBitmap_TempMemBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	hMemTempBackgroundDC = CreateCompatibleDC(hdc);					//create match this screen memory dc
	hMemDC = CreateCompatibleDC(hdc);

	SelectObject(hMemTempBackgroundDC, hBitmap_TempMemBitmap);

//	ball.x = 0; ball.y = 0;
//	ball.vx = 4; ball.vy = 6;
//	ball.ax = 0; ball.ay = 1;			//y-axis accelerated equals 1
//	ball.width = 205/4; ball.height = 205/4;
	for (i = 0; i < BALL_MAX_COUNT; i++)
	{
		balls[i].x = rand() % WINDOW_WIDTH;
		balls[i].y = rand() % WINDOW_HEIGHT;
		balls[i].vx = rand() % 5 + 2;
		balls[i].vy = rand() % 6 + 3;
		balls[i].ax = 0;
		balls[i].ay = 3;
		balls[i].width = 205 / 8;
		balls[i].height = 205 / 8;
	}
	ReleaseDC(hwnd, hdc);
}
void GamePaint(HWND hwnd){
	HDC hdc = GetDC(hwnd);
	int i;
	SelectObject(hMemDC, hBitmap_Background);
	BitBlt(hMemTempBackgroundDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, SRCCOPY);
	for (i = 0; i < BALL_MAX_COUNT; i++)
	{
		SelectObject(hMemDC, hBitmap_Ball);
		//BitBlt(hMemTempBackgroundDC, 0, 0, ball.width, ball.height, hMemDC, 0, 0, SRCCOPY);
		TransparentBlt(hMemTempBackgroundDC, balls[i].x, balls[i].y, balls[i].width, balls[i].height, hMemDC, 0, 0, balls[i].width * 8, balls[i].height * 8, RGB(0, 0, 0));
	}
	BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemTempBackgroundDC, 0, 0, SRCCOPY);

	ReleaseDC(hwnd, hdc);
}
void GameDestroy(HWND hwnd){

}