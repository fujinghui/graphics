#ifndef _GRAPH_ENGINE_H_
#define _GRAPH_ENGINE_H_
#include "math_engine.h"
#include <windows.h>
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
/*
typedef struct POLY_EX_TYPE_1{
	POINT3D v[3];			//顶点列表
}POLY_EX_1, *POLY_EX_1_PTR;
typedef struct POLY_EX_TYPE_2{
	POINT3D_PTR vlist;
	int vertices[3];
}POLY_EX_2, *POLY_EX_2_PTR;
typedef struct POLY4DV1_TYP{
	int state;			//状态信息
	int attr;			//多边形的物理属性
	int color;			//多边形的颜色
	POINT4D_PTR vlist;	//顶点列表
	int vert[3];		//顶点列表元素的索引
}POLY4DV1, *POLY4DV1_PTR;
typedef struct POLYF4DV1_TYP{
	int state;
	int attr;
	int color;
	POINT4D vlist[3];		//变换前的顶点坐标
	POINT4D tvilist[3];		//变换后的顶点坐标
	POLYF4DV1_TYP *next;
	POLYF4DV1_TYP *pre;
}POLYF4DV1, *POLYF4DV1_PTR;

typedef struct OBJECT4DV1_TYP{
	int id;			//物体的数值ID
	char name[64];	//物体的字符串名称
	int state;
	int attr;
	float avg_radius;	//物体的平均半径
	float max_radius;	//物体的最大半径
	POINT4D world_pos;	//物体在世界坐标系中的位置
	VECTOR4D dir;		//物体在局部坐标下的旋转角度
						//为用户定义的坐标或单位方向向量
	VECTOR3D ux, uy, uz;	//记录物体朝向的局部坐标轴
							//物体被旋转时，相应的旋转
	int num_vertices;		//物体的顶点数

	POINT4D vlist_local[64];	//存储顶点局部坐标的数组
	POINT4D vlist_trans[64];	//存储顶点变换后的坐标的数组
	int num_polys;				//物体的多边形数
	POLY4DV1 plist[128];		
}OBJECT4DV1, *OBJECT4DV1_PTR;
*/
#define RENDERLIST4DV1_MAX_POLYS 32768
#define OBJECT4DV1_MAX_VERTICES 1024
#define OBJECT4DV1_MAX_POLYS 1024

#define OBJECT4DV1_STATE_ACTIVE			0x0001
#define OBJECT4DV1_STATE_VISIBLE		0x0002
#define OBJECT4DV1_STATE_CULLED			0x0004

//多边形和多边形面的属性
#define POLY4DV1_ATTR_2SIDED 0x0001
#define POLY4DV1_ATTR_TRANSPARENT 0x0002
#define POLY4DV1_ATTR_8BITCOLOR 0x0004
#define POLY4DV1_ATTR_RGB16 0x0008
#define POLY4DV1_ATTR_RGB24 0x0010

#define POLY4DV1_ATTR_SHADE_MODE_PURE 0x0020
#define POLY4DV1_ATTR_SHADE_MODE_FLAT 0x0040
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD 0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG 0x0100
//多边形和面的状态值
#define POLY4DV1_STATE_ACTIVE 0x0001
#define POLY4DV1_STATE_CLIPPED 0x0002
#define POLY4DV1_STATE_BACKFACE 0x0004
//多边形和多边形面的属性
#define POLY4DV1_ATTR_2SIDED 0x0001
#define POLY4DV1_ATTR_TRANSPARENT 0x0002
#define POLY4DV1_ATTR_8BITCOLOR 0x0004
#define POLY4DV1_ATTR_RGB16 0x0008
#define POLY4DV1_ATTR_RGB24 0x0010

#define POLY4DV1_ATTR_SHADE_MODE_PURE 0x0020
#define POLY4DV1_ATTR_SHADE_MODE_FLAT 0x0040
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD 0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG 0x0100
//多边形和面的状态值
#define POLY4DV1_STATE_ACTIVE			0x0001
#define POLY4DV1_STATE_CLIPPED			0x0002
#define POLY4DV1_STATE_BACKFACE			0x0004

