#include <windows.h>
#include <stdio.h>
#include <ddraw.h>
#include "graph_engine.h"
#include "model.h"
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
POINT4D cam_pos = { 0, 0, -100, 1 };
VECTOR4D cam_dir = { 0, 0, 0, 1 };
VECTOR4D vscale = { 0.5, 0.5, 0.5 }, vpos = { 0, 0, 0, 1 }, vrot = { 0, 0, 0, 1 };
CAM4DV1 cam;					//摄像机
RENDERLIST4DV1 rend_list;		//渲染列表
POLYF4DV1 poly1, poly2;					//多边形
POINT4D poly1_pos = { 0, 0, 100, 1 };
OBJECT4DV1 object[10];
void Game_Init(HWND hwnd){
	hdc_main = GetDC(hwnd);
	hdc_buffer = CreateCompatibleDC(hdc_main);
	hbitmap_buffer = CreateCompatibleBitmap(hdc_main, SCREEN_WIDTH, SCREEN_HEIGHT);
	SelectObject(hdc_buffer, hbitmap_buffer);
	SelectObject(hdc_buffer, GetStockObject(WHITE_BRUSH));
	DeleteObject(hbitmap_buffer);

	//3D引擎初始化
	Build_Sin_Cos_Tables();
	poly1.state = POLY4DV1_STATE_ACTIVE;
	poly1.attr = 0;
	poly1.color = RGB(255, 0, 0);
	
	poly1.vlist[0].x = 0; poly1.vlist[0].y = 50;poly1.vlist[0].z = 0;poly1.vlist[0].w = 1;
	poly1.vlist[1].x = 50;poly1.vlist[1].y = -50;poly1.vlist[1].z = 0;poly1.vlist[1].w = 1;
	poly1.vlist[2].x = -50;poly1.vlist[2].y = -50;poly1.vlist[2].z = 0;poly1.vlist[2].w = 1;

	poly2.state = POLY4DV1_STATE_ACTIVE;
	poly2.attr = 0;
	poly2.color = RGB(0, 255, 0);
	poly2.vlist[0].x = 50; poly2.vlist[0].y = -50; poly2.vlist[0].z = 50;
	poly2.vlist[1].x = 0; poly2.vlist[1].y = 50; poly2.vlist[1].z = 0;
	poly2.vlist[2].x = -50; poly2.vlist[2].y = -50; poly2.vlist[2].z = 50;

	poly1.next = poly1.prev = NULL;
	poly2.next = poly2.prev = NULL;

	Load_OBJECT4DV1_PLG(&object[0], "tank1.plg", &vscale, &vpos, &vrot);
	for (int i = 1; i < 10; i++)
	{
		memcpy(&object[i], &object[0], sizeof(OBJECT4DV1));
	}
	for (int i = 0; i < 10; i++)
	{
		object[i].world_pos.x = -400 + i * 100;// rand() % 400 + 200;
		object[i].world_pos.z = 100;
		object[i].world_pos.y = 0;
	}
	
	Init_CAM4DV1(&cam, CAM_MODE_ELUER, &cam_pos, &cam_dir,
		NULL,
		10.0f, 1000.0f,		//近远裁剪面
		90.0f,
		SCREEN_WIDTH,
		SCREEN_HEIGHT
		);
	//建立欧拉相机模型
	Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

}
POINT3D points[3] = { {0, 0.5, 0}, {0.5, 0, 0}, {-0.5, 0, 0} };
void Game_Main(HDC hdc){
	if (hdc == NULL)
		return;
	if (KEY_DOWN(VK_ESCAPE))
	{
		PostQuitMessage(0);
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
	Rectangle(hdc, 0, 0, SCREEN_WIDTH + 1, SCREEN_HEIGHT + 1);
	
	TCHAR fps_buffer[1024];                                                                                                                                                 
	int fps = GetFPS();
	sprintf_s(fps_buffer, "fps:%d\0", fps);
	TextOut(hdc, 10, 10, fps_buffer, strlen(fps_buffer));
	//DrawObject(hdc, points, 3);
	
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
	index = 0;
	for (index = 0; index < 10; index++)
	{
		Reset_OBJECT4DV1(&object[index]);
		if (!(Cull_OBJECT4DV1(&object[index], &cam, CULL_OBJECT_XYZ_PLANES)))
		{
			Transform_OBJECT4DV1(&object[index], &mrot, TRANSFORM_LOCAL_TO_TRANS, 1);
			Model_To_World_OBJECT4DV1(&object[index], TRANSFORM_TRANS_ONLY);
			Remove_Backfaces_OBJECT4DV1(&object[index], &cam);
			World_To_Camera_OBJECT4DV1(&object[index], &cam);
			Camera_To_Perspective_OBJECT4DV1(&object[index], &cam);
			Perspective_To_Screen_OBJECT4DV1(&object[index], &cam);
			Draw_OBJECT4DV1_Wire16(&object[index], hdc, 1);
		}
	}
	//Reset_RENDERLIST4DV1(&rend_list); 
	//Reset_OBJECT4DV1(&object);

	//Insert_POLYF4DV1_RENDERLIST4DV1(&rend_list, &poly1);
	//Insert_POLYF4DV1_RENDERLIST4DV1(&rend_list, &poly2);
	//for (int i = 0; i < 10; i ++)
	//	Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &object[i], 1);


	//对渲染列表中唯一一个多边形的局部坐标进行旋转变换
	//Transform_RENDERLIST4DV1(&rend_list, &mrot, TRANSFORM_LOCAL_ONLY);
	//Transform_OBJECT4DV1(&object, &mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

	//Model_To_World_RENDERLIST4DV1(&rend_list, &poly1_pos);

	//if (!Cull_OBJECT4DV1(&object, &cam, CULL_OBJECT_XYZ_PLANES))
	//{
	//	Model_To_World_OBJECT4DV1(&object, TRANSFORM_TRANS_ONLY);
	//}
	//else
	//	TextOut(hdc, 0, 220, "culled", 6);
	
	

	char text[128];
	sprintf_s(text, "%d \0", rend_list.num_polys);
	TextOut(hdc, 20, 200, text, strlen(text));

	//背面裁剪，必须在建立完世界坐标之后（紧挨着）
	//Remove_Backfaces_OBJECT4DV1(&object, &cam);
	//Remove_Backfaces_RENDERLIST4DV1(&rend_list, &cam);

	//创建相机变换矩阵
	//Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);
	//执行世界坐标到相机坐标变换
	//World_To_Camera_RENDERLIST4DV1(&rend_list, &cam);	
	//World_To_Camera_OBJECT4DV1(&object, &cam);



	//执行相机坐标到透视坐标变换
	//Camera_To_Perspective_RENDERLIST4DV1(&rend_list, &cam);
	//Camera_To_Perspective_OBJECT4DV1(&object, &cam);

	//sprintf_s(text, "%d %d \0", object.num_vertices, object.num_polys);
	//TextOut(hdc, 100, 100 + 80, text, strlen(text));
	//执行屏幕变换
	//Perspective_To_Screen_RENDERLIST4DV1(&rend_list, &cam);
	//Perspective_To_Screen_OBJECT4DV1(&object, &cam);

	//for (int i = 0; i < object.num_polys; i++)
	//{
		//sprintf_s(text, "%f %f %f\0", object.vlist_trans[i].x, object.vlist_trans[i].y, object.vlist_trans[i].z);
		//sprintf_s(text, "%f %f %f\0", object.vlist_local[i].x, object.vlist_local[i].y, object.vlist_local[i].z);
		//for (int j = 0; j < 3; j++)
	//	{
			//			sprintf_s(text, "%.2f %.2f %.2f\0", rend_list.poly_ptrs[i]->tvlist[j].x, rend_list.poly_ptrs[i]->tvlist[j].y, rend_list.poly_ptrs[i]->tvlist[j].z);
			//sprintf_s(text, "%.2f %.2f %.2f \0",
			//	object.vlist_trans[object.plist[i].vert[j]].x,
			//	object.vlist_trans[object.plist[i].vert[j]].y,
				//object.vlist_trans[object.plist[i].vert[j]].z
			//	);
			//TextOut(hdc, 200 + j * 130, 0 + i * 20, text, strlen(text));
	//	}
	//}
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			sprintf_s(text, "%.2f\0", cam.mcam.M[i][j]);
			TextOut(hdc, 20 + j * 40, 20 + i * 20, text, strlen(text));
		}
	}
//	TextOut(hdc, 10, 40, text, strlen(text));

	//Draw_RENDERLIST4DV1_Wrie16(&rend_list, hdc, 1);
	//Draw_OBJECT4DV1_Wire16(&object, hdc, 1);
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