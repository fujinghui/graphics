#include "femy.h"
#include "fclass.h"
#include "ftext.h"
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msimg32.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void GameInit(HDC hdc);
void GameDraw(HDC hdc);
void GameDraw2(HDC hdc);
void GameDraw3(HDC hdc);
void GameDestroy();

void DrawAnimation(HDC hdc, int index);
/* Game Resources*/

HPEN hPenSolid;				//实线画笔
HBRUSH hBrush;				//正常画刷
HBRUSH hHatchBrush;			//阴影画刷
HBRUSH hHatchBrush2;
HBRUSH hHatchBrush3;
HBRUSH hHatchBrush4;
HBITMAP hBitmapBackground;
HBITMAP hBitmapTransparent;	//透明图

const TCHAR animation_res[11][15] = {L"image/0.bmp", L"image/1.bmp", L"image/2.bmp", L"image/3.bmp", L"image/4.bmp", L"image/5.bmp",
							L"image/6.bmp", L"image/7.bmp", L"image/8.bmp", L"image/9.bmp", L"image/10.bmp"};
HBITMAP animation_hBitamp[11];			//保存图片
int animation_index = 0;
const int animation_max = 11;
HDC hMemAnimation;

/* 精灵动画资源 */
typedef struct tagSpirit{
	int x, y;
	int direction;
}Spirit;
HBITMAP animation_spirit_hBitmap[4];
const TCHAR animation_spirit_res[4][15]= { L"image/11.bmp", L"image/22.bmp", L"image/33.bmp", L"image/44.bmp" };
const int animation_spirit_count = 10;
Spirit animation_spirit[10];

HDC hMemDC2;
HDC hMemDC;					//内存兼容DC
							/*
								阴影画刷的风格：
									HS_BDIAGONAL
									HS_CROSS
									HS_DIAGCROSS
									HS_FDIAGONAL
									HS_HORIZONTAL
									HS_VERTICAL
							*/

HDC g_hdc;
HWND g_hwnd;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdline){
	FWindow femy;
	int itNow = 0, itPre = 0;
	MSG msg = { 0 };
	femy.FInitWindowClass();
	femy.FSetWndProc(WndProc);
	femy.FSetIcon((HICON)LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE));
	femy.FRegisterWindowClass();
	femy.FCreateWindow();
	femy.FShowWindow(nCmdline);
	femy.TMoveWindow(100, 100, 800, 600);

	PlaySound(L"test.wav", NULL, SND_FILENAME | SND_ASYNC);
	GameInit(GetDC(femy.hWnd));


	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			itNow = GetTickCount();		//获取当前系统时间
			if (itNow - itPre >= 30)
			{
				//GameDraw(g_hdc);
				DrawAnimation(GetDC(msg.hwnd), animation_index);
				itPre = itNow;
			}
		}
	}
	GameDestroy();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	static HDC hdc;
	static PAINTSTRUCT ps;
	static const int time_id = 1000;
	switch (message)
	{
	case WM_CREATE:
		srand((signed)time(NULL));
		rand();

		g_hdc = GetDC(hwnd);
		g_hwnd = hwnd;
		SetTimer(hwnd, time_id, 100, NULL);
		break;
	case WM_TIMER:
		//	MessageBox(NULL, L"tiandao", L"error", MB_OK);
		//DrawAnimation(g_hdc, animation_index++);
		if (animation_index >= animation_max)
			animation_index = 0;
		else
			animation_index++;
		//ValidateRect(hwnd, NULL);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		//GameDraw(hdc);
		//GameDraw2(hdc);
		//DrawAnimation(hdc, animation_index++);
		EndPaint(hwnd, &ps);
		//InvalidateRect(hwnd, NULL, TRUE);
		//ValidateRect(hwnd, NULL);
		break;
	case WM_LBUTTONDOWN:				
		break;
	case WM_RBUTTONDOWN:				//鼠标右键按下

		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		KillTimer(hwnd, time_id);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
void GameInit(HDC hdc){
	/* GameDraw里的资源 */
	hPenSolid = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	hBrush = CreateSolidBrush(RGB(255, 0, 0));
	//hHatchBrush = CreateHatchBrush(HS_CROSS, RGB(0, 0, 255));
	hHatchBrush = CreateHatchBrush(HS_HORIZONTAL, RGB(0, 0, 125));
	hHatchBrush2 = CreateHatchBrush(HS_VERTICAL, RGB(0, 255, 0));
	hHatchBrush3 = CreateHatchBrush(HS_FDIAGONAL, RGB(255, 0, 0));
	hHatchBrush4 = CreateHatchBrush(HS_CROSS, RGB(125, 125, 255));

	hBitmapBackground = (HBITMAP)LoadImage(NULL, TEXT("image/Naruto.bmp"), IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);		//load bitmap
	hMemDC = CreateCompatibleDC(hdc);
	SelectObject(hMemDC, hBitmapBackground);
	/* GameDraw2里的资源 */
	hBitmapTransparent = (HBITMAP)LoadImage(NULL, TEXT("image/tiandao.bmp"), IMAGE_BITMAP, 640, 579, LR_LOADFROMFILE);
	hMemDC2 = CreateCompatibleDC(hdc);
	SelectObject(hMemDC2, hBitmapTransparent);
	/* 初始化动画资源 */
	for (animation_index = 0; animation_index < animation_max; animation_index++)
	{
		animation_hBitamp[animation_index] = (HBITMAP)LoadImage(NULL, animation_res[animation_index], IMAGE_BITMAP, 365, 200, LR_LOADFROMFILE);
	}
	animation_index = 0;
	hMemAnimation = CreateCompatibleDC(hdc);
}

void GameDraw(HDC hdc){
	RECT rect;

	SelectObject(hdc, hHatchBrush);		//将阴影画刷选到设备里去
	SelectObject(hdc, hPenSolid);

	BitBlt(hdc, 0, 0, 800, 600, hMemDC, 0, 0, SRCCOPY);
	
	rect.left = 300; rect.top = 0;
	rect.right = 400; rect.bottom = 100;
	FillRect(hdc, &rect, hHatchBrush4);

	MoveToEx(hdc, 10, 200, NULL);
	LineTo(hdc, 400, 200);
	SelectObject(hdc, hHatchBrush2);
	Rectangle(hdc, 400, 400, 500, 500);
	SelectObject(hdc, hHatchBrush3);
	Rectangle(hdc, 500, 400, 600, 500);

	SetFont(hdc, TEXT("微软雅黑"));
	SetTextColor(hdc, RGB(255, 0, 0));
	SetBkMode(hdc, TRANSPARENT);			//set text background aphle
	TextOut(hdc, 10, 220, _T("天道"), wcslen(_T("天道")));

	//BitBlt(hdc, 0, 0, 800, 600, hMemDC, 0, 0, SRCAND);
	//DrawText(hdc, L"fffff", 5, &rect, DT_TOP);
}
void GameDraw2(HDC hdc){
	BitBlt(hdc, 0, 0, 283, 567, hMemDC2, 283, 0, SRCAND);		//or
	BitBlt(hdc, 0, 0, 283, 567, hMemDC2, 0, 0, SRCPAINT);		//and
}
void GameDraw3(HDC hdc){
	//TransparentBlt(hdc, 800, 0, 320, 579, , 0, 0, 320, 579, RGB(255, 255, 255));			//已失效
}
void GameDestroy(){
	DeleteObject(hPenSolid);
	DeleteObject(hBrush);

	//Destroy hatch brush
	DeleteObject(hHatchBrush);
	DeleteObject(hHatchBrush2);
	DeleteObject(hHatchBrush3);
	DeleteObject(hHatchBrush4);
	DeleteDC(hMemDC);
	DeleteDC(hMemDC2);

	for (animation_index = 0; animation_index < animation_max; animation_index++)
	{
		DeleteObject(animation_hBitamp[animation_index]);										//删除设备DC
	}
	ReleaseDC(g_hwnd, g_hdc);
}


void DrawAnimation(HDC hdc, int index){
	if (index < animation_max)
	{
		SelectObject(hMemAnimation, animation_hBitamp[index]);
		BitBlt(hdc, 0, 0, 365, 200, hMemAnimation, 0, 0, SRCCOPY);
	}
}