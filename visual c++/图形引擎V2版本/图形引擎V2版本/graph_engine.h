#ifndef _GRAPH_ENGINE_H_
#define _GRAPH_ENGINE_H_
#include "math_engine.h"
#include <windows.h>
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
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

#define _RGBA32BIT(r, g, b, a) ((a) + ((b) << 8) + ((g) << 16) + (r << 24))


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

#define POLY4DV1_ATTR_SHADE_MODE_PURE		0x0020
#define POLY4DV1_ATTR_SHADE_MODE_CONSTANT	0x0020	//别名			不执行光照的面
#define POLY4DV1_ATTR_SHADE_MODE_FLAT		0x0040	//执行光照的面
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD	0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG		0x0100
#define POLY4DV1_ATTR_SHADE_MODE_FASTPHONG	0x0100	//别名
#define POLY4DV1_ATTR_SHADE_MODE_TEXTURE	0x0200
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
#define CULL_OBJECT_XYZ_PLANES        0x0007 //(CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)

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
//材质有关常量
#define MATV1_ATTR_2SIDED			0x0001
#define MATV1_ATTR_TRANSPARENT		0X0002
#define MATV1_ATTR_8BITCOLOR		0x0004
#define MATV1_ATTR_RGB16			0x0008
#define MATV1_ATTR_RGB24			0x0010

#define MATV1_ATTR_SHADE_MODE_FASTPHONG	0x0020
#define MATV1_ATTR_SHADE_MODE_CONSTANT	0x0020					//不执行光照的材质
#define MATV1_ATTR_SHADE_MODE_EMMISIVE	0x0020		//别名
#define MATV1_ATTR_SHADE_MODE_FLAT		0x0040					//执行光照的材质
#define MATV1_ATTR_SHADE_MODE_GOURAUD	0x0080
#define MATV1_ATTR_SHADE_MODE_FASTPHONG	0x0100
#define MATV1_ATTR_SHADE_MODE_TEXTURE	0x0200
//材质状态
#define MATV1_STATE_ACTIVE				0x0001
#define MAX_MATERIALS					256

////灯光有关的常量
#define LIGHTV1_ATTR_AMBIENT			0x0001		//环境光
#define LIGHTV1_ATTR_INFINITE			0x0002		//无穷点光源
#define LIGHTV1_ATTR_POINT				0x0004		//点光源
#define LIGHTV1_ATTR_SPOTLIGHT1			0x0008		//简单聚光灯
#define LIGHTV1_ATTR_SPOTLIGHT2			0x0010		//复杂聚光灯

#define LIGHTV1_STATE_ON				1		//光源打开
#define LIGHTV1_STATE_OFF				0		//光源关闭
#define MAX_LIGHTS						8		//最多支持的光源

#define SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))

//纹理有关常量
#define TRI_TYPE_NONE				0	
#define TRI_TYPE_FLAT_TOP			1
#define TRI_TYPE_FLAT_BOTTOM		2
#define TRI_TYPE_FLAT_MASK			3
#define TRI_TYPE_FLAT_GENERAL		4
#define INTERP_LHS					0	
#define INTERP_RHS					1
#define MAXVERTICES_PER_POLY		6

#define POLY4DV2_ATTR_2SIDED		0x0001
#define POLY4DV2_ATTR_TRANSPARENT	0x0002
#define POLY4DV2_ATTR_8BITCOLOR		0x0004
#define POLY4DV2_ATTR_RGB16			0x0008
#define POLY4DV2_ATTR_RGB24			0x0010

#define POLY4DV2_ATTR_SHADE_MODE_PURE			0x0020
#define POLY4DV2_ATTR_SHADE_MODE_CONSTANT		0x0020		//别名
#define POLY4DV2_ATTR_SHADE_MODE_EMISSIVE		0x0020		//别名

