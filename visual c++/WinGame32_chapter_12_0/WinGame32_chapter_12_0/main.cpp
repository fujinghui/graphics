#include "femy.h"
#include "fclass.h"
#include "fgame.h"

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

LPDIRECT3D9 pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
ID3DXFont *g_pFont = NULL;
//顶点显示变量
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;

//FVertex pVertices[6];
wchar_t string_fps[40];

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
	FWindow femy;
	MSG msg = { 0 };
	int itNow = 0, itPre = 0;
	femy.FInitWindowClass();
	femy.FSetWndProc(WndProc);
	femy.FSetHInstance(hInstance);
	femy.FSetIcon((HICON)LoadImage(NULL, _T("icon.cio"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
	femy.FRegisterWindowClass();
	femy.FCreateWindow();
	if (S_OK == Direct3D_Init(femy.hWnd))
		;
	else
		MessageBox(NULL, _T("error"), _T("prompt"), MB_OK);
	femy.FShowWindow(nShowCmd);
	femy.TMoveWindow(100, 100, WINDOW_WIDTH, WINDOW_HEIGHT);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Direct3D_Render(femy.hWnd);
		}
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){
	case WM_PAINT:
		Direct3D_Render(hwnd);
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
		break;
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
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return E_FAIL;
	}
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	//Direct3D初始化之：填写内容
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;										//设置后台链式缓冲数量
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;



	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &g_pd3dDevice)))
	{
		MessageBox(NULL, L"t", L"t", MB_OK);
		return E_FAIL;
	}

	SAFE_RELEASE(pD3D);
	if (!(S_OK == Objects_Init(hwnd)))
		return E_FAIL;
	return S_OK;
}

HRESULT Objects_Init(HWND hwnd){
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pFont)))
		return E_FAIL;

	//创建顶点缓存
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(6 * sizeof(FVertex), 0,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL)))
	{
		return E_FAIL;
	}

	FVertex vertices[6] = { 
		{ 300.0f, 100.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 0) },
		{ 500.0F, 100.0f, 0.0f, 1.0F, D3DCOLOR_XRGB(0, 255, 0) },
		{ 300.0f, 300.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 255, 0) },
		{ 300.0f, 300.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 0) },
		{ (float)(800.0 * rand() / (RAND_MAX + 1.0)), (float)(600.0 * rand() / (RAND_MAX + 1.0)), 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256)},
		{ (float)(800.0 * rand() / (RAND_MAX + 1.0)), (float)(600.0 * rand() / (RAND_MAX + 1.0)), 0.0f, 1.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256)}
	};
	FVertex *pVertices;
	if(FAILED(g_pVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))		//lock(其实就是获取D3D顶点内存的地址)
		return E_FAIL;
	memcpy(pVertices, vertices, sizeof(vertices));
	
	g_pVertexBuffer->Unlock();															//un lock
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, false);								//关闭背面渲染
	return S_OK;
}
void Direct3D_Render(HWND hwnd){
	RECT rect;
	rect.left = 0; rect.right = WINDOW_WIDTH;
	rect.top = 0; rect.bottom = WINDOW_HEIGHT;
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	g_pd3dDevice->BeginScene();

	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(FVertex));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);



	swprintf_s(string_fps, 14, _T("FPS:%.1f/s"), Get_FPS());
	g_pFont->DrawText(0, string_fps, -1, &rect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 255, 255));
	
	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Direct3D_CleanUp(){
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pd3dDevice);
}

double Get_FPS(){
	static double fps = 0;
	static int frameCount = 0;
	static double currentTime = 0;
	static double lastTime = 0.0;
	frameCount++;
	currentTime = timeGetTime() * 0.001;
	if (currentTime - lastTime > 1.0)
	{
		fps = (double)frameCount;
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}