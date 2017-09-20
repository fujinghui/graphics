/*
	2016.8.26 开始实现引擎的光源功能

*/
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <ddraw.h>
#include "graph_engine.h"
#include "model.h"
#include "draw.h"
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dsound.lib")
//#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "winmm.lib")

//宏常量定义
#define _RGB16BIT555(r, g, b) (((r)&0x31<<10) | ((g)&0x31<<5) | ((b)&0x31))
#define _RGB16BIT565(r, g, b) (((r)&0x31<<11) | ((g)&0x63<<5) | ((b)&0x31))
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

//变量定义

UCHAR *primary_buffer;		//主缓存
int primary_pitch;			//内存跨距 单位为字节
UCHAR *secondary_buffer;	//辅助缓存
int secondary_pitch;		//内存跨距 单位为字节

UCHAR pixel;				//像素值
int x, y;					//像素坐标

HDC hdc_buffer = NULL;
HDC hdc_main;
HBITMAP hbitmap_buffer;
//int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 640;
LPDIRECTDRAW lpdd = NULL;
//函数定义
void DirectInit();
void Game_Init(HWND hwnd);
void Game_Main(HDC hdc);
int GetFPS();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
	WNDCLASS wnex;
	HWND hwnd;
	char scClassName[] = "femy";
	char scTitle[] = "图形引擎";
	MSG msg;
	wnex.style = CS_HREDRAW | CS_VREDRAW;
	wnex.hInstance = hInstance;
	wnex.cbClsExtra = 0;
	wnex.cbWndExtra = 0;
	wnex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnex.hIcon = NULL;
	wnex.lpfnWndProc = WndProc;
	wnex.lpszClassName = scClassName;
	wnex.lpszMenuName = NULL;
	if (!RegisterClass(&wnex))
		return 0;
	hwnd = CreateWindow(scClassName, scTitle, WS_OVERLAPPEDWINDOW, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);
	if (hwnd == NULL)
		return 0;
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);
	DirectInit();
	Game_Init(hwnd);
	long time = 0;
	while (1)// GetMessage(&msg, NULL, 0, 0))
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (GetCurrentTime() - time > 10)
			{
				Game_Main(hdc_buffer);
				time = GetCurrentTime();
			}
		}
	}
	ReleaseDC(hwnd, hdc_main);
	UnregisterClass(scClassName, hInstance);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){

	switch (message){
	case WM_CREATE:

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

void DirectInit(){
	DirectDrawCreate(NULL, &lpdd, NULL);
	if (lpdd == NULL)
	{
		return;
	}
	//lpdd->SetVideoMode(640, 480, 8);
}
POINT4D cam_pos = { 0, 0, -400, 1 };
VECTOR4D cam_dir = { 0, 0, 0, 1 };
VECTOR4D vscale = { 0.5, 0.5, 0.5 }, vpos = { 0, 0, 0, 1 }, vrot = { 0, 0, 0, 1 };
CAM4DV1 cam;					//摄像机
RENDERLIST4DV1 rend_list;		//渲染列表
POLYF4DV1 poly1, poly2;					//多边形
POINT4D poly1_pos = { 0, 0, 100, 1 };
OBJECT4DV1 object[10];
OBJECT4DV1 object_terri;

RGBAV1 white, gray, black, red, green, blue;
VECTOR4D point_light_pos = { 0, 200, 0, 0 };
CPARSERV1 model_loader;
void Game_Init(HWND hwnd){
	//GDI初始化
	hdc_main = GetDC(hwnd);
	hdc_buffer = CreateCompatibleDC(hdc_main);
	hbitmap_buffer = CreateCompatibleBitmap(hdc_main, SCREEN_WIDTH, SCREEN_HEIGHT);
	SelectObject(hdc_buffer, hbitmap_buffer);
	SelectObject(hdc_buffer, GetStockObject(BLACK_BRUSH));
	DeleteObject(hbitmap_buffer);
	
	SetBkMode(hdc_buffer, TRANSPARENT);
	SetTextColor(hdc_buffer, RGB(0, 255, 0));
	//3D引擎初始化
	Build_Sin_Cos_Tables();
	poly1.state = POLY4DV1_STATE_ACTIVE;
	poly1.attr = 0;
	poly1.color = RGB(125, 125, 125);
	poly1.color_light_trans = poly1.color;
	//poly1.vlist[0].x = 0; poly1.vlist[0].y = 50; poly1.vlist[0].z = 0; poly1.vlist[0].w = 1;
	//poly1.vlist[2].x = 50; poly1.vlist[1].y = -50; poly1.vlist[1].z = 0; poly1.vlist[1].w = 1;
	//poly1.vlist[2].x = -50; poly1.vlist[2].y = -50; poly1.vlist[2].z = 0; poly1.vlist[2].w = 1;
	poly1.vlist[0].x = 100; poly1.vlist[0].y = -100; poly1.vlist[0].z = 100; poly1.vlist[0].w = 1;
	poly1.vlist[1].x = 100; poly1.vlist[1].y = -100; poly1.vlist[1].z = -100; poly1.vlist[1].w = 1;
	poly1.vlist[2].x = -100; poly1.vlist[2].y = -100; poly1.vlist[2].z = -100; poly1.vlist[2].w = 1;

	poly2.state = POLY4DV1_STATE_ACTIVE;
	poly2.attr = 0;
	poly2.color = RGB(125, 125, 125);

	poly2.color_light_trans = poly2.color;
	poly2.vlist[0].x = -100; poly2.vlist[0].y = -100; poly2.vlist[0].z = -100;
	poly2.vlist[1].x = -100; poly2.vlist[1].y = -100; poly2.vlist[1].z = 100;
	poly2.vlist[2].x = 100; poly2.vlist[2].y = -100; poly2.vlist[2].z = 100;

	//poly2.vlist[0].x = 50; poly2.vlist[0].y = -50; poly2.vlist[0].z = 50;
	//poly2.vlist[1].x = 0; poly2.vlist[1].y = 50; poly2.vlist[1].z = 0;
	//poly2.vlist[2].x = -50; poly2.vlist[2].y = -50; poly2.vlist[2].z = 50;


	poly1.next = poly1.prev = NULL;
	poly2.next = poly2.prev = NULL;

	Load_OBJECT4DV1_PLG(&object[0], "tank2_8b.plg", &vscale, &vpos, &vrot);
	vpos.x = 0; vpos.z = -50; vpos.y = 0;
	vscale.x = 10; vscale.y = 10; vscale.z = 10;
	Load_OBJECT4DV1_PLG(&object_terri, "markerg1_8b.plg", &vscale, &vpos, &vrot);
	for (int i = 0; i < object_terri.num_polys; i++)
	{
		object_terri.plist[i].color_light_trans = object_terri.plist[i].color;
	}
	for (int i = 1; i < 9; i++)
	{
		memcpy(&object[i], &object[0], sizeof(OBJECT4DV1));
	}

	if (Load_OBJECT4DV1_3DSASC(&object[9], "cube01.asc", &vscale, &vpos, &vrot, 0))
	{
		//MessageBox(NULL, "text", "title", MB_OK);
	}

	for (int i = 0; i < 10; i++)
	{
		object[i].world_pos.x = -400 + i * 100;// rand() % 400 + 200;
		object[i].world_pos.z = 100;
		object[i].world_pos.y = 0;

		object[i].plist[0].color = RGB(rand() % 155, rand() % 155, rand() % 125);
		//if (i % 3 == 0)
		for (int j = 1; j < object[i].num_polys; j++)
		{
			object[i].plist[j].color = object[i].plist[0].color;// RGB(rand() % 255, rand() % 255, rand() % 225);
		}
	}

	Init_CAM4DV1(&cam, CAM_MODE_ELUER, &cam_pos, &cam_dir,
		NULL,
		1.0f, 40000.0f,		//近远裁剪面
		90.0f,
		SCREEN_WIDTH,
		SCREEN_HEIGHT
		);
	//建立欧拉相机模型
	Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);
	white.rgba = _RGBA32BIT(255, 255, 255, 0);
	gray.rgba = _RGBA32BIT(100, 100, 100, 0);
	black.rgba = _RGBA32BIT(0, 0, 0, 0);
	red.rgba = _RGBA32BIT(255, 0, 0, 0);
	green.rgba = _RGBA32BIT(0, 255, 0, 0);
	blue.rgba = _RGBA32BIT(0, 0, 255, 0);
	//初始化灯源
	//初始化环境光（ambient light)
	Init_Light_LIGHTV1(0, 
		LIGHTV1_STATE_OFF,
		LIGHTV1_ATTR_AMBIENT,
		white, black, black,
		NULL, NULL,
		0, 0, 0,
		0, 0, 0
		);
	VECTOR4D dlight_dir = { 0, -1, 0, 0 };
	Init_Light_LIGHTV1(1,
		LIGHTV1_STATE_OFF,
		LIGHTV1_ATTR_INFINITE,
		black, white, black,
		NULL, &dlight_dir,
		0, 0, 0,
		0, 0, 0
		);
	VECTOR4D plight_pos = { 0, 200, 0, 0 };
	Init_Light_LIGHTV1(2,
		LIGHTV1_STATE_OFF,
		LIGHTV1_ATTR_POINT,
		black, white, black,
		&plight_pos, NULL,
		0, 0.001, 0,
		0, 0, 1
		);
	VECTOR4D slight_pos = { 0, 200, -100, 0 };
	VECTOR4D slight_dir = { 0, -1, 1, 0 };
	Init_Light_LIGHTV1(3,
		LIGHTV1_STATE_ON,
		LIGHTV1_ATTR_SPOTLIGHT2,
		black, white, black,
		&slight_pos, &slight_dir,
		0, 0.001, 0,
		0, 0, 1
		);
}
int light_on = 1;
int y_move = 2;
void Game_Main(HDC hdc){
	if (hdc == NULL)
		return;
	Rectangle(hdc, 0, 0, SCREEN_WIDTH + 1, SCREEN_HEIGHT + 1);
	if (KEY_DOWN(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}
	if (KEY_DOWN(VK_SPACE))
	{
		light_on = (~light_on) & 1;
	}
	if (KEY_DOWN(VK_UP))
	{
		cam.pos.x += 5 * Fast_Sin(cam.dir.y);
		cam.pos.z += 5 * Fast_Cos(cam.dir.y);
		Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);
	}
	if (KEY_DOWN(VK_DOWN))
	{
		cam.pos.x -= 5 * Fast_Sin(cam.dir.y);
		cam.pos.z -= 5 * Fast_Cos(cam.dir.y);
		Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);
	}
	if (KEY_DOWN(VK_LEFT))
	{
		cam.dir.y -= 1;
		//cam.pos.x -= 1;
		Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);
	}
	if (KEY_DOWN(VK_RIGHT))
	{
		cam.dir.y += 1;
		//cam.pos.x += 1;
		Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);
	}

	char text[128];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			sprintf_s(text, "%.2f\0", cam.mcam.M[i][j]);
			TextOut(hdc, 20 + j * 40, 40 + i * 20, text, strlen(text));
		}
	}
	char  input[128] = "habhabhabhhadfadfasdkjfalksdjfl";
	char s[128];
	ReplaceChars(input, s, "ha", ' ');
	TextOut(hdc, 0, 220, s, strlen(s));

