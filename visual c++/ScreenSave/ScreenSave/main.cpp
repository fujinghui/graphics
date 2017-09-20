
#include <windows.h>
#define STRMAXLEN 30
#define STRMINLEN 12
typedef struct tagCharColumn{
	TCHAR str[STRMAXLEN + 1];
	int x, y, iStrlen;
	int r, g, b;
	int iStopTiems, iMustStopTimes;
}CharColumn, *pCharColumn;

TCHAR *getGBK(){
	TCHAR *str = (TCHAR *)calloc(2, sizeof(TCHAR));
	*str = rand() % 128; *str |= 0x80;				//第一个字节必须小于0
	*(str + 1) = rand() % 256; *(str+1) |= 0x40;		//0100 0000 = 64 也就是说必须要大于63或者小于0
	return str;
}
TCHAR getGBK1(){
	return ((TCHAR)(rand() % 128)) | 0x80;
}
TCHAR getGBK2(){
	return ((TCHAR)(rand() % 256)) | 0x40;
}
void init(CharColumn *cc, int cyScreen, int x){
	int j;
	cc->iStrlen = rand() % (STRMAXLEN - STRMINLEN) + STRMINLEN; //设置随机字符的大小
	cc->x = x + 3;
	cc->y = rand() % 3 ? rand() % cyScreen : 0;
	cc->iMustStopTimes = rand() % 6;
	cc->iStopTiems = 0;
	cc->r = rand() % 256;
	cc->g = rand() % 256;
	cc->b = rand() % 256;
	for (j = 0; j < cc->iStrlen; j++)
	{
		cc->str[j] = (TCHAR)(rand() % (126 - 33) + 33);
	//	cc->str[j][1] = getGBK1();
	//	cc->str[j][0] = getGBK2();
	}
	return;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
	WNDCLASS wnex;
	HWND hwnd;
	TCHAR *scAppName = TEXT("My Screen Protected");
	TCHAR *scClassName = TEXT("femy_screen");
	MSG msg;

	wnex.style = CS_VREDRAW | CS_HREDRAW;
	wnex.lpfnWndProc = WndProc;
	wnex.cbClsExtra = 0;
	wnex.cbWndExtra = 0;
	wnex.hInstance = hInstance;
	wnex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wnex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnex.lpszMenuName = NULL;
	wnex.lpszClassName = scClassName;
	if (!RegisterClass(&wnex))
	{
		return 0;
	}
	hwnd = CreateWindow(scClassName, scAppName, WS_DLGFRAME | WS_POPUP,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
	if (hwnd == NULL)
	{
		return 0;
	}
	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hwnd);
	ShowCursor(FALSE);
	srand((int)GetCurrentTime());
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);				//转换消息
		DispatchMessage(&msg);				//发送消息
	}
	ShowCursor(TRUE);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){

	HDC hdc;
	int i, j;
	static HDC hdcMem;
	HFONT hFont;
	static HBITMAP hBitmap;
	static int iFontWidth = 10, iFontHeight = 15, iColumnCount;
	static int cxScreenWidth = 0, cyScreenHeight = 0;
	static int ID_TIMER = 1001;
	static CharColumn *ccChain;

	TCHAR temp, temp1, temp2;
	switch (message){
	case WM_CREATE:
			SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hwnd, RGB(125, 125, 125), 255, LWA_ALPHA | LWA_COLORKEY);
		
		cxScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		cyScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		//注册定时器
		SetTimer(hwnd, ID_TIMER, 10, NULL);

		hdc = GetDC(hwnd);
		hdcMem = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, cxScreenWidth, cyScreenHeight);
		SelectObject(hdcMem, hBitmap);
		ReleaseDC(hwnd, hdc);

		//创建字体
		hFont = CreateFont(iFontHeight, iFontWidth - 4, 0, 0, FW_BOLD, 0, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, FIXED_PITCH | FF_SWISS, TEXT("ZS FIXED"));
		SelectObject(hdcMem, hFont);
		DeleteObject(hFont);
		SetBkMode(hdcMem, TRANSPARENT);
		
		iColumnCount = cxScreenWidth / (iFontWidth * 3 / 2);	//计算按照当前屏幕宽度，应该出现多少列字符串
		ccChain = (pCharColumn)calloc(iColumnCount, sizeof(CharColumn));
		for (i = 0; i < iColumnCount; i++)
		{
			init(ccChain + i, cyScreenHeight, (iFontWidth * 3 / 2) * i);
		}
	case WM_TIMER:
		hdc = GetDC(hwnd);
		PatBlt(hdcMem, 0, 0, cxScreenWidth, cyScreenHeight, WHITENESS);						//将屏幕显示成黑色
		//显示字符串
		for (i = 0; i < iColumnCount; i++)
		{
			SetTextColor(hdcMem, RGB(255, 255, 255));
			TextOut(hdcMem, (ccChain + i)->x, (ccChain + i)->y, &((ccChain + i)->str[0]), 1);
			//TextOut(hdcMem, (ccChain + i)->x, (ccChain + i)->y, "0", 2);
			for (j = 1; j < ccChain[i].iStrlen; j++)
			{
				//SetTextColor(hdcMem, RGB(0, 255 - (j * 255 / (ccChain + i)->iStrlen), 0));
				SetTextColor(hdcMem, 
					RGB(
					(ccChain + i)->r - (j * (ccChain + i)->r / (ccChain + i)->iStrlen), 
					(ccChain + i)->g - (j * (ccChain + i)->g / (ccChain + i)->iStrlen), 
					(ccChain + i)->b - (j * (ccChain + i)->b / (ccChain + i)->iStrlen)
					));
				//TextOut(hdcMem, (ccChain + i)->x, (ccChain + i)->y - j * iFontHeight, TEXT("*"), 1);
				TextOut(hdcMem, (ccChain + i)->x, (ccChain + i)->y - j * iFontHeight, &((ccChain + i)->str[j]), 1);
			}
			temp = (ccChain + i)->iStopTiems++ >(ccChain + i)->iMustStopTimes;
			if (temp)
			{
				(ccChain + i)->iStopTiems = 0;
			}
			else
				continue;
			(ccChain + i)->y += iFontHeight;// / 5 + rand() % (iFontHeight / 5 * 4);// (ccChain + i)->iMustStopTimes);
			if ((ccChain + i)->y - iFontHeight * (ccChain + i)->iStrlen > cyScreenHeight)
			{
				init(ccChain + i, cyScreenHeight, (ccChain + i)->x);
			}

			/*
			temp1 = (ccChain + i)->str[ccChain[i].iStrlen - 1][0];
			temp2 = (ccChain + i)->str[ccChain[i].iStrlen - 1][1];
			for (j = ccChain[i].iStrlen - 1; j > 0; j--)
			{
				(ccChain + i)->str[j][0] = (ccChain + i)->str[j - 1][0];
				(ccChain + i)->str[j][1] = (ccChain + i)->str[j - 1][1];
			}
			(ccChain + i)->str[0][0] = temp1;
			(ccChain + i)->str[0][1] = temp2;
			*/
			temp = (ccChain + i)->str[ccChain[i].iStrlen - 1];
			for (j = ccChain[i].iStrlen - 1; j > 0; j--)
			{
				(ccChain + i)->str[j] = (ccChain + i)->str[j - 1];
			}
			(ccChain + i)->str[0] = temp;
			
		}
		SetBkMode(hdc, TRANSPARENT);
		BitBlt(hdc, 0, 0, cxScreenWidth, cyScreenHeight, hdcMem, 0, 0, SRCCOPY);

		ReleaseDC(hwnd, hdc);

		break;
	case WM_KEYUP:
	case WM_LBUTTONUP:
	case WM_DESTROY:

		KillTimer(hwnd, ID_TIMER);
		DeleteObject(hBitmap);
		DeleteDC(hdcMem);
		free(ccChain);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return TRUE;
}
