#include "femy.h"
#include "fclass.h"
#include "fgame.h"			//定义游戏常用结构体

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT Direct3D_Init(HWND hwnd);
HRESULT Objects_Init(HWND hwnd);
void Direct3D_Render(HWND hwnd);
void Direct3D_CleanUp();
double GetFPS();
void Matrix_Set();
void Light_Init();
void Light_Set(LPDIRECT3DDEVICE9 pd3dDevice, UINT nType);
void Material_Init();

LPDIRECT3D9 pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
ID3DXFont *g_pFont = NULL;
//顶点显示变量
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
LPDIRECT3DINDEXBUFFER9 g_pIndexBuffer = NULL;
//显示帧率的变量
wchar_t string_fps[40];
//与变换有关的变量

//物体变量
LPD3DXMESH g_teapot = NULL;		//茶壶对象
LPD3DXMESH g_cube = NULL;		//立方体对象
LPD3DXMESH g_sphere = NULL;		//球体对象
LPD3DXMESH g_torus = NULL;		//圆环对象
D3DXMATRIX g_WorldMatrix[4], R;	//定义一些世界矩阵

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
	FWindow femy;
	MSG msg = { 0 };
	femy.FInitWindowClass();
	femy.FSetWndProc(WndProc);
	femy.FSetHInstance(hInstance);
	femy.FSetIcon(NULL);					//(HICON)LoadImage(NULL, TEXT("icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
	femy.FRegisterWindowClass();
	femy.FCreateWindow();
	if (S_OK == Direct3D_Init(femy.hWnd))
		;
	else
		MessageBox(NULL, TEXT("Direct3D Object Create Falid"), TEXT("error"), MB_OK);
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
void Direct3D_Render(HWND hwnd){
	RECT rect;
	//立方体
	D3DXMatrixRotationY(&R, timeGetTime() / 720.0f);
	D3DXMatrixTranslation(&g_WorldMatrix[0], 3.0f, -3.0f, 0.0f);
	g_WorldMatrix[0] = g_WorldMatrix[0] * R;

	//茶壶的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[1], -3.0f, -3.0f, 0.0f);
	g_WorldMatrix[1] = g_WorldMatrix[1] * R;
	
	//圆环的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[2], 3.0f, 3.0f, 0.0f);
	g_WorldMatrix[2] = g_WorldMatrix[2] * R;
	
	//球体的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[3], -3.0f, 3.0f, 0.0f);
	g_WorldMatrix[3] = g_WorldMatrix[3] * R;

	GetClientRect(hwnd, &rect);
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	//规范化法向量
	g_pd3dDevice->BeginScene();
	Matrix_Set();
	if (::GetAsyncKeyState(0x31) & 0x8000f)
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	if (::GetAsyncKeyState(0x32) & 0x8000f)
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState(0x51) & 0x8000f)
	{
		Light_Set(g_pd3dDevice, 1);
	}
	if (::GetAsyncKeyState(0x57) & 0x8000f)
	{
		Light_Set(g_pd3dDevice, 2);
	}
	if (::GetAsyncKeyState(0x45) & 0x8000f)
	{
		Light_Set(g_pd3dDevice, 3);
	}
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[0]);
	g_cube->DrawSubset(0);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[1]);
	g_teapot->DrawSubset(0);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[2]);
	g_torus->DrawSubset(0);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[3]);
	g_sphere->DrawSubset(0);
	//g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);	
	//g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(35, 35, 35));			//设置环境光

	//g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);						//开启镜面反射

	//g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof()
	swprintf_s(string_fps, 14, TEXT("FPS:%.1f/s"), GetFPS());
	g_pFont->DrawText(0, string_fps, -1, &rect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 255, 255));
	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	
}

HRESULT Direct3D_Init(HWND hwnd){
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
	SAFE_RELEASE(pD3D);
	if (!(S_OK == Objects_Init(hwnd)))
		return E_FAIL;
	//g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);		//关闭光照
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//开启背面消隐
	return S_OK;
}

HRESULT Objects_Init(HWND hwnd){
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, TEXT("微软雅黑"), &g_pFont)))
		return E_FAIL;
	if (FAILED(D3DXCreateBox(g_pd3dDevice, 2, 2, 2, &g_cube, NULL)))
	{
		return E_FAIL;
	}
	if (FAILED(D3DXCreateTeapot(g_pd3dDevice, &g_teapot, NULL)))
	{
		return E_FAIL;
	}
	if (FAILED(D3DXCreateSphere(g_pd3dDevice, 1.5, 25, 25, &g_sphere, NULL)))
	{
		return E_FAIL;
	}
	if (FAILED(D3DXCreateTorus(g_pd3dDevice, 0.5f, 1.2f, 25, 25, &g_torus, NULL)))
	{
		return E_FAIL;
	}
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);					//关闭光照
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	
	Material_Init();
	Light_Init();
	return S_OK;
}