//	model_loader.Open("main.cpp");
//	model_loader.Close();


	//Draw_Triangle_2D(50, 100, 150, 100, 400, 200, RGB(0, 255, 0), hdc, 1);
	//Draw_Triangle_2D(200, 100, 50, 200, 150, 200, RGB(0, 0, 255), hdc, 1);
	//Draw_Triangle_2D(10, 20, 400, 300, 150, 200, RGB(125, 125, 0), hdc, 1);
	TCHAR fps_buffer[1024];
	int fps = GetFPS();
	sprintf_s(fps_buffer, "fps:%d light:%d\0", fps, light_on);
	TextOut(hdc, 10, 10, fps_buffer, strlen(fps_buffer));


	static MATRIX4X4 mrot;
	static float ang_y = 0.0f;
	int index;	//创建绕y轴旋转的矩阵
	Build_XYZ_Rotation_MATRIX4X4(0, ang_y, 0, &mrot);
	//缓慢地旋转y轴
	if (ang_y >= 360.0)
	{
		ang_y = 0.0f;
	}
	else
	{
		ang_y += 1;
	}

	for (index = 0; index < 10; index++)
	{
		Reset_OBJECT4DV1(&object[index]);
		if (!(Cull_OBJECT4DV1(&object[index], &cam, CULL_OBJECT_XYZ_PLANES)))
		{

		}
		Transform_OBJECT4DV1(&object[index], &mrot, TRANSFORM_LOCAL_TO_TRANS, 1);
		Model_To_World_OBJECT4DV1(&object[index], TRANSFORM_TRANS_ONLY);
		Remove_Backfaces_OBJECT4DV1(&object[index], &cam);
		if (light_on == 1)
		{
			Light_OBJECT4DV1_World16(&object[index], &cam, 4);
			TextOut(hdc, 10, 180, "active light", strlen("active light"));
		}
		World_To_Camera_OBJECT4DV1(&object[index], &cam);
		Camera_To_Perspective_OBJECT4DV1(&object[index], &cam);
		Perspective_To_Screen_OBJECT4DV1(&object[index], &cam);
		if (light_on)
			Draw_OBJECT4DV1_Wire16(&object[index], hdc, 1);
		else
			Draw_OBJECT4DV1_Solid16(&object[index], hdc, 1);
		
	}

	//渲染列表
	Reset_RENDERLIST4DV1(&rend_list);
	Insert_POLYF4DV1_RENDERLIST4DV1(&rend_list, &poly1);
	Insert_POLYF4DV1_RENDERLIST4DV1(&rend_list, &poly2);
	//Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &object[1], 1);
	Transform_RENDERLIST4DV1(&rend_list, &mrot, TRANSFORM_LOCAL_TO_TRANS);
	Model_To_World_RENDERLIST4DV1(&rend_list, &poly1_pos, TRANSFORM_TRANS_ONLY);
	Sort_RENDERLIST4DV1(&rend_list, SORT_POLYLIST_AVGZ);
	//Remove_Backfaces_RENDERLIST4DV1(&rend_list, &cam);
	Light_RENDERLIST4DV1_World(&rend_list, &cam, 4);
	//此处应该执行光照计算
	World_To_Camera_RENDERLIST4DV1(&rend_list, &cam);
	Camera_To_Perspective_RENDERLIST4DV1(&rend_list, &cam);
	Perspective_To_Screen_RENDERLIST4DV1(&rend_list, &cam);
	//Draw_RENDERLIST4DV1_Wrie16(&rend_list, hdc, 1);
	//Draw_RENDERLIST4DV1_Solid16(&rend_list, hdc, 1);

	text[0] = ' '; text[1] = ' '; text[2] = 'b'; text[3] = 'a'; text[4] = 'b'; text[5] = 'c'; text[6] = 'd', text[7] = '5', text[8] = ' ', text[9] = ' ', text[10] = ' ', text[11] = '\0';
	//ReplaceChars("f#fiandaofad:f#f", text, "f", 'a');
	StringLtrim(text);
	StringRtrim(text);
	TextOut(hdc, 10, 200, text, strlen(text));

	for (index = 0; index < 3; index++)
	{
		sprintf_s(text, "%.2f %.2f %.2f\0", rend_list.poly_data[0].tvlist[index].x, rend_list.poly_data[0].tvlist[index].y, rend_list.poly_data[0].tvlist[index].z);
		TextOut(hdc, 200, 40 + index * 20, text, strlen(text));
		sprintf_s(text, "%.2f %.2f %.2f\0", rend_list.poly_data[1].tvlist[index].x, rend_list.poly_data[1].tvlist[index].y, rend_list.poly_data[1].tvlist[index].z);
		TextOut(hdc, 400, 40 + index * 20, text, strlen(text));
	}

	
	BitBlt(hdc_main, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hdc, 0, 0, SRCCOPY);

}


int GetFPS(){
	static long old_time = 0;
	static int fps = 0;
	static int result_fps = 0;
	if (GetCurrentTime() - old_time >= 1000)
	{
		result_fps = fps;
		old_time = GetCurrentTime();
		fps = 0;
	}
	else
	{
		fps++;
	}
	return result_fps;
}