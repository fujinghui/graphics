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

/* 接口对象 */
LPDIRECT3D9 pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;			//设备对象
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
	//获取设备接口
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return E_FAIL;
	}
	//判断是硬件渲染还是软件渲染
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
	//Direct3D初始化之：填写内容
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth				= WINDOW_WIDTH;
	d3dpp.BackBufferHeight				= WINDOW_HEIGHT;
	d3dpp.BackBufferFormat				= D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount				= 1;										//设置后台链式缓冲数量
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
	//Direct3D初始化之：创建设备
	//1：默认显卡 2:设备类型 3:窗口 4:模式，软件计算或者硬件计算 5:创建Direct3D对象之后填充的数据信息
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &g_pd3dDevice)))
	{
		return  E_FAIL;
	}
	SAFE_RELEASE(pD3D);					//D3D对象使用完毕，释放资源
	if (!(S_OK == Objects_Init(hwnd)))
		return E_FAIL;
	
	return S_OK;
}
HRESULT Objects_Init(HWND hwnd){
	//if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pFont)))
	//	return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pFont)))
		return E_FAIL;
	return S_OK;
}
void Direct3D_Render(HWND hwnd){
	//HDC hdc = GetDC(hwnd);
	RECT rect;
	rect.left = 0; rect.top = 0;
	rect.bottom = WINDOW_WIDTH; rect.right = WINDOW_HEIGHT;
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);			//绘制第一步：清屏
	g_pd3dDevice->BeginScene();																//绘制第二部：开始绘制





	//显示FPS
	swprintf_s(string_fps,14, _T("FPS:%.0f/s"), Get_FPS());
	g_pFont->DrawText(0, string_fps, -1, &rect, DT_RIGHT | DT_TOP, D3DCOLOR_XRGB(255, 255, 255));

	g_pd3dDevice->EndScene();																//结束绘制
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