#define TRANSFORM_LOCAL_ONLY		0
#define TRANSFORM_TRANS_ONLY		1
#define TRANSFORM_LOCAL_TO_TRANS	2
#define TRANSFORM_LOCAL_ONLY		0
#define TRANSFORM_TRANS_ONLY		1
#define TRANSFORM_LOCAL_TO_TRANS	2

//裁剪标志
#define CULL_OBJECT_X_PLANE           0x0001 // cull on the x clipping planes
#define CULL_OBJECT_Y_PLANE           0x0002 // cull on the y clipping planes
#define CULL_OBJECT_Z_PLANE           0x0004 // cull on the z clipping planes
#define CULL_OBJECT_XYZ_PLANES        (CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)

//相机有关常量
#define CAM_MODE_ELUER	0
#define CAM_MODE_UVN	1

#define CAM_ROT_SEQ_XYZ  0
#define CAM_ROT_SEQ_YXZ  1
#define CAM_ROT_SEQ_XZY  2
#define CAM_ROT_SEQ_YZX  3
#define CAM_ROT_SEQ_ZYX  4
#define CAM_ROT_SEQ_ZXY  5

#define CAM_PROJ_NORMALIZED        0x0001
#define CAM_PROJ_SCREEN            0x0002
#define CAM_PROJ_FOV90             0x0004
//摄像机UVN模式
#define UVN_MODE_SIMPLE            0 
#define UVN_MODE_SPHERICAL         1
//
#define SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))
typedef struct POLY4DV1_TYP{
	int state;			//状态信息
	int attr;			//多边形的物理属性
	int color;			//多边形的颜色
	POINT4D_PTR vlist;	//顶点列表
	int vert[3];		//顶点列表中元素的索引
}POLY4DV1, *POLY4DV1_PTR;
typedef struct POLYF4DV1_TYP{
	int state;			//状态信息
	int attr;			//多边形的物理属性
	int color;			//多边形的颜色
	POINT4D vlist[3];	//该三角形的顶点
	POINT4D tvlist[3];	//变换后的顶点
	POLYF4DV1_TYP *next;	//指向渲染列表中下一个多边形的指针
	POLYF4DV1_TYP *prev;	//指向渲染列表中前一个多边形的指针
}POLYF4DV1, *POLYF4DV1_PTR;
typedef struct OBJECT4DV1_TYP{
	int id;				//物体的数字ID
	char name[64];		//物体的字符名称
	int state;			//物体的状态
	int attr;			//物体的属性
	float avg_radius;	//物体的平均半径,用于碰撞检测
	float max_radius;	//物体的最大半径
	POINT4D world_pos;	//物体在世界坐标系中的位置
	VECTOR4D dir;		//物体在局部坐标系中的旋转角度
	VECTOR4D ux, uy, uz;//局部坐标轴

	int num_vertices;	//物体的顶点数
	POINT4D vlist_local[OBJECT4DV1_MAX_VERTICES];		//局部坐标数组
	POINT4D vlist_trans[OBJECT4DV1_MAX_VERTICES];		//变换后坐标数组
	int num_polys;		//物体网格的多边形数
	POLY4DV1 plist[OBJECT4DV1_MAX_POLYS];
}OBJECT4DV1, *OBJECT4DV1_PTR;
typedef struct RENDERLIST4DV1_TYP{
	int state;			//渲染列表的状态
	int attr;			//渲染列表的属性
	//渲染列表是一个指针数组
	//其中每个指针指向一个自包含的，可渲染的多边形面
	POLYF4DV1_PTR poly_ptrs[RENDERLIST4DV1_MAX_POLYS];
	//为避免每帧都为多边形分配和释放存储空间
	//多边形存储在下述数组汇总
	POLYF4DV1 poly_data[RENDERLIST4DV1_MAX_POLYS];
	int num_polys;		//渲染列表包含的多边形数目
}RENDERLIST4DV1, *RENDERLIST4DV1_PTR;
typedef struct CAM4DV1_TYP{
	int state;		//相机状态
	int attr;		//相机属性
	POINT4D pos;	//相机在世界坐标系中的位置
	VECTOR4D dir;	//欧拉角度或UVN相机模型的注视方向

	VECTOR4D u;		//UVN相机模型的朝向向量
	VECTOR4D v;
	VECTOR4D n;
	POINT4D target;	//UVN模型的目标位置

	float view_dist;
	float view_dist_h;	//水平视距和垂直视距
	float view_dist_v;	//

	float fov;

	//3D裁减面
	//如果视野不足90度，3D裁剪面方程将为一般性平面方程
	float near_clip_z;		//近裁剪面
	float far_clip_z;		//远裁剪面

	PLANE3D rt_clip_plane;	//右裁剪面
	PLANE3D lt_clip_plane;	//左裁剪面
	PLANE3D tp_clip_plane;	//上裁剪面
	PLANE3D bt_clip_plane;	//下裁剪面

	float viewplane_width;	//视平面的宽度和高度
	float viewplane_height;	//对于归一化投影，为2X2，否则大小与视口或屏幕窗口相同

	//屏幕和视口是同义词
	float viewport_width;
	float viewport_height;
	float viewport_center_x;
	float viewport_center_y;

	//宽高比
	float aspect_ratio;
	//是否需要下述矩阵取决于变换方法
	//例如，以手工方式进行透视变换，屏幕变换时，不需要这些矩阵
	//而然提供这些矩阵提高了灵活性
	MATRIX4X4 mcam;
	MATRIX4X4 mper;
	MATRIX4X4 mscr;
}CAM4DV1, *CAM4DV1_PTR;
void Build_XYZ_Rotation_MATRIX4X4(float theta_x, float theta_y, float theta_z, MATRIX4X4_PTR mrot);

