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
void Conversion_Init();										//对转换有关的变量进行初始化
double GetFPS();
void Matrix_Set();

LPDIRECT3D9 pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
ID3DXFont *g_pFont = NULL;
//顶点显示变量
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;				//顶点缓存对象
LPDIRECT3DINDEXBUFFER9 g_pIndexBuf = NULL;					//索引缓存对象
wchar_t string_fps[40];
//与变换有关的变量
D3DXMATRIX pMatrix_TransRight;
D3DXMATRIX pMatrix_TransLeft;
D3DXMATRIX pMatrix_RotateRight;
D3DXMATRIX pMatrix_RotateLeft;
D3DXMATRIX pMatrix_Scale;
D3DXMATRIX matWorld;
//摄像机
D3DXMATRIX matView;
D3DXVECTOR3 vEyePt(0.0f, 0.0f, -100.0f);			//摄像机的位置
D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);		//观察点的位置
D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);			//向上的向量
//投影
D3DXMATRIX matProj;
//视口
D3DVIEWPORT9 vp = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 1 };

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
	FWindow femy;
	MSG msg = { 0 };
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
	//Direct3D_Init(femy.hWnd);
	//Objects_Init(femy.hWnd);
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
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);		//关闭光照
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//开启背面消隐
	return S_OK;
}

HRESULT Objects_Init(HWND hwnd){
	srand(timeGetTime());
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, TEXT("微软雅黑"), &g_pFont)))
		return E_FAIL;
	//创建顶点缓存
	if (FAILED(g_pd3dDevice->CreateVertexBuffer((8) * sizeof(FVertex), 0,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL)))
	{
		return E_FAIL;
	}
	//创建索引缓存
	if (FAILED(g_pd3dDevice->CreateIndexBuffer(36 * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIndexBuf, NULL)))
	{
		return E_FAIL;
	}
	FVertex vertices[] = {
		{ -20.0f, 20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ -20.0f, 20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, 20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, 20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ -20.0f, -20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ -20.0f, -20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, -20.0f, 20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
		{ 20.0f, -20.0f, -20.0f, D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256) },
	};
	/*
	vertices[0].x =  - 20; vertices[0].y =  + 20; vertices[0].z = 0.0 - 20; vertices[0].color = D3DCOLOR_XRGB(255, 255, 0);
	vertices[1].x =  - 20; vertices[1].y =  + 20; vertices[1].z = 0.0 + 20; vertices[1].color = D3DCOLOR_XRGB(255, 0, 255);
	vertices[2].x =  + 20; vertices[2].y =  + 20; vertices[2].z = 0.0 + 20; vertices[2].color = D3DCOLOR_XRGB(125, 125, 125);
	vertices[3].x =  + 20; vertices[3].y =  - 20; vertices[3].z = 0.0 + 20; vertices[3].color = D3DCOLOR_XRGB(0, 255, 0);

	vertices[4].x =  - 20; vertices[4].y =  - 20; vertices[4].z = 0.0 - 20; vertices[4].color = D3DCOLOR_XRGB(255, 255, 0);
	vertices[5].x =  - 20; vertices[5].y =  - 20; vertices[5].z = 0.0 + 20; vertices[5].color = D3DCOLOR_XRGB(255, 0, 255);
	vertices[6].x =  + 20; vertices[6].y =  - 20; vertices[6].z = 0.0 + 20; vertices[6].color = D3DCOLOR_XRGB(125, 255, 125);
	vertices[7].x =  + 20; vertices[7].y =  - 20; vertices[7].z = 0.0 - 20; vertices[7].color = D3DCOLOR_XRGB(0, 255, 0);
	*/
	//vertices[0].rhw = vertices[1].rhw = vertices[2].rhw = vertices[3].rhw = vertices[4].rhw = vertices[5].rhw = vertices[6].rhw = vertices[7].rhw = 1.0f;


	void *pVertices;
	if (FAILED(g_pVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, vertices, sizeof(vertices));
	g_pVertexBuffer->Unlock();


	WORD Indices[] = { 0, 1, 2, 0, 2, 3, 0, 3, 7, 0, 7, 4,
		0, 4, 5, 0, 5, 1, 2, 6, 7, 2, 7, 3,
		2, 5, 6, 2, 1, 5, 4, 6, 5, 4, 7, 6 };
	WORD *pIndices = NULL;
	if (FAILED(g_pIndexBuf->Lock(0, 0, (void**)&pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, Indices, sizeof(Indices));
	g_pIndexBuf->Unlock();
	//关闭背面渲染
	//	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	//	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


	return S_OK;
}

void Direct3D_Render(HWND hwnd){
	RECT rect;
	rect.left = 0; rect.right = rect.left + WINDOW_WIDTH;
	rect.top = 0; rect.bottom = rect.top + WINDOW_HEIGHT;
	GetClientRect(hwnd, &rect);
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 214, 158), 1.0f, 0);

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

	//g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(FVertex));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	g_pd3dDevice->SetIndices(g_pIndexBuf);
	g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	g_pd3dDevice->SetTransform(D3DTS_WORLD, &pMatrix_RotateLeft);

	swprintf_s(string_fps, 14, TEXT("FPS:%.1f/s"), GetFPS());
	g_pFont->DrawText(0, string_fps, -1, &rect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 255, 255));
	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Direct3D_CleanUp(){
	SAFE_RELEASE(g_pIndexBuf);				//清理资源
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pd3dDevice);
}

