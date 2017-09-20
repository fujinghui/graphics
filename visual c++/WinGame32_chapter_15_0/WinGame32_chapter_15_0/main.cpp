#include <DInput.h>
#include "fgame.h"
#include "femy.h"
#include "fclass.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT Direct3D_Init(HWND hwnd);
HRESULT Objects_Init(HWND hwnd);
HRESULT DirectInput_Init(HWND hwnd);
void Direct3D_Render(HWND hwnd);
char *GetDisplayInfo();
BOOL Device_Read(IDirectInputDevice8 *pDIDevice, void*pBuffer, long lSize);
void Direct3D_CleanUp();
double GetFPS();
//-------------
LPDIRECT3D9 pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
ID3DXFont *g_pFont = NULL;
HINSTANCE g_hinstance;
DInputClass *g_pDInput = NULL;
wchar_t display_info[60];
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
	FWindow femy;
	MSG msg = { 0 };
	femy.FInitWindowClass();
	femy.FSetWndProc(WndProc);
	femy.FSetHInstance(hInstance);
	femy.FSetIcon(NULL);					//(HICON)LoadImage(NULL, TEXT("icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
	femy.FRegisterWindowClass();
	femy.FCreateWindow();
	g_hinstance = hInstance;
	if (S_OK == Direct3D_Init(femy.hWnd))
	{
		;
	}
	else
	{
		MessageBox(NULL, TEXT("Direct3D Object Init Error!"), TEXT("error"), MB_OK);
	}
	femy.FShowWindow(nShowCmd);
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
	Direct3D_CleanUp();
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	D3DCAPS9 caps;
	D3DPRESENT_PARAMETERS d3dpp;

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
	wchar_t tempName[60];
	D3DADAPTER_IDENTIFIER9 Adapter;		//
	D3DCAPS9 caps;
	D3DPRESENT_PARAMETERS d3dpp;
	int vp = 0;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)								//获取pD3D对象
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
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
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
		MessageBox(NULL, TEXT("create device error"), TEXT("title"), MB_OK);
	}
	
	//获取显卡型号的信息
	pD3D->GetAdapterIdentifier(0, 0, &Adapter);
	int len = MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, display_info, len);

	SAFE_RELEASE(pD3D);

	g_pDInput = new DInputClass();
	if (S_OK != g_pDInput->Init(hwnd, g_hinstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))
	{
		
	}

	if (!(S_OK == Objects_Init(hwnd)))
	{
		return E_FAIL;
	}
	//g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);		//关闭光照
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//开启背面消隐
	return S_OK;
}
HRESULT Objects_Init(HWND hwnd){
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, TEXT("微软雅黑"), &g_pFont)))
		return E_FAIL;



	return S_OK;
}
void Direct3D_Render(HWND hwnd){
	RECT rect;
	
	GetClientRect(hwnd, &rect);
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(80, 80, 80), 1.0f, 0);
	g_pd3dDevice->BeginScene();

	g_pDInput->GetInput();
	if (g_pDInput->IsKeyDown(DIK_A))
	{
		//MessageBox(NULL, TEXT("title"), TEXT("tiandao"), MB_OK);

		g_pFont->DrawText(0, display_info, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 2555));
	}
	else
		g_pFont->DrawText(0, display_info, -1, &rect, DT_RIGHT | DT_TOP, D3DCOLOR_XRGB(255, 0, 255));
	if (g_pDInput->IsMouseButtonDown(0))
	{
		g_pFont->DrawText(0, display_info, -1, &rect, DT_RIGHT | DT_TOP, D3DCOLOR_XRGB(125, 0, 125));
	}
	if (g_pDInput->IsMouseButtonDown(1))
	{
		g_pFont->DrawText(0, display_info, -1, &rect, DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255, 255, 0));
	}
	if (g_pDInput->IsMouseButtonDown(2))
	{
		g_pFont->DrawText(0, display_info, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(0, 255, 255));
	}

	//处理鼠标事件
//	::ZeroMemory(&g_diMouseState, sizeof(g_diMouseState));
//	Device_Read(g_pMouseDevice, (LPVOID)&g_diMouseState, sizeof(g_diMouseState));
//	static FLOAT fPosX = 0.0f, fPosY = 30.0f, fPosZ = 0.0f;
//	if (g_diMouseState.rgbButtons[0] & 0x80)
//	{
//		fPosX += g_diMouseState.lX * 0.08f;
//		fPosY += g_diMouseState.lY * -0.08f;
//	}

	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}
double GetFPS(){
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

BOOL Device_Read(IDirectInputDevice8 *pDIDevice, void*pBuffer, long lSize)
{
	HRESULT hr;
	while (true)
	{
		pDIDevice->Poll();			//轮询设备
		pDIDevice->Acquire();		//获取设备的控制权
		if (SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer)))
			break;
		if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED)
			return FALSE;
		if (FAILED(pDIDevice->Acquire()))
			return FALSE;
	}
}


void Direct3D_CleanUp(){
}