void Direct3D_CleanUp(){
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);	
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pd3dDevice);

	//销毁物体
	SAFE_RELEASE(g_torus);
	SAFE_RELEASE(g_sphere);
	SAFE_RELEASE(g_cube);
	SAFE_RELEASE(g_teapot);
}

void Matrix_Set(){
	//--------------------------------------------------------------------------------------
	//【四大变换之一】：世界变换矩阵的设置
	//--------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------
	//【四大变换之二】：取景变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matView; //定义一个矩阵
	D3DXVECTOR3 vEye(0.0f, 0.0f, -15.0f);  //摄像机的位置
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //观察点的位置
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//向上的向量
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //计算出取景变换矩阵
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView); //应用取景变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之三】：投影变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matProj; //定义一个矩阵
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f); //计算投影变换矩阵
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);  //设置投影变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之四】：视口变换的设置
	//--------------------------------------------------------------------------------------
	D3DVIEWPORT9 vp; //实例化一个D3DVIEWPORT9结构体，然后做填空题给各个参数赋值就可以了
	vp.X = 0;		//表示视口相对于窗口的X坐标
	vp.Y = 0;		//视口相对对窗口的Y坐标
	vp.Width = WINDOW_WIDTH;	//视口的宽度
	vp.Height = WINDOW_HEIGHT; //视口的高度
	vp.MinZ = 0.0f; //视口在深度缓存中的最小深度值
	vp.MaxZ = 1.0f;	//视口在深度缓存中的最大深度值
	g_pd3dDevice->SetViewport(&vp); //视口的设置
}
void Material_Init(){
	D3DMATERIAL9 material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.7f, 1.0f);
	material.Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
	material.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	material.Emissive = D3DXCOLOR(0.3f, 0.0f, 0.1f, 1.0f);

	g_pd3dDevice->SetMaterial(&material);						//设置当前环境的材质
	//g_pd3dDevice->GetMaterial(&material);						//过去当前环境的材质
}
void Light_Init(){
	D3DLIGHT9 light_point;
	D3DLIGHT9 light_direction;
	D3DLIGHT9 light_spot;
	//初始化点光源
	/*
	ZeroMemory(&light_point, sizeof(light_point));
	light_point.Type = D3DLIGHT_POINT;
	light_point.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	light_point.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light_point.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	light_point.Position = D3DXVECTOR3(0.0f, 200.0f, 0.0f);
	light_point.Attenuation0 = 1.0f;
	light_point.Attenuation1 = 0.0f;
	light_point.Attenuation2 = 0.0f;
	light_point.Range = 300.0f;
	g_pd3dDevice->SetLight(0, &light_point);						//初始化0号灯光	
	g_pd3dDevice->LightEnable(0, true);						//开启0号灯光
	
	//初始化方向光
	ZeroMemory(&light_direction, sizeof(light_direction));
	light_direction.Type = D3DLIGHT_DIRECTIONAL;
	light_direction.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	light_direction.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.f, 1.0f);
	light_direction.Specular = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	light_direction.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	g_pd3dDevice->SetLight(1, &light_direction);
	g_pd3dDevice->LightEnable(1, true);
	
	//初始化聚光灯
	ZeroMemory(&light_spot, sizeof(light_spot));
	light_spot.Type = D3DLIGHT_SPOT;
	light_spot.Position = D3DXVECTOR3(100.0f, 100.0f, 100.0f);
	light_spot.Direction = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	light_spot.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	light_spot.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light_spot.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	light_spot.Attenuation0 = 1.0f;
	light_spot.Attenuation1 = 0.0f;
	light_spot.Attenuation2 = 0.0f;
	light_spot.Range = 300.0f;
	light_spot.Falloff = 0.1f;
	light_spot.Phi = D3DX_PI / 3.0f;				//内核的角度180/3=60度
	light_spot.Theta = D3DX_PI / 6.0f;				//内核的角度180/6=30度
	*/
	//g_pd3dDevice->SetLight(0, &light_direction);
	//g_pd3dDevice->LightEnable(0, true);
	Light_Set(g_pd3dDevice, 1);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}
void Light_Set(LPDIRECT3DDEVICE9 pd3dDevice, UINT nType){
	static D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	switch (nType){
	case 1:
		light.Type = D3DLIGHT_POINT;
		light.Ambient = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Position = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;
		light.Range = 300.0f;
		break;
	case 2:
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Direction = D3DXVECTOR3(0.0f, 1.0f, 0.0f);//D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		break;
	case 3:
		light.Type = D3DLIGHT_SPOT;
		light.Position = D3DXVECTOR3(100.0f, 100.0f, 100.0f);
		light.Direction = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		light.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;
		light.Range = 300.0f;
		light.Falloff = 0.1f;
		light.Phi = D3DX_PI / 3.0f;
		light.Theta = D3DX_PI / 6.0f;
		break;
	}
	pd3dDevice->SetLight(0, &light);
	pd3dDevice->LightEnable(0, true);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(36, 36, 36));

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