void Conversion_Init(){
	D3DXMatrixTranslation(&pMatrix_TransRight, 10.0f, 0, 0);
	D3DXMatrixTranslation(&pMatrix_TransLeft, -10.0f, 0, 0);

	D3DXMatrixRotationY(&pMatrix_RotateLeft, 45 * (2.0f * D3DX_PI) / 360.0f);		//旋转90度
	D3DXMatrixRotationX(&pMatrix_RotateRight, -3.1415926 / 2);

	D3DXMatrixScaling(&pMatrix_Scale, 1.0f, 1.0f, 1.0f);

	D3DXMatrixIdentity(&matWorld);
	//建立摄像机的位置
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	//建立投影
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, (float)800 / 600, 1.0f, 1.0f); //45度角 4/3 
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	//建立视窗
	vp.X = 0; vp.Y = 0; vp.Width = WINDOW_WIDTH; vp.Height = WINDOW_HEIGHT; vp.MinZ = 0; vp.MaxZ = 1;
	g_pd3dDevice->SetViewport(&vp);
	//D3DXMatrixMultiply(&pMatrix, &pMatrix, &pMatrix_Right);		//将矩阵向右移动
}
void Matrix_Set(){
	static double angle = 0;
	//世界变换矩阵
	D3DXMATRIX matWorld, Rx, Ry, Rz;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixRotationX(&Rx, D3DX_PI * angle);
	D3DXMatrixRotationY(&Ry, D3DX_PI * angle);
	D3DXMatrixRotationZ(&Rz, D3DX_PI);
	angle += 0.0001;
	matWorld = Rx * Ry * Rz * matWorld;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	//取景变换矩阵的设置（摄像机）
	D3DXMATRIX matView;
	D3DXVECTOR3 vEye(0.0f, 0.0f, -200.0f);		//摄像机的位置
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);			//观察点的位置
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);			//向上的向量
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	//投影矩阵变换
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f);		//D3DX_PI/4.0f 45度
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	//视口变换的设置
	D3DVIEWPORT9 vp;
	vp.X = 0; vp.Y = 0; vp.Width = WINDOW_WIDTH; vp.Height = WINDOW_HEIGHT;
	vp.MinZ = 0.0f; vp.MaxZ = 1.0f;
	g_pd3dDevice->SetViewport(&vp);
	

	/*
	//--------------------------------------------------------------------------------------
	//【四大变换之一】：世界变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matWorld, Rx, Ry, Rz;
	D3DXMatrixIdentity(&matWorld);                  // 单位化世界矩阵
	D3DXMatrixRotationX(&Rx, D3DX_PI *(::timeGetTime() / 1000.0f));    // 绕X轴旋转
	D3DXMatrixRotationY(&Ry, D3DX_PI *(::timeGetTime() / 1000.0f / 2));    // 绕Y轴旋转
	D3DXMatrixRotationZ(&Rz, D3DX_PI *(::timeGetTime() / 1000.0f / 3));   // 绕Z轴旋转
	matWorld = Rx * Ry * Rz * matWorld;             // 得到最终的组合矩阵
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);  //设置世界变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之二】：取景变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matView; //定义一个矩阵
	D3DXVECTOR3 vEye(0.0f, 0.0f, -200.0f);  //摄像机的位置
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
	*/
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


