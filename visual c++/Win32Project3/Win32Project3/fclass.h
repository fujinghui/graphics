#ifndef _FCLASS_H_
#define _FCLASS_H_

#include "femy.h"
class FWindow{
public:
	FWindow(){
	}
	~FWindow(){
		//Destory window class
		UnregisterClass(_T("femy"), this->wnex.hInstance);
	}
	void FInitWindowClass(){
		wnex.cbSize = sizeof(WNDCLASSEX);
		wnex.style = CS_HREDRAW | CS_VREDRAW;
		wnex.lpfnWndProc = NULL;
		wnex.cbClsExtra = 0;			//窗口类附加内存
		wnex.cbWndExtra = 0;
		wnex.hInstance = hInstance;
		wnex.hIcon = NULL;				//没有图标
		wnex.hCursor = LoadCursor(NULL, IDC_ARROW);			//鼠标 LoadCursor(_In_opt_ HINSTANCE hInstance, _In_opt_ LPCTSTR lpCursorName);
		wnex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		//背景画刷
		wnex.lpszMenuName = NULL;		//不需要菜单
		wnex.lpszClassName = _T("femy");	//窗口类的名称
	}
	void FRegisterWindowClass(void){
		if (!RegisterClassEx(&wnex))
			MessageBox(NULL, L"error", L"title", MB_OK);
	}
	void FCreateWindow(void){
		//::CreateWindow();
		this->hWnd = CreateWindow(_T("femy"), _T("femy"), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, this->wnex.hInstance, NULL);
		if (this->hWnd == NULL)
			MessageBox(NULL, L"Create Windows Error", L"title", MB_OK);
	}
	void FShowWindow(_In_ int nCmdShow){
		ShowWindow(this->hWnd, nCmdShow);
		UpdateWindow(this->hWnd);
	}
	void FSetClassName(LPCTSTR name){
		this->wnex.lpszClassName = name;
	}
	void FSetIcon(HICON hIcon){
		this->wnex.hIcon = hIcon;
	}
	void FSetHInstance(HINSTANCE hInstance){
		this->hInstance = hInstance;
		this->wnex.hInstance = hInstance;
	}
	void FSetWndProc(WNDPROC WndProc){
		this->wnex.lpfnWndProc = WndProc;
	}
	void FSetCursor(HCURSOR hCursor){
		this->wnex.hCursor = hCursor;
	}
	void FSetBackgroundHBrush(HBRUSH hBrush){
		this->wnex.hbrBackground = hBrush;
	}
	//功能函数
	void TMoveWindow(int x, int y, int width, int height){
		MoveWindow(this->hWnd, x, y, width, height, true);
	}
	void TMoveWindow(int x, int y){
		MoveWindow(this->hWnd, x, y, CW_USEDEFAULT, CW_USEDEFAULT, true);
	}


	HWND hWnd;
private:
	HINSTANCE hInstance;
	WNDCLASSEX wnex;
};

#endif