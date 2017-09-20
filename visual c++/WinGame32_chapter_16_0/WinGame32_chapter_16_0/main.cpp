#include <DInput.h>
#include "fgame.h"
#include "femy.h"
#include "fclass.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

HINSTANCE g_hinstance;
LPDIRECT3D9 pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
ID3DXFont *g_pFont = NULL;
DInputClass *g_pDInput = NULL;

//顶点显示变量
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
LPDIRECT3DINDEXBUFFER9 g_pIndexBuffer = NULL;
//纹理对象
LPDIRECT3DTEXTURE9 g_pTexture = NULL;
HRESULT Direct3D_Init(HWND hwnd);
HRESULT Objects_Init(HWND hwnd);
void Direct3D_Render(HWND hwnd);
void Direct_CleanUp(HWND hwnd);
void Direct3D_Update(HWND hwnd);

void Matrix_Set();

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
	FWindow femy;
	MSG msg = { 0 };
	femy.FInitWindowClass();
	femy.FSetWndProc(WndProc);
	femy.FSetHInstance(hInstance);
	femy.FSetIcon(NULL);
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
		return 0;
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
			Direct3D_Update(femy.hWnd);
			Direct3D_Render(femy.hWnd);
		}
	}


	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){
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
	//pD3D->GetAdapterIdentifier(0, 0, &Adapter);
	//int len = MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, NULL, 0);
	//MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, display_info, len);

	SAFE_RELEASE(pD3D);

	//初始化输入类
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
	CUSTOMVERTEX *pVertices;
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, TEXT("微软雅黑"), &g_pFont)))
	{
		return E_FAIL;
	}
	//创建顶点数据
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(8 * 3 * sizeof(CUSTOMVERTEX), 0,
		D3DFVF_CUSTOMVERTEX_TEX, D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL)))
	{
		return E_FAIL;
	}
	if (FAILED(g_pVertexBuffer->Lock(0, 24 * sizeof(CUSTOMVERTEX), (void**)&pVertices, 0)))
	{
		return E_FAIL;
	}
	//正面顶点数据
	pVertices[0] = CUSTOMVERTEX(-10.0f, 10.0f, -10.0f, 0.0f, 0.0f);
	pVertices[1] = CUSTOMVERTEX(10.0f, 10.0f, -10.0f, 1.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(10.0f, -10.0f, -10.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(-10.0f, -10.0f, -10.0f, 0.0f, 1.0f);

	// 背面顶点数据
	pVertices[4] = CUSTOMVERTEX(10.0f, 10.0f, 10.0f, 0.0f, 0.0f);
	pVertices[5] = CUSTOMVERTEX(-10.0f, 10.0f, 10.0f, 1.0f, 0.0f);
	pVertices[6] = CUSTOMVERTEX(-10.0f, -10.0f, 10.0f, 1.0f, 1.0f);
	pVertices[7] = CUSTOMVERTEX(10.0f, -10.0f, 10.0f, 0.0f, 1.0f);

	// 顶面顶点数据
	pVertices[8] = CUSTOMVERTEX(-10.0f, 10.0f, 10.0f, 0.0f, 0.0f);
	pVertices[9] = CUSTOMVERTEX(10.0f, 10.0f, 10.0f, 1.0f, 0.0f);
	pVertices[10] = CUSTOMVERTEX(10.0f, 10.0f, -10.0f, 1.0f, 1.0f);
	pVertices[11] = CUSTOMVERTEX(-10.0f, 10.0f, -10.0f, 0.0f, 1.0f);

	// 底面顶点数据
	pVertices[12] = CUSTOMVERTEX(-10.0f, -10.0f, -10.0f, 0.0f, 0.0f);
	pVertices[13] = CUSTOMVERTEX(10.0f, -10.0f, -10.0f, 1.0f, 0.0f);
	pVertices[14] = CUSTOMVERTEX(10.0f, -10.0f, 10.0f, 1.0f, 1.0f);
	pVertices[15] = CUSTOMVERTEX(-10.0f, -10.0f, 10.0f, 0.0f, 1.0f);

	// 左侧面顶点数据
	pVertices[16] = CUSTOMVERTEX(-10.0f, 10.0f, 10.0f, 0.0f, 0.0f);
	pVertices[17] = CUSTOMVERTEX(-10.0f, 10.0f, -10.0f, 1.0f, 0.0f);
	pVertices[18] = CUSTOMVERTEX(-10.0f, -10.0f, -10.0f, 1.0f, 1.0f);
	pVertices[19] = CUSTOMVERTEX(-10.0f, -10.0f, 10.0f, 0.0f, 1.0f);

	// 右侧面顶点数据
	pVertices[20] = CUSTOMVERTEX(10.0f, 10.0f, -10.0f, 0.0f, 0.0f);
	pVertices[21] = CUSTOMVERTEX(10.0f, 10.0f, 10.0f, 1.0f, 0.0f);
	pVertices[22] = CUSTOMVERTEX(10.0f, -10.0f, 10.0f, 1.0f, 1.0f);
	pVertices[23] = CUSTOMVERTEX(10.0f, -10.0f, -10.0f, 0.0f, 1.0f);

	
	g_pVertexBuffer->Unlock();
	//创建索引缓存数据
	if (FAILED(g_pd3dDevice->CreateIndexBuffer(12 * 3 * sizeof(WORD), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIndexBuffer, NULL)))
	{
		return E_FAIL;
	}
	WORD *pIndices;
	g_pIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);
	// 正面索引数据
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
	pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;

	// 背面索引数据
	pIndices[6] = 4; pIndices[7] = 5; pIndices[8] = 6;
	pIndices[9] = 4; pIndices[10] = 6; pIndices[11] = 7;

	// 顶面索引数据
	pIndices[12] = 8; pIndices[13] = 9; pIndices[14] = 10;
	pIndices[15] = 8; pIndices[16] = 10; pIndices[17] = 11;

	// 底面索引数据
	pIndices[18] = 12; pIndices[19] = 13; pIndices[20] = 14;
	pIndices[21] = 12; pIndices[22] = 14; pIndices[23] = 15;

	// 左侧面索引数据
	pIndices[24] = 16; pIndices[25] = 17; pIndices[26] = 18;
	pIndices[27] = 16; pIndices[28] = 18; pIndices[29] = 19;

	// 右侧面索引数据
	pIndices[30] = 20; pIndices[31] = 21; pIndices[32] = 22;
	pIndices[33] = 20; pIndices[34] = 22; pIndices[35] = 23;
	g_pIndexBuffer->Unlock();


	//初始化纹理
	D3DXCreateTextureFromFile(g_pd3dDevice, TEXT("test.jpg"), &g_pTexture);
	
	D3DMATERIAL9 mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_pd3dDevice->SetMaterial(&mtrl);

	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	light.Direction = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	g_pd3dDevice->SetLight(0, &light); //设置光源  
	g_pd3dDevice->LightEnable(0, true);//启用光照  

	// 设置渲染状态
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);				  //初始化顶点法线
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				  //开启背面消隐
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(36, 36, 36));   //设置一下环境光 

	//g_pd3dDevice->SetTexture(D3DRS_LIGHTING, false);						  //关闭光照
	return S_OK;
}

