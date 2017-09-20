#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "myfunction.h"


char *text[100] = { "fujinghui", "tiandajafdsjfajdslkfjals;kdfja", "fsafadjklafjfdsa",
"afjajdfklajdkfad", "ftiaaddfdadfasdfasdf", "fajdsfutafhaskjdhakjsjdkfaj",
"������--------����������", "����ʲô��������������", "fasjdlfkajslkdfjalkdf",
"faslkdjfafasdjlkfajdklads", "fajdflajdflkajdsflkajsdlkfjal", "fdffffffffffff",
"falsdjflkajsdlkfjakldfjalksdfjakl", "fujinghuifujinghuifujinghui", "fadsfaoidfjajdflkajdfiia" };
char t[100];
int DATA_LINE = 15;		//ԭʼ�����ж����� 
/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static int WINDOWS_WIDTH = 0;
	static int WINDOWS_HEIGHT = 0;
	static RECT rect;

	//��������йصı��� 
	static SCROLLINFO si;
	static int scrollCode = 0;
	static int currentScrollPosition = 0;
	//��������Ϣ�йصı���
	static TEXTMETRIC tm;
	static int cxChar, cxCaps, cyChar; /*
									   *cxChar:ƽ���ַ����
									   *cxCaps:��д��ĸƽ�����
									   *cyChar:�ַ��ĸ߶�
									   */
	//�봰���йصı���
	static HDC hdc;
	static HPEN hPen;
	//�ڴ���WM_PAINT��Ϣʱ���õ��ı���
	static PAINTSTRUCT ps;
	static int i, x, y;

	switch (Message) {
		//���ڴ�����ʱ���������Ϣ
	case WM_CREATE:
		si.cbSize = sizeof(si);
		hdc = GetDC(hwnd);
		//��ȡϵͳ������Ϣ 
		GetTextMetrics(hdc, &tm);	//��ȡϵͳ������Ϣ
		cxChar = tm.tmAveCharWidth;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		//���û���
		//	hPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
		//	SelectObject(hdc, hPen);


		ReleaseDC(hwnd, hdc);
		break;
		//���ڸı���Ϣ 
	case WM_SIZE:
		GetClientRect(hwnd, &rect);
		WINDOWS_WIDTH = LOWORD(lParam);
		WINDOWS_HEIGHT = HIWORD(lParam);
		//			sprintf(t, "width:%d\nheight:%d\n", WINDOWS_WIDTH, WINDOWS_HEIGHT);
		//			OutDebug("what!");
		break;
		//����ֱ�����������¼� 
	case WM_VSCROLL:
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT, &si);	//����й��ڹ���������Ϣ 
		scrollCode = LOWORD(wParam);	//��һ����ȡ��������״̬��Ϣ
		currentScrollPosition = si.nPos;	//��¼��ǰ��������λ�� 
		switch (scrollCode)
		{
		case SB_TOP:		//�������Ѿ�������� 
			si.nPos = si.nMin;	 break;
		case SB_BOTTOM:		//�������Ѿ�������Ͳ� 
			si.nPos = si.nMax; 	 break;
		case SB_LINEUP:		//���Ϸ�һ��
			si.nPos -= 1;	     break;
		case SB_LINEDOWN:	//���·�һ��
			si.nPos += 1;		 break;
		case SB_PAGEUP:		//���Ϸ���һҳ
			si.nPos -= si.nPage; break;
		case SB_PAGEDOWN:	//���·���һҳ 
			si.nPos += si.nPage; break;
		case SB_THUMBTRACK:	//�ƶ�����
			si.nPos = si.nTrackPos; break;
		default:
			break;
		}
		//�������ù�������Ϣ
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);	//���¹�������λ��
		if (si.nPos != currentScrollPosition)		//������º�Ĺ�������λ�ò�����֮ǰ�Ĺ�����λ�ã���ô������ʾ���ݵ�λ�� 
		{
			//	setScrollPos(hwnd, SB_HORZ, si.nPos, TRUE);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
		//����ˮƽ�����������¼�
	case WM_HSCROLL:
		//	GetScrollInfo(hwnd, SB_HORZ, &si);	//��ȡ�й��ڹ���������Ϣ 
		si.fMask = SIF_ALL;
		scrollCode = LOWORD(wParam);	//��һ����ȡ��������״̬��Ϣ
		currentScrollPosition = si.nPos;	//��¼��ǰ��������λ�� 
		switch (scrollCode)
		{
		case SB_TOP:		//�������Ѿ�������� 
			si.nPos = si.nMin;	 break;
		case SB_BOTTOM:		//�������Ѿ�������Ͳ� 
			si.nPos = si.nMax; 	 break;
		case SB_LINEUP:		//���Ϸ�һ��
			si.nPos -= 1;	     break;
		case SB_LINEDOWN:	//���·�һ��
			si.nPos += 1;		 break;
		case SB_PAGEUP:		//���Ϸ���һҳ
			si.nPos -= si.nPage; break;
		case SB_PAGEDOWN:	//���·���һҳ 
			si.nPos += si.nPage; break;
		case SB_THUMBTRACK:	//�ƶ�����
			si.nPos = si.nTrackPos; break;
		default:
			break;
		}
		//	SetScrollPos(hwnd, SB_HORZ, si.nPos, TRUE);
		break;
		//��ʾ���� 
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SetTextColor(hdc, 0x00ff0000);
		for (i = 0; i < DATA_LINE; i++)
		{
			y = cxChar * (i)-si.nPos;
			//	x = cxChar * (i - si.nPos);
			TextOut(hdc, x, y, text[i], strlen(text[i]));
		}
		EndPaint(hwnd, &ps);
		break;
		/* Upon destruction, tell the main thread to stop */
	case WM_DESTROY: {
						 PostQuitMessage(0);
						 break;
	}
		/* All other messages (a lot of them) are processed using default procedures */
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */
	TCHAR windowsClass[] = "WindowClass";
	/* zero out the struct and set the stuff we want to modify */
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc; /* This is where we will send messages to */
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = windowsClass;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, windowsClass, "Caption", WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		640, /* width */
		480, /* height */
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	//	sprintf(t, "length:%d\n", strlen(text[1]));
	//	OutDebug(t);
	/*
	This is the heart of our program where all input is processed and
	sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
	this loop will not produce unreasonably high CPU usage
	*/
	while (GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	return msg.wParam;
}