
#include <windows.h>
#include <gdiplus.h>			//图像库 将bmp转换成png

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void ScreenCapture();
void CopyBitmapToCipBoard(RECT *rect);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
	WNDCLASS wnex;
	MSG msg;
	HWND hwnd;
	char sTitle[] = "Screen";
	char sClass[] = "femy";
	wnex.style = CS_HREDRAW | CS_VREDRAW;
	wnex.hInstance = hInstance;
	wnex.cbClsExtra = 0;
	wnex.cbWndExtra = 0;
	wnex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnex.hIcon = NULL;
	wnex.lpfnWndProc = WndProc;
	wnex.lpszClassName = sClass;
	wnex.lpszMenuName = NULL;

	if (!RegisterClass(&wnex))
		return 0;
	hwnd = CreateWindow(sClass, sTitle, WS_DLGFRAME | WS_POPUP, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);
	//WS_DLGFRAME | WS_POPUP
	if (hwnd == NULL)
		return 0;
	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
	//ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
HDC hdc;
int width, height;
HDC hdcMem;
HBITMAP hbitMap;
HDC disDc;
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	static HDC hdcWindow = NULL;
	PAINTSTRUCT ps;
	static RECT capture_rect = {0, 0, 0, 0};
	static RECT capture_rect_temp = { 0, 0, 0, 0 };
	POINT pt;
	static BOOL show_rect_state = FALSE;
	LOGBRUSH brush;

	brush.lbStyle = BS_NULL;
	HBRUSH hBrush = CreateBrushIndirect(&brush);
	switch (message){
	case WM_CREATE:
		capture_rect.left = 0; capture_rect.top = 0; capture_rect.right = 0; capture_rect.bottom = 0;
		disDc = CreateDC("DISPLAY", 0, 0, 0);
		width = GetDeviceCaps(disDc, HORZRES);		//获取桌面宽度
		height = GetDeviceCaps(disDc, VERTRES);		//获取桌面高度

		//获取桌面截图
		hdcMem = CreateCompatibleDC(disDc);		//创建一个兼容DC
		hbitMap = CreateCompatibleBitmap(disDc, width, height);
		SelectObject(hdcMem, hbitMap);

		BitBlt(hdcMem, 0, 0, width, height, disDc, 0, 0, SRCCOPY);

		SetTimer(hwnd, 0, 100, NULL);
		break;
	case WM_KEYDOWN:
		if (LOWORD(wParam) == VK_ESCAPE)
			PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		GetCursorPos(&pt);
		capture_rect.left = pt.x;
		capture_rect.top = pt.y;
		show_rect_state = FALSE;
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		GetCursorPos(&pt);
		capture_rect.right = pt.x;
		capture_rect.bottom = pt.y;

		capture_rect_temp.left = min(capture_rect.left, capture_rect.right);
		capture_rect_temp.top = min(capture_rect.top, capture_rect.bottom);
		capture_rect_temp.right = max(capture_rect.left, capture_rect.right);
		capture_rect_temp.bottom = max(capture_rect.top, capture_rect.bottom);
		memcpy(&capture_rect, &capture_rect_temp, sizeof(RECT));
		
		CopyBitmapToCipBoard(&capture_rect);

		show_rect_state = TRUE;
		break;
	case WM_TIMER:
		//ScreenCapture();
		//将桌面画到窗口中
		hdc = GetDC(hwnd);
		BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);


		SelectObject(hdc, hBrush);
		if (show_rect_state)
			Rectangle(hdc, capture_rect.left, capture_rect.top, capture_rect.right, capture_rect.bottom);
		ReleaseDC(hwnd, hdc);


		break;
	case WM_DESTROY:
		KillTimer(hwnd, 0);
		DeleteDC(hdcMem);
		DeleteDC(disDc);
		DeleteObject(hbitMap);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;

	}

	return 0;
}

//对屏幕进行截取
void ScreenCapture(){
	
	
}





void CopyBitmapToCipBoard(RECT *rect){
	
	int width = rect->right - rect->left;
	int height = rect->bottom - rect->top;
	HDC hdcMem = CreateCompatibleDC(disDc);
	HBITMAP bmp = CreateCompatibleBitmap(disDc, width, height);
	HBITMAP oldBmp = (HBITMAP)SelectObject(hdcMem, bmp);

	BitBlt(hdcMem, 0, 0, width, height, disDc, rect->left, rect->top, SRCCOPY);
	

	HBITMAP newBmp = (HBITMAP)SelectObject(hdcMem, oldBmp);
	if (OpenClipboard(NULL))
	{
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, newBmp);
		CloseClipboard();
	}
	
}