//重置渲染列表函数，以重置渲染列表，供动画的下一帧使用
void Reset_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list);
void Transform_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, MATRIX4X4_PTR mt, int coord_select);
void Transform_OBJECT4DV1(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mt, int coord_select, int transform_basis);
void Model_To_World_OBJECT4DV1(OBJECT4DV1_PTR obj, int coord_select = TRANSFORM_LOCAL_TO_TRANS);
void Model_To_World_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, POINT4D_PTR world_pos, int coord_select = TRANSFORM_LOCAL_TO_TRANS);
int Insert_POLY4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, POLY4DV1_PTR poly);
int Insert_POLYF4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, POLYF4DV1_PTR poly);
int Insert_OBJECT4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, OBJECT4DV1_PTR object, int insert_local);
void Init_CAM4DV1(CAM4DV1_PTR cam,
	int cam_attr,				//相机属性
	POINT4D_PTR cam_pos,		//相机的初始位置
	VECTOR4D_PTR cam_dir,		//相机的初始角度
	POINT4D_PTR cam_target,		//UVN相机的初始目标
	float near_clip_z,			//近裁剪面和远裁剪面
	float far_clip_z,			//
	float fov,					//视野，单位为度
	float viewport_width,		//屏幕/视口的大小
	float viewport_height		//
	);
void Build_CAM4DV1_Matrix_Euler(CAM4DV1_PTR cam, int cam_ort_seq);
void Build_CAM4DV1_Matrix_UVN(CAM4DV1_PTR cam, int mode);
void World_To_Camera_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
void World_To_Camera_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
int Cull_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam, int cull_flags);
void Reset_OBJECT4DV1(OBJECT4DV1_PTR obj);
void Remove_Backfaces_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
void Remove_Backfaces_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
void Camera_To_Perspective_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
void Camera_To_Perspective_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
void Perspective_To_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);					//转换成屏幕坐标
void Perspective_To_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
void Camera_To_Perspective_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
void Camera_To_Perspective_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
void Draw_OBJECT4DV1_Wire16(OBJECT4DV1_PTR obj, HDC hdc, int lpitch);
void Draw_RENDERLIST4DV1_Wrie16(RENDERLIST4DV1_PTR rend_list, HDC hdc, int lpitch);



OBJECT4DV1 CreateObject();
void DrawObject(HDC hdc, POINT3D points[], int count);

#endif