void Direct3D_Render(HWND hwnd){
	RECT rect;
	GetClientRect(hwnd, &rect);
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(80, 125, 125), 1.0f, 0);
	g_pd3dDevice->BeginScene();


	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX_TEX);
	g_pd3dDevice->SetIndices(g_pIndexBuffer);

	//设置最近点采样过滤
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	//设置线性纹理过滤
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//设置各向异性过滤
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, 3);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	//多级渐进纹理过滤
	//g_pd3dDevice->SetSamplerState(1)
	
	

	g_pd3dDevice->SetTexture(0, g_pTexture);
	g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

	g_pFont->DrawText(0, TEXT("fadafa"), -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 0, 0));


	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

}
void Direct_CleanUp(HWND hwnd){
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pd3dDevice);
}

void Matrix_Set()
{
	//--------------------------------------------------------------------------------------
	//【四大变换之一】：世界变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matWorld;
	D3DXMatrixTranslation(&matWorld, 0, 0, 0);
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	//--------------------------------------------------------------------------------------
	//【四大变换之二】：取景变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX Rx, Ry, g_matWorld;
	D3DXMATRIX matView; //定义一个矩阵
	D3DXVECTOR3 vEye(0.0f, 0.0f, -100.0f);  //摄像机的位置
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //观察点的位置
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//向上的向量
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //计算出取景变换矩阵
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView); //应用取景变换矩阵


	//--------------------------------------------------------------------------------------
	//【四大变换之三】：投影变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matProj; //定义一个矩阵
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, (float)((double)WINDOW_WIDTH / WINDOW_HEIGHT), 1.0f, 1000.0f); //计算投影变换矩阵
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
D3DXMATRIX g_matWorld;
void Direct3D_Update(HWND hwnd)
{
	//使用DirectInput类读取数据
	g_pDInput->GetInput();

	// 获取键盘消息并给予设置相应的填充模式
	if (g_pDInput->IsKeyDown(DIK_1))         // 若数字键1被按下，进行实体填充
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if (g_pDInput->IsKeyDown(DIK_2))         // 若数字键2被按下，进行线框填充
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// 按住鼠标左键并拖动，为平移操作
	static FLOAT fPosX = 0.0f, fPosY = 0.0f, fPosZ = 0.0f;

	if (g_pDInput->IsMouseButtonDown(0))
	{
		fPosX += (g_pDInput->MouseDX())*  0.08f;
		fPosY += (g_pDInput->MouseDY()) * -0.08f;
	}

	//鼠标滚轮，为观察点收缩操作
	fPosZ += (g_pDInput->MouseDZ())* 0.02f;

	// 平移物体
	if (g_pDInput->IsKeyDown(DIK_A)) fPosX -= 0.005f;
	if (g_pDInput->IsKeyDown(DIK_D)) fPosX += 0.005f;
	if (g_pDInput->IsKeyDown(DIK_W)) fPosY += 0.005f;
	if (g_pDInput->IsKeyDown(DIK_S)) fPosY -= 0.005f;


	D3DXMatrixTranslation(&g_matWorld, fPosX, fPosY, fPosZ);


	// 按住鼠标右键并拖动，为旋转操作
	static float fAngleX = D3DX_PI / 6, fAngleY = D3DX_PI / 6;

	if (g_pDInput->IsMouseButtonDown(1))
	{
		fAngleX += (g_pDInput->MouseDY())* -0.01f;
		fAngleY += (g_pDInput->MouseDX()) * -0.01f;
	}
	// 旋转物体
	if (g_pDInput->IsKeyDown(DIK_UP)) fAngleX += 0.005f;
	if (g_pDInput->IsKeyDown(DIK_DOWN)) fAngleX -= 0.005f;
	if (g_pDInput->IsKeyDown(DIK_LEFT)) fAngleY -= 0.005f;
	if (g_pDInput->IsKeyDown(DIK_RIGHT)) fAngleY += 0.005f;


	D3DXMATRIX Rx, Ry;
	D3DXMatrixRotationX(&Rx, fAngleX);
	D3DXMatrixRotationY(&Ry, fAngleY);

	g_matWorld = Rx * Ry * g_matWorld; //算出最终的世界矩阵
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_matWorld);//设置世界矩阵

	Matrix_Set();
}
