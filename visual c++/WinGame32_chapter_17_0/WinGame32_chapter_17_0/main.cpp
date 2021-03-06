#include <DInput.h>
#include <stdio.h>
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
D3DXMATRIX g_matWorld;

HRESULT Direct3D_Init(HWND hwnd);
HRESULT Objects_Init(HWND hwnd);
void Direct3D_Render(HWND hwnd);
void Direct_CleanUp(HWND hwnd);
void Direct3D_Update(HWND hwnd);
void Direct3D_Render(HWND hwnd);
void Matrix_Set();

//网格对象接口变量
LPD3DXMESH g_pMesh = NULL;				//网格对象
D3DMATERIAL9 * g_pMaterials = NULL;		//材质对象
LPDIRECT3DTEXTURE9* g_pTextures = NULL;
DWORD g_dwNumMtrls;

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
	wchar_t buffer[100];
	if (FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, TEXT("微软雅黑"), &g_pFont)))
	{
		return E_FAIL;
	}

	// 从X文件中加载网格数据
	LPD3DXBUFFER pAdjBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;

	//D3DXLoadMeshFromX(L"miki.X", D3DXMESH_MANAGED, g_pd3dDevice,
	//	&pAdjBuffer, &pMtrlBuffer, NULL, &g_dwNumMtrls, &g_pMesh);
	D3DXLoadMeshFromX(L"test.X", D3DXMESH_MANAGED, g_pd3dDevice,
		&pAdjBuffer, &pMtrlBuffer, NULL, &g_dwNumMtrls, &g_pMesh);
	// 读取材质和纹理数据
	D3DXMATERIAL *pMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer(); //创建一个D3DXMATERIAL结构体用于读取材质和纹理信息
	g_pMaterials = new D3DMATERIAL9[g_dwNumMtrls];
	g_pTextures = new LPDIRECT3DTEXTURE9[g_dwNumMtrls];
	//wsprintf(buffer, TEXT("materila:%d\0"), g_dwNumMtrls);
	//MessageBox(NULL, buffer, TEXT("error"), MB_OK);
	for (DWORD i = 0; i < g_dwNumMtrls; i++)
	{
		//获取材质，并设置一下环境光的颜色值
		g_pMaterials[i] = pMtrls[i].MatD3D;
		g_pMaterials[i].Ambient = g_pMaterials[i].Diffuse;
		g_pMaterials[i].Diffuse.a = 0.8f;
		//创建一下纹理对象
		g_pTextures[i] = NULL;
	//	wsprintf(buffer, TEXT("%s\0"), pMtrls[i].pTextureFilename);
	//	MessageBox(NULL, buffer, TEXT("error"), MB_OK);
	//	D3DXCreateTextureFromFileA(g_pd3dDevice, pMtrls[i].pTextureFilename, &g_pTextures[i]);
	}

	SAFE_RELEASE(pAdjBuffer)
	SAFE_RELEASE(pMtrlBuffer)
	
	// 设置光照
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	//light.Ambient = D3DXCOLOR(1.0f, 1.0f, 0.5f, 1.0f);
	light.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	light.Direction = D3DXVECTOR3(1.0f, 0.0f, 1.0f);
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, true);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	// 设置渲染状态
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		//开启背面消隐
	g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	// 三步曲之一，开启Alpha融合
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	//三步曲之二，设置融合因子
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//三步曲之三，设置融合运算方式
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);  //这句设置运算方式为D3DBLENDOP_ADD的代码Direct3D默认为我们写了，所以注释掉这句也没大碍

	//g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);		//开启Alpha混合



	/*
	LPD3DXBUFFER pAdjBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	//加载模型
	
	D3DXLoadMeshFromX(TEXT("miki.X"), D3DXMESH_MANAGED, g_pd3dDevice,
		&pAdjBuffer, &pMtrlBuffer, NULL, &g_dwNumMtrls, &g_pMesh);

	
	D3DXMATERIAL *pMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();			//创建一个D3DXMATERIAL结构体
	
	g_pMaterials = new D3DMATERIAL9[g_dwNumMtrls];
	g_pTextures = new LPDIRECT3DTEXTURE9[g_dwNumMtrls];
	
	
	for (DWORD i = 0; i < g_dwNumMtrls; i++)
	{
		//获取材质
		g_pMaterials[i] = pMtrls[i].MatD3D;
		g_pMaterials[i].Ambient = g_pMaterials[i].Diffuse;
		//创建纹理 
		g_pTextures[i] = NULL;
		D3DXCreateTextureFromFileA(g_pd3dDevice, pMtrls[i].pTextureFilename, &g_pTextures[i]);
	}
	//释放资源
	SAFE_RELEASE(pAdjBuffer);
	SAFE_RELEASE(pMtrlBuffer);
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	light.Direction = D3DXVECTOR3(1.0f, 0.0f, 1.0f);
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, true);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
	// 设置渲染状态
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //开启背面消隐
	*/

	return S_OK;
}

/*void Direct3D_Render(HWND hwnd){
	RECT rect;
	GetClientRect(hwnd, &rect);
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(80, 125, 125), 1.0f, 0);
	g_pd3dDevice->BeginScene();

	g_pFont->DrawText(0, TEXT("fadfafa"), -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 0, 0));

	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

}
*/
void Direct_CleanUp(HWND hwnd){
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pd3dDevice);

}
void Direct3D_Update(HWND hwnd)
{
	//使用DirectInput类读取数据
	g_pDInput->GetInput();
	//通过按键的按下来控制漫反射Alpha分量值的变化
	if (g_pDInput->IsKeyDown(DIK_1))  //按下1键
	{
		for (DWORD i = 0; i < g_dwNumMtrls; i++)
		{
			g_pMaterials[i].Diffuse.a += 0.001f;
		}
	}

	if (g_pDInput->IsKeyDown(DIK_2)) //按下1键
	{
		for (DWORD i = 0; i < g_dwNumMtrls; i++)
		{
			g_pMaterials[i].Diffuse.a -= 0.001f;
		}
	}

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
	static float fAngleX = 0, fAngleY = 0;

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

	g_matWorld = Rx * Ry * g_matWorld;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_matWorld);
	Matrix_Set();
}
void Direct3D_Render(HWND hwnd)
{

	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之一】：清屏操作
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(150, 150, 100), 1.0f, 0);

	//定义一个矩形，用于获取主窗口矩形
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之二】：开始绘制
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->BeginScene();                     // 开始绘制

	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之三】：正式绘制
	//--------------------------------------------------------------------------------------

	// 用一个for循环，进行网格各个部分的绘制
	for (DWORD i = 0; i < g_dwNumMtrls; i++)
	{
		g_pd3dDevice->SetMaterial(&g_pMaterials[i]);
		g_pd3dDevice->SetTexture(0, g_pTextures[i]);
		g_pMesh->DrawSubset(i);
	}



	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之四】：结束绘制
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->EndScene();                       // 结束绘制
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之五】：显示翻转
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);  // 翻转与显示

}
void Matrix_Set()
{
	//--------------------------------------------------------------------------------------
	//【四大变换之一】：世界变换矩阵的设置
	//--------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------
	//【四大变换之二】：取景变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matView; //定义一个矩阵
	D3DXVECTOR3 vEye(0.0f, 0.0f, -250.0f);  //摄像机的位置
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