#define POLY4DV2_ATTR_SHADE_MODE_FLAT			0x0040
#define POLY4DV2_ATTR_SHADE_MODE_GOURAUD		0x0080
#define POLY4DV2_ATTR_SHADE_MODE_PHONG			0x0100
#define POLY4DV2_ATTR_SHADE_MODE_FASTPHONG		0x0100		//别名
#define POLY4DV2_ATTR_SHADE_MODE_TEXTURE		0x0200
//相较于v1版本，以下为新加的属性值
#define POLY4DV2_ATTR_ENABLE_MATERIAL			0x0800		//根据材质进行光照计算
#define POLY4DV2_ATTR_DISABLE_MATERIAL			0x1000		//根据多边形颜色进行光照
//多边形和多边形面的状态
#define POLY4DV2_STATE_NULL						0x0000
#define POLY4DV2_STATE_ACTIVE					0x0001
#define POLY4DV2_STATE_CLIPPED					0x0002
#define POLY4DV2_STATE_BACKFACE					0x0004
#define POLY4DV2_STATE_LIT						0x0008		//设置为有光照的面
//（相较于v1版本，以下常量为提供着色模式）
#define VERTEX_FLAGS_OVERRIDE_MASK				0xf000		//掩码值，提取模型信息

#define VERTEX_FLAGS_OVERRIDE_CONSTANT			0x1000
#define VERTEX_FLAGS_OVERRIDE_EMISSIVE			0x1000		//别名
#define VERTEX_FLAGS_OVERRIDE_PURE				0x1000
#define VERTEX_FLAGS_OVERRIDE_FLAG				0x2000
#define VERTEX_FLAGS_OVERRIDE_GOURAUD			0x4000
#define VERTEX_FLAGS_OVERRIDE_TEXTURE			0x8000

#define VERTEX_FLAGS_INVERT_X					0x0001
#define VERTEX_FLAGS_INVERT_Y					0x0002
#define VERTEX_FLAGS_INVERT_Z					0x0004

#define VERTEX_FLAGS_SWAP_YZ					0x0008
#define VERTEX_FLAGS_SWAP_XZ					0x0010
#define VERTEX_FLAGS_SWAP_XY					0x0020
#define VERTEX_FLAGS_INVERT_WINDING_ORDER		0x0040

#define VERTEX_FLAGS_TRANSFORM_LOCAL			0x0200
#define VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD		0x0400

#define VERTEX_FLAGS_OVERRIDE_MASK				0xf000		//用于提取参数
#define VERTEX_FLAGS_OVERRIDE_CONSTANT			0x1000
#define VERTEX_FLAGS_OVERRIDE_EMISSIVE			0x1000
#define VERTEX_FLAGS_OVERRIDE_PURE				0x1000
#define VERTEX_FLAGS_OVERRIDE_FLAT				0x2000
#define VERTEX_FLAGS_OVERRIDE_GOURAUD			0x4000
#define VERTEX_FLAGS_OVERRIDE_TEXTURE			0x8000

//转换纹理坐标常量
#define VERTEX_FLAGS_INVERT_TEXTURE_U			0x0080		//反转u轴		纹理坐标轴u(笛卡尔坐标系x轴)
#define VERTEX_FLAGS_INVERT_TEXTURE_V			0x0100		//反转v轴		纹理坐标轴v(笛卡尔坐标系y轴)
#define VERTEX_FLAGS_INVERT_SWAP_UV				0x0800		//蒋雯丽坐标u与v呼唤
//物体常量
#define OBJECT4DV2_MAX_VERTICES					4096
#define OBJECT4DV2_MAX_POLYS					8192
	//物体状态
#define OBJECT4DV2_STATE_NULLL					0x0000
#define OBJECT4DV2_STATE_ACTIVE					0x0001
#define OBJECT4DV2_STATE_VISIBLE				0x0002
#define OBJECT4DV2_STATE_CULLED					0x0004
	//（相较于v1版本，以下常量为新增）
#define OBJECT4DV2_ATTR_SINGLE_FRAME			0x0001		//单帧物体
#define OBJECT4DV2_ATTR_MULTI_FRAME				0x0002		//多帧物体，用于支持.md2
#define OBJECT4DV2_ATTR_TEXTURES				0x0004		//指出物体是否包含带纹理的多边形

//渲染列表v2版本
#define RENDERLIST4DV2_MAX_POLYS				32768
//3D点
//3D点 + 法线
//3D点 + 法线 + 纹理坐标
#define VERTEX4DTV1_ATTR_NULL					0x0000		//顶点为空
#define VERTEX4DTV1_ATTR_POINT					0x0001
#define VERTEX4DTV1_ATTR_NORMAL					0x0002
#define VERTEX4DTV1_ATTR_TEXTURE				0x0004

//与裁剪有关的常量--------------------------------------------------------------------------------裁剪相关----------------------------
#define CLIP_POLY_X_PLANE						0x0001		//左右裁剪
#define CLIP_POLY_Y_PLANE						0x0002		//上下裁剪
#define CLIP_POLY_Z_PLANE						0x0004		//远近裁剪
#define CLIP_OBJECT_XYZ_PLANES					(CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)		//裁剪三个面


//辅助函数常量
#define SORT_POLYLIST_AVGZ		0	//按平均z值排序
#define SORT_POLYLIST_NEARZ		1	//按最小z值排序
#define SORT_POLYLIST_FARZ		2	//按最大z值排序
/////////////////////------------------v1版本结构体---------------------------------
typedef struct POLY4DV1_TYP{
	int state;				//状态信息
	int attr;				//多边形的物理属性
	int color;				//多边形的颜色
	int color_light_trans;	//与下方结构体的该字段相同
	POINT4D_PTR vlist;		//顶点列表
	int vert[3];			//顶点列表中元素的索引
}POLY4DV1, *POLY4DV1_PTR;
typedef struct POLYF4DV1_TYP{
	int state;				//状态信息
	int attr;				//多边形的物理属性
	int color;				//多边形的颜色
	int color_light_trans;	//在有光照的情况下，变换后的颜色
	POINT4D vlist[3];		//该三角形的顶点
	POINT4D tvlist[3];		//变换后的顶点
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
	POLY4DV1 plist[OBJECT4DV1_MAX_POLYS];				//面的数据
														//POLY4DV1结构体里的数据指针指向vlist_local指针和vlist_trans指针
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
	VECTOR4D v;		//uvn分别为在x,y,z轴上的偏移角度
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

//材质相关结构体
typedef struct RGBAV1_TYPE{
	union{
		int rgba;						//压缩格式
		UCHAR rgba_M[4];				//数组格式
		struct { UCHAR a, b, g, r; };	//显示名称格式
	};
}RGBAV1, *RGBAV1_PTR;
typedef struct MATV1_TYP{
	int state;			//材质的状态
	int id;				//材质id，指向材质数组的索引
	char name[64];		//材质名称
	int attr;			//属性：着色模式，着色方法，环境，纹理以及其他特殊标记
	RGBAV1 color;		//材质颜色
	float ka, kd, ks, power;	//对环境光，散射光，镜面反射光的发射系数和镜面反射指数
								//他们是独立的标量，很多建模程序都采用这种格式
								//ambient diffuse specluar
	RGBAV1 ra, rd, rs;			//预先计算得到的颜色和反射系数的积
	char texture_file[80];		//包含纹理的文件的位置
	BITMAP texture;				//纹理图
}MATV1, *MATV1_PTR;
//光源有关的数据结构
typedef struct LIGHTV1_TYP{
	int state;					//光源状态
	int id;						//光源id
	int attr;					//光源类型及其他属性
	
	RGBAV1 c_ambient;			//环境光强度
	RGBAV1 c_diffuse;			//散射光强度
	RGBAV1 c_specular;			//镜面反射光强度
	POINT4D pos;				//光源位置
	VECTOR4D dir;				//光源方向
	float kc, kl, kq;			//衰减因子
	float spot_inner;			//聚光灯内锥角
	float spot_outer;			//聚光灯外锥角
	float pf;					//聚光灯指数因子
}LIGHTV1, *LIGHTV1_PTR;
//-----------------------------------------v1版本结构体-------------------------------------
//-----------------------------------------v2版本结构体-------------------------------------
typedef struct VERTEX4DTV1_TYP{
	union{
		float M[12];
		struct{
			float x, y, z, w;		//点
			float nx, ny, nz, nw;	//法线(向量或点)
			float u0, v0;			//纹理坐标
			float i;				//经过光照处理后的顶点颜色
			int attr;				//属性
		};
		struct{
			POINT4D v;				//顶点
			VECTOR4D n;				//法线
			POINT2D t;				//纹理坐标
		};
	};
}VERTEX4DTV1, *VERTEX4DTV1_PTR;
typedef struct BITMAP_IMAGE_TYP
{
	int state;          // state of bitmap			纹理图状态位
	int attr;           // attributes of bitmap		属性
	int x, y;            // position of bitmap		
	int width, height;  // size of bitmap
	int num_bytes;      // total bytes of bitmap	整个纹理图所占的字节大小
	int bpp;            // bits per pixel			一个小像素点所占的字节大小
	UCHAR *buffer;      // pixels of bitmap			纹理图数据	

} BITMAP_IMAGE, *BITMAP_IMAGE_PTR;

typedef struct POLY4DV2_TYP{
	int state;							//状态信息
	int attr;							//多边形的颜色
	int color;
	int light_color;
	int lit_color[3];					//用于存储经过光照处理后的颜色
										//对于恒定着色，多边形颜色存储在第一个元素中
										//对于Gouraud着色，顶点颜色分别存储在三个元素中
	BITMAP_IMAGE_PTR texture;			//指向纹理的指针，用于简单的纹理映射
	int mati;							//-1表示没有材质
	VERTEX4DTV1_PTR vlist;				//顶点列表
	POINT2D_PTR tlist;					//纹理坐标列表
	int vert[3];						//指向顶点列表的索引
	int text[3];						//指向纹理坐标列表的索引
	float nlength;						//法线长度
}POLY4DV2, *POLY4DV2_PTR;
typedef struct POLYF4DV2_TYP{
	int state;
	int attr;
	int color;
	int lit_color[4];
	BITMAP_IMAGE_PTR texture;
	int mati;
	float nlength;
	VECTOR4D normal;
	float avg_z;
	VERTEX4DTV1 vlist[3];
	VERTEX4DTV1 tvlist[3];
	POLYF4DV2_TYP *next;
	POLYF4DV2_TYP *prev;
}POLYF4DV2, *POLYF4DV2_PTR;

typedef struct OBJECT4DV2_TYP{
	int id;								//物体的数字ID
	char name[64];						//物体的名称
	int state;							//物体的状态
	int attr;							//物体的属性
	int mati;							//新增的，-1表示没有材质
	float *avg_radius;					//包含OBJECTT4DV2_MAX_FRAMES个元素
	float *max_radius;					//包含OBJECT4DV2_MAX_FRAMES个元素，物体的最大半径
	POINT4D world_pos;					//物体在世界做白皙中的位置
	VECTOR4D dir;						//物体在局部坐标系下的旋转角度
										//坐标或单位方向向量
	VECTOR4D ux, uy, uz;				//用于记录朝向的局部坐标轴
										//物体被旋转时他们将自动更新
	int num_vertices;					//每帧包含的顶点数
	int num_frames;						//新增的，帧数
	int total_vertices;					//全部顶点
	int curr_frame;						//当前帧，如果只有一帧，则为0
	
	VERTEX4DTV1_PTR vlist_local;		//局部顶点数组
	VERTEX4DTV1_PTR vlist_trans;		//变换后的顶点数组
	//上述两个指针必须指向顶点列表的开头
	VERTEX4DTV1_PTR head_vlist_local;
	VERTEX4DTV1_PTR head_vlist_trans;

	POINT2D_PTR tlist;					//纹理坐标列表
	BITMAP_IMAGE_PTR texture;			//指向纹理信息的指针

	int num_polys;						//物体网格中的多边形数
	POLY4DV2_PTR plist;					//指向多边形的指针
	//指定当前帧
	int Set_Frame(int frame);
}OBJECT4DV2, *OBJECT4DV2_PTR;
typedef struct RENDERLIST4DV2_TYP{
	int state;
	int attr;
	POLYF4DV2_PTR poly_ptrs[RENDERLIST4DV2_MAX_POLYS];
	POLYF4DV2 poly_data[RENDERLIST4DV2_MAX_POLYS];
	int num_polys;
}RENDERLIST4DV2, *RENDERLIST4DV2_PTR;

#define FCMP(a, b) ((fabs(a-b)<EPSILON_E3)>1:0)
inline void VERTEX4DTV1_COPY(VERTEX4DTV1_PTR vdst, VERTEX4DTV1_PTR vsrc)
{*vdst = *vsrc;}
inline void VECTEX4DTV1_INIT(VERTEX4DTV1_PTR vdst, VERTEX4DTV1_PTR vsrc)
{*vdst = *vsrc;}
inline float VECTOR4D_Length_Fast2(VECTOR4D_PTR va){
	int temp;
	int x, y, z;
	x = fabs(va->x) * 1024;
	y = fabs(va->y) * 1024;
	z = fabs(va->z) * 1024;
	//从小到大
	if (y < x)	SWAP(x, y, temp);
	if (z < y)  SWAP(y, z, temp);
	if (y < x)  SWAP(y, x, temp);
	int dist = (z + 11 * (y >> 5) + (x >> 2));
	return ((float)(dist >> 10));
}
//--------------------------------------------------------v2版本结构体结束------------------------------------------------------------------



//--------------------------------------------------------v1版本函数------------------------------------------------------------------------
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
int Insert_OBJECT4DV1_RENDERLIST4DV2(RENDERLIST4DV1_PTR rend_list, OBJECT4DV1_PTR object, int insert_local, int light_on);
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
void Cull_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list);
//渲染函数
void Draw_OBJECT4DV1_Wire16(OBJECT4DV1_PTR obj, HDC hdc, int lpitch);
void Draw_RENDERLIST4DV1_Wrie16(RENDERLIST4DV1_PTR rend_list, HDC hdc, int lpitch);
void Draw_OBJECT4DV1_Solid16(OBJECT4DV1_PTR obj, HDC hdc, int lpitch);
void Draw_RENDERLIST4DV1_Solid16(RENDERLIST4DV1_PTR rend_list, HDC hdc, int lpitch);

//材质函数
void Destroy_Bitmap(BITMAP *bitmap);		//销毁材质
int Reset_Materials_MATV1(void);
//光源函数
int Reset_Lights_LIGHTV1(void);
int Init_Light_LIGHTV1(int index, int _state, int _attr, RGBAV1 _c_ambient, RGBAV1 _c_diffuse, RGBAV1 _c_specular, POINT4D_PTR _pos, VECTOR4D_PTR _dir, float kc, float kl, float kq, float _apot_inner, float _spot_outer, float _pf);
int Light_OBJECT4DV1_World16(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam,int max_lights);
int Light_RENDERLIST4DV1_World(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam, int max_lights);


///辅助功能函数
void Sort_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, int sort_method);
inline int Compare_AvgZ_POLYF4DV1(const void *arg1, const void *arg2);
inline int Compare_NearZ_POLY4DV1(const void *arg1, const void *arg2);
inline int Compare_FarZ_POLY4DV1(const void *arg1, const void *arg2);
void qsort(POLYF4DV1_PTR *data, int s, int e, int *compare(const void *element1, const void *element2));

//-----------------------------------------------------------v1版本函数结束--------------------------------------------------------------------------





//-----------------------------------------------------------v2版本的函数----------------------------------------------------------------------------
/////////////////////////物体操作函数/////////////////////
void Transform_OBJECT4DV2(OBJECT4DV2_PTR obj, MATRIX4X4_PTR mt, int coord_select, int transform_basis);
void Build_CAM4DV2_Matrix_Euler(CAM4DV1_PTR cam, int cam_ort_seq);
void Build_CAM4DV2_Matrix_UVN(CAM4DV1_PTR cam, int mode);
void Model_To_World_OBJECT4DV2(OBJECT4DV2_PTR obj, int coord_select = TRANSFORM_LOCAL_TO_TRANS);
void World_To_Camera_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
int Cull_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam, int cull_flags);
void Reset_OBJECT4DV2(OBJECT4DV2_PTR obj);
void Remove_Backfaces_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
void Camera_To_Perspective_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
void Perspective_To_Screen_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);					//转换成屏幕坐标
void Camera_To_Perspective_Screen_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
int Light_OBJECT4DV2_World16(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam, int max_lights);
void Draw_OBJECT4DV2_Wire16(OBJECT4DV2_PTR obj, HDC hdc, int lpitch);
void Draw_OBJECT4DV12_Solid16(OBJECT4DV2_PTR obj, HDC hdc, int lpitch);
//////////////////////渲染列表的操作/////////////////////
void Reset_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list);
void Transform_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, MATRIX4X4_PTR mt, int coord_select);
void Remove_Backfaces_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);
void World_To_Camera_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);
void Camera_To_Perspective_RENDERLSIT4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);
void Perspective_To_Screen_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);
void Camera_To_Perspective_Screen_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);
int Insert_POLY4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, POLY4DV2_PTR poly);						//将POLY4DV2插入到RENDLIST
int Insert_POLYF4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, POLYF4DV2_PTR poly);						//POLYF4DV2插入到RENDLIST
int Insert_OBJECT4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, OBJECT4DV2_PTR obj, int insret_local = 0);						//将OBJECT4DV2CT插入到RENDLIST
int Light_RENDERLIST4DV2_World16(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam, int max_ligths);
void Draw_RENDERLIST4DV2_Wire16(RENDERLIST4DV2_PTR rend_list, HDC hdc, int lpitch);
void Draw_RENDERLIST4DV2_Solid16(RENDERLIST4DV2_PTR rend_list, HDC hdc, int lpitch);
void Clip_Polys_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam, int clip_flags);

//辅助功能
void Sort_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, int sort_method);
int Compare_AvgZ_POLYF4DV2(const void *arg1, const void *arg2);
int Compare_NearZ_POLYF4DV2(const void *arg1, const void *arg2);
int Compare_FarZ_POLYF4DV2(const void *arg1, const void *arg2);




//int Ligth_RENDERLIST4DV2_World(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam, int max_lights);

//渲染函数

//裁剪函数
//void Clip_Polys_RENDERLIST4DV2(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam, int clip_flag);
int Generate_Terrain_OBJECT4DV2(OBJECT4DV2_PTR obj, 
			float twidth,			//在世界坐标系中的宽度(X轴)
			float theight,			//在世界坐标系中的长度
			float vscale,			//地形中最大可能高度
			char *height_map_file,	//256色高位图的文件名
			char *texture_map_file,	//纹理图的文件名
			int rgbcolor,			//没有纹理时地形的颜色
			VECTOR4D_PTR pos,		//初始位置
			VECTOR4D_PTR rot,		//初始旋转角度
			int poly_attr
	);
//---------------------------------------------------------v2版本函数结束----------------------------------------------------------------------------

OBJECT4DV1 CreateObject();
void DrawObject(HDC hdc, POINT3D points[], int count);



#endif