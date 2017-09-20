#include "femy.h"
#include "fclass.h"
#include "fgame.h"

//#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT Direct3D_Init(HWND hwnd);
HRESULT Objects_Init(HWND hwnd);
void Direct3D_Render(HWND hwnd);
void Direct3D_CleanUp();
double Get_FPS();

/* �ӿڶ��� */
LPDIRECT3D9 pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;			//�豸����
ID3DXFont *g_pFont = NULL;
wchar_t string_fps[40];
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
	FWindow femy;
	MSG msg = { 0 };
	int itNow = 0, itPre = 0;
	femy.FInitWindowClass();
	femy.FSetWndProc(WndProc);
	femy.FSetHInstance(hInstance);
	femy.FSetIcon((HICON)LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE));
	femy.FRegisterWindowClass();
	femy.FCreateWindow();

	if (S_OK == Direct3D_Init(femy.hWnd))
		;// MessageBox(NULL, L"Direct3D Init Success!", L"title", MB_OK);
	else
		MessageBox(NULL, L"Direct3D Init Error!", L"title", MB_OK);


	femy.FShowWindow(nShowCmd);
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
			//itNow = GetTickCount();
			//if (itNow - itPre >= 20)
			{
				Direct3D_Render(femy.hWnd);
				
			//	itPre = itNow;
			}
		}
	}
	
	//GameDestroy(femy.hWnd);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message)
	{
	case WM_PAINT:
		Direct3D_Render(hwnd);
		ValidateRect(hwnd, NULL);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
			PostQuitMessage(0);
		}
		break;
	case WM_DESTROY:
		Direct3D_CleanUp();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}


HRESULT Direct3D_Init(HWND hwnd){
	
	D3DCAPS9 caps;
	D3DPRESENT_PARAMETERS d3dpp;
	int vp = 0;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		return E_FAIL;
	}
	//��ȡ�豸�ӿ�
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return E_FAIL;
	}
	//�ж���Ӳ����Ⱦ���������Ⱦ
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		//MessageBox(NULL, L"hardware", L"title", MB_OK);
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		//MessageBox(NULL, L"software", L"title", MB_OK);
	}
	//Direct3D��ʼ��֮����д����
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth				= WINDOW_WIDTH;
	d3dpp.BackBufferHeight				= WINDOW_HEIGHT;
	d3dpp.BackBufferFormat				= D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount				= 1;										//���ú�̨��ʽ��������
	d3dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality			= 0;
	d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow					= hwnd;
	d3dpp.Windowed						= true;
	d3dpp.EnableAutoDepthStencil		= true;
	d3dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;
	d3dpp.Flags							= 0;
	d3dpp.FullScreen_RefreshRateInHz	= 0;
	d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;
	//Direct3D��ʼ��֮�������豸
	//1��Ĭ���Կ� 2:�豸���� 3:���� 4:ģʽ������������Ӳ������ 5:����Direct3D����֮������������Ϣ
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &g_pd3dDevice)))
	{
		return  E_FAIL;
	}
	SAFE_RELEASE(pD3D);					//D3D����ʹ����ϣ��ͷ���Դ
	if (!(S_OK == Objects_Init(hwnd)))
		return E_FAIL;
	
	return S_OK;
}
HRESULT Objects_Init(HWND hwnd){
	//if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pFont)))
	//	return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pFont)))
		return E_FAIL;
	return S_OK;
}
void Direct3D_Render(HWND hwnd){
	//HDC hdc = GetDC(hwnd);
	RECT rect;
	rect.left = 0; rect.top = 0;
	rect.bottom = WINDOW_WIDTH; rect.right = WINDOW_HEIGHT;
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);			//���Ƶ�һ��������
	g_pd3dDevice->BeginScene();																//���Ƶڶ�������ʼ����





	//��ʾFPS
	swprintf_s(string_fps,14, _T("FPS:%.0f/s"), Get_FPS());
	g_pFont->DrawText(0, string_fps, -1, &rect, DT_RIGHT | DT_TOP, D3DCOLOR_XRGB(255, 255, 255));

	g_pd3dDevice->EndScene();																//��������
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

	//ReleaseDC(hwnd, hdc);
}
void Direct3D_CleanUp(){
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pd3dDevice);
}

double Get_FPS(){
	static double fps = 0;
	static int frameCount = 0;
	static double currentTime = 0.0;
	static double lastTime = 0.0f;
	frameCount++;
	currentTime = timeGetTime() * 0.001;
	if (currentTime - lastTime > 1.0)
	{
		fps = (double)frameCount;// / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}