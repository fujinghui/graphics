#include "femy.h"
#include "fclass.h"
#include <gl/glut.h>
#include "CGfxOpengl.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void GameLoop();
void SetupPixelFormat(HDC hdc);

typedef struct tagCharacter{
	int x, y;
	int mx, my;
}Character;

Character character;
HBITMAP hBitmap_Character;
HBITMAP hBitmap_Background;
HDC hMemDC, hMemTempDC;
HBITMAP bmp;

CGfxOpenGL g_glRender;


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
				itPre = itNow;
				GameLoop();
			}
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	static HDC hdc;
	static HGLRC hrc;
	static PIXELFORMATDESCRIPTOR pfd;
	switch (message){
	case WM_CREATE:
		hdc = GetDC(hwnd);
		SetupPixelFormat(hdc);
		hrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hrc);			//使当前的渲染环境生效
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
			PostQuitMessage(0);
		}
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_SYSKEYDOWN:

		break;
	case WM_SYSKEYUP:

		break;
	case WM_SIZE:

		break;
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
		wglMakeCurrent(hdc, NULL);			//取消渲染环境
		wglDeleteContext(hrc);				//删除渲染环境
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}
void GameLoop(){
	HGLRC hrc = wglGetCurrentContext();
}
void SetupPixelFormat(HDC hdc){
	int pixelFormat;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER |
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0,
	};
	pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);
}