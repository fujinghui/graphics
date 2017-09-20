#ifndef _GRAPH_ENGINE_H_
#define _GRAPH_ENGINE_H_
#include "math_engine.h"
#include <windows.h>
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
/*
typedef struct POLY_EX_TYPE_1{
POINT3D v[3];			//�����б�
}POLY_EX_1, *POLY_EX_1_PTR;
typedef struct POLY_EX_TYPE_2{
POINT3D_PTR vlist;
int vertices[3];
}POLY_EX_2, *POLY_EX_2_PTR;
typedef struct POLY4DV1_TYP{
int state;			//״̬��Ϣ
int attr;			//����ε���������
int color;			//����ε���ɫ
POINT4D_PTR vlist;	//�����б�
int vert[3];		//�����б�Ԫ�ص�����
}POLY4DV1, *POLY4DV1_PTR;
typedef struct POLYF4DV1_TYP{
int state;
int attr;
int color;
POINT4D vlist[3];		//�任ǰ�Ķ�������
POINT4D tvilist[3];		//�任��Ķ�������
POLYF4DV1_TYP *next;
POLYF4DV1_TYP *pre;
}POLYF4DV1, *POLYF4DV1_PTR;

typedef struct OBJECT4DV1_TYP{
int id;			//�������ֵID
char name[64];	//������ַ�������
int state;
int attr;
float avg_radius;	//�����ƽ���뾶
float max_radius;	//��������뾶
POINT4D world_pos;	//��������������ϵ�е�λ��
VECTOR4D dir;		//�����ھֲ������µ���ת�Ƕ�
//Ϊ�û�����������λ��������
VECTOR3D ux, uy, uz;	//��¼���峯��ľֲ�������
//���屻��תʱ����Ӧ����ת
int num_vertices;		//����Ķ�����

POINT4D vlist_local[64];	//�洢����ֲ����������
POINT4D vlist_trans[64];	//�洢����任������������
int num_polys;				//����Ķ������
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

//����κͶ�����������
#define POLY4DV1_ATTR_2SIDED 0x0001
#define POLY4DV1_ATTR_TRANSPARENT 0x0002
#define POLY4DV1_ATTR_8BITCOLOR 0x0004
#define POLY4DV1_ATTR_RGB16 0x0008
#define POLY4DV1_ATTR_RGB24 0x0010

#define POLY4DV1_ATTR_SHADE_MODE_PURE		0x0020
#define POLY4DV1_ATTR_SHADE_MODE_CONSTANT	0x0020	//����			��ִ�й��յ���
#define POLY4DV1_ATTR_SHADE_MODE_FLAT		0x0040	//ִ�й��յ���
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD	0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG		0x0100
#define POLY4DV1_ATTR_SHADE_MODE_FASTPHONG	0x0100	//����
#define POLY4DV1_ATTR_SHADE_MODE_TEXTURE	0x0200
//����κ����״ֵ̬
#define POLY4DV1_STATE_ACTIVE 0x0001
#define POLY4DV1_STATE_CLIPPED 0x0002
#define POLY4DV1_STATE_BACKFACE 0x0004
//����κͶ�����������
#define POLY4DV1_ATTR_2SIDED 0x0001
#define POLY4DV1_ATTR_TRANSPARENT 0x0002
#define POLY4DV1_ATTR_8BITCOLOR 0x0004
#define POLY4DV1_ATTR_RGB16 0x0008
#define POLY4DV1_ATTR_RGB24 0x0010

#define POLY4DV1_ATTR_SHADE_MODE_PURE 0x0020
#define POLY4DV1_ATTR_SHADE_MODE_FLAT 0x0040
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD 0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG 0x0100
//����κ����״ֵ̬
#define POLY4DV1_STATE_ACTIVE			0x0001
#define POLY4DV1_STATE_CLIPPED			0x0002
#define POLY4DV1_STATE_BACKFACE			0x0004

#define TRANSFORM_LOCAL_ONLY		0
#define TRANSFORM_TRANS_ONLY		1
#define TRANSFORM_LOCAL_TO_TRANS	2
#define TRANSFORM_LOCAL_ONLY		0
#define TRANSFORM_TRANS_ONLY		1
#define TRANSFORM_LOCAL_TO_TRANS	2

//�ü���־
#define CULL_OBJECT_X_PLANE           0x0001 // cull on the x clipping planes
#define CULL_OBJECT_Y_PLANE           0x0002 // cull on the y clipping planes
#define CULL_OBJECT_Z_PLANE           0x0004 // cull on the z clipping planes
#define CULL_OBJECT_XYZ_PLANES        0x0007 //(CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)

//����йس���
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
//�����UVNģʽ
#define UVN_MODE_SIMPLE            0 
#define UVN_MODE_SPHERICAL         1
//
//�����йس���
#define MATV1_ATTR_2SIDED			0x0001
#define MATV1_ATTR_TRANSPARENT		0X0002
#define MATV1_ATTR_8BITCOLOR		0x0004
#define MATV1_ATTR_RGB16			0x0008
#define MATV1_ATTR_RGB24			0x0010

#define MATV1_ATTR_SHADE_MODE_FASTPHONG	0x0020
#define MATV1_ATTR_SHADE_MODE_CONSTANT	0x0020					//��ִ�й��յĲ���
#define MATV1_ATTR_SHADE_MODE_EMMISIVE	0x0020		//����
#define MATV1_ATTR_SHADE_MODE_FLAT		0x0040					//ִ�й��յĲ���
#define MATV1_ATTR_SHADE_MODE_GOURAUD	0x0080
#define MATV1_ATTR_SHADE_MODE_FASTPHONG	0x0100
#define MATV1_ATTR_SHADE_MODE_TEXTURE	0x0200
//����״̬
#define MATV1_STATE_ACTIVE				0x0001
#define MAX_MATERIALS					256

////�ƹ��йصĳ���
#define LIGHTV1_ATTR_AMBIENT			0x0001		//������
#define LIGHTV1_ATTR_INFINITE			0x0002		//������Դ
#define LIGHTV1_ATTR_POINT				0x0004		//���Դ
#define LIGHTV1_ATTR_SPOTLIGHT1			0x0008		//�򵥾۹��
#define LIGHTV1_ATTR_SPOTLIGHT2			0x0010		//���Ӿ۹��

#define LIGHTV1_STATE_ON				1		//��Դ��
#define LIGHTV1_STATE_OFF				0		//��Դ�ر�
#define MAX_LIGHTS						8		//���֧�ֵĹ�Դ

#define SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))

//�����йس���
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
#define POLY4DV2_ATTR_SHADE_MODE_CONSTANT		0x0020		//����
#define POLY4DV2_ATTR_SHADE_MODE_EMISSIVE		0x0020		//����

#define POLY4DV2_ATTR_SHADE_MODE_FLAT			0x0040
#define POLY4DV2_ATTR_SHADE_MODE_GOURAUD		0x0080
#define POLY4DV2_ATTR_SHADE_MODE_PHONG			0x0100
#define POLY4DV2_ATTR_SHADE_MODE_FASTPHONG		0x0100		//����
#define POLY4DV2_ATTR_SHADE_MODE_TEXTURE		0x0200
//�����v1�汾������Ϊ�¼ӵ�����ֵ
#define POLY4DV2_ATTR_ENABLE_MATERIAL			0x0800		//���ݲ��ʽ��й��ռ���
#define POLY4DV2_ATTR_DISABLE_MATERIAL			0x1000		//���ݶ������ɫ���й���
//����κͶ�������״̬
#define POLY4DV2_STATE_NULL						0x0000
#define POLY4DV2_STATE_ACTIVE					0x0001
#define POLY4DV2_STATE_CLIPPED					0x0002
#define POLY4DV2_STATE_BACKFACE					0x0004
#define POLY4DV2_STATE_LIT						0x0008		//����Ϊ�й��յ���
//�������v1�汾�����³���Ϊ�ṩ��ɫģʽ��
#define VERTEX_FLAGS_OVERRIDE_MASK				0xf000		//����ֵ����ȡģ����Ϣ

#define VERTEX_FLAGS_OVERRIDE_CONSTANT			0x1000
#define VERTEX_FLAGS_OVERRIDE_EMISSIVE			0x1000		//����
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

#define VERTEX_FLAGS_OVERRIDE_MASK				0xf000		//������ȡ����
#define VERTEX_FLAGS_OVERRIDE_CONSTANT			0x1000
#define VERTEX_FLAGS_OVERRIDE_EMISSIVE			0x1000
#define VERTEX_FLAGS_OVERRIDE_PURE				0x1000
#define VERTEX_FLAGS_OVERRIDE_FLAT				0x2000
#define VERTEX_FLAGS_OVERRIDE_GOURAUD			0x4000
#define VERTEX_FLAGS_OVERRIDE_TEXTURE			0x8000

//ת���������곣��
#define VERTEX_FLAGS_INVERT_TEXTURE_U			0x0080		//��תu��		����������u(�ѿ�������ϵx��)
#define VERTEX_FLAGS_INVERT_TEXTURE_V			0x0100		//��תv��		����������v(�ѿ�������ϵy��)
#define VERTEX_FLAGS_INVERT_SWAP_UV				0x0800		//����������u��v����
//���峣��
#define OBJECT4DV2_MAX_VERTICES					4096
#define OBJECT4DV2_MAX_POLYS					8192
	//����״̬
#define OBJECT4DV2_STATE_NULLL					0x0000
#define OBJECT4DV2_STATE_ACTIVE					0x0001
#define OBJECT4DV2_STATE_VISIBLE				0x0002
#define OBJECT4DV2_STATE_CULLED					0x0004
	//�������v1�汾�����³���Ϊ������
#define OBJECT4DV2_ATTR_SINGLE_FRAME			0x0001		//��֡����
#define OBJECT4DV2_ATTR_MULTI_FRAME				0x0002		//��֡���壬����֧��.md2
#define OBJECT4DV2_ATTR_TEXTURES				0x0004		//ָ�������Ƿ����������Ķ����

//��Ⱦ�б�v2�汾
#define RENDERLIST4DV2_MAX_POLYS				32768
//3D��
//3D�� + ����
//3D�� + ���� + ��������
#define VERTEX4DTV1_ATTR_NULL					0x0000		//����Ϊ��
#define VERTEX4DTV1_ATTR_POINT					0x0001
#define VERTEX4DTV1_ATTR_NORMAL					0x0002
#define VERTEX4DTV1_ATTR_TEXTURE				0x0004

//��ü��йصĳ���--------------------------------------------------------------------------------�ü����----------------------------
#define CLIP_POLY_X_PLANE						0x0001		//���Ҳü�
#define CLIP_POLY_Y_PLANE						0x0002		//���²ü�
#define CLIP_POLY_Z_PLANE						0x0004		//Զ���ü�
#define CLIP_OBJECT_XYZ_PLANES					(CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)		//�ü�������


//������������
#define SORT_POLYLIST_AVGZ		0	//��ƽ��zֵ����
#define SORT_POLYLIST_NEARZ		1	//����Сzֵ����
#define SORT_POLYLIST_FARZ		2	//�����zֵ����
/////////////////////------------------v1�汾�ṹ��---------------------------------
typedef struct POLY4DV1_TYP{
	int state;				//״̬��Ϣ
	int attr;				//����ε���������
	int color;				//����ε���ɫ
	int color_light_trans;	//���·��ṹ��ĸ��ֶ���ͬ
	POINT4D_PTR vlist;		//�����б�
	int vert[3];			//�����б���Ԫ�ص�����
}POLY4DV1, *POLY4DV1_PTR;
typedef struct POLYF4DV1_TYP{
	int state;				//״̬��Ϣ
	int attr;				//����ε���������
	int color;				//����ε���ɫ
	int color_light_trans;	//���й��յ�����£��任�����ɫ
	POINT4D vlist[3];		//�������εĶ���
	POINT4D tvlist[3];		//�任��Ķ���
	POLYF4DV1_TYP *next;	//ָ����Ⱦ�б�����һ������ε�ָ��
	POLYF4DV1_TYP *prev;	//ָ����Ⱦ�б���ǰһ������ε�ָ��
}POLYF4DV1, *POLYF4DV1_PTR;
typedef struct OBJECT4DV1_TYP{
	int id;				//���������ID
	char name[64];		//������ַ�����
	int state;			//�����״̬
	int attr;			//���������
	float avg_radius;	//�����ƽ���뾶,������ײ���
	float max_radius;	//��������뾶
	POINT4D world_pos;	//��������������ϵ�е�λ��
	VECTOR4D dir;		//�����ھֲ�����ϵ�е���ת�Ƕ�
	VECTOR4D ux, uy, uz;//�ֲ�������

	int num_vertices;	//����Ķ�����
	POINT4D vlist_local[OBJECT4DV1_MAX_VERTICES];		//�ֲ���������
	POINT4D vlist_trans[OBJECT4DV1_MAX_VERTICES];		//�任����������
	int num_polys;		//��������Ķ������
	POLY4DV1 plist[OBJECT4DV1_MAX_POLYS];				//�������
														//POLY4DV1�ṹ���������ָ��ָ��vlist_localָ���vlist_transָ��
}OBJECT4DV1, *OBJECT4DV1_PTR;
typedef struct RENDERLIST4DV1_TYP{
	int state;			//��Ⱦ�б��״̬
	int attr;			//��Ⱦ�б������
	//��Ⱦ�б���һ��ָ������
	//����ÿ��ָ��ָ��һ���԰����ģ�����Ⱦ�Ķ������
	POLYF4DV1_PTR poly_ptrs[RENDERLIST4DV1_MAX_POLYS];
	//Ϊ����ÿ֡��Ϊ����η�����ͷŴ洢�ռ�
	//����δ洢�������������
	POLYF4DV1 poly_data[RENDERLIST4DV1_MAX_POLYS];
	int num_polys;		//��Ⱦ�б�����Ķ������Ŀ
}RENDERLIST4DV1, *RENDERLIST4DV1_PTR;
typedef struct CAM4DV1_TYP{
	int state;		//���״̬
	int attr;		//�������
	POINT4D pos;	//�������������ϵ�е�λ��
	VECTOR4D dir;	//ŷ���ǶȻ�UVN���ģ�͵�ע�ӷ���

	VECTOR4D u;		//UVN���ģ�͵ĳ�������
	VECTOR4D v;		//uvn�ֱ�Ϊ��x,y,z���ϵ�ƫ�ƽǶ�
	VECTOR4D n;
	POINT4D target;	//UVNģ�͵�Ŀ��λ��

	float view_dist;
	float view_dist_h;	//ˮƽ�Ӿ�ʹ�ֱ�Ӿ�
	float view_dist_v;	//

	float fov;

	//3D�ü���
	//�����Ұ����90�ȣ�3D�ü��淽�̽�Ϊһ����ƽ�淽��
	float near_clip_z;		//���ü���
	float far_clip_z;		//Զ�ü���

	PLANE3D rt_clip_plane;	//�Ҳü���
	PLANE3D lt_clip_plane;	//��ü���
	PLANE3D tp_clip_plane;	//�ϲü���
	PLANE3D bt_clip_plane;	//�²ü���

	float viewplane_width;	//��ƽ��Ŀ�Ⱥ͸߶�
	float viewplane_height;	//���ڹ�һ��ͶӰ��Ϊ2X2�������С���ӿڻ���Ļ������ͬ

	//��Ļ���ӿ���ͬ���
	float viewport_width;
	float viewport_height;
	float viewport_center_x;
	float viewport_center_y;

	//��߱�
	float aspect_ratio;
	//�Ƿ���Ҫ��������ȡ���ڱ任����
	//���磬���ֹ���ʽ����͸�ӱ任����Ļ�任ʱ������Ҫ��Щ����
	//��Ȼ�ṩ��Щ��������������
	MATRIX4X4 mcam;
	MATRIX4X4 mper;
	MATRIX4X4 mscr;
}CAM4DV1, *CAM4DV1_PTR;

//������ؽṹ��
typedef struct RGBAV1_TYPE{
	union{
		int rgba;						//ѹ����ʽ
		UCHAR rgba_M[4];				//�����ʽ
		struct { UCHAR a, b, g, r; };	//��ʾ���Ƹ�ʽ
	};
}RGBAV1, *RGBAV1_PTR;
typedef struct MATV1_TYP{
	int state;			//���ʵ�״̬
	int id;				//����id��ָ��������������
	char name[64];		//��������
	int attr;			//���ԣ���ɫģʽ����ɫ�����������������Լ�����������
	RGBAV1 color;		//������ɫ
	float ka, kd, ks, power;	//�Ի����⣬ɢ��⣬���淴���ķ���ϵ���;��淴��ָ��
								//�����Ƕ����ı������ܶཨģ���򶼲������ָ�ʽ
								//ambient diffuse specluar
	RGBAV1 ra, rd, rs;			//Ԥ�ȼ���õ�����ɫ�ͷ���ϵ���Ļ�
	char texture_file[80];		//����������ļ���λ��
	BITMAP texture;				//����ͼ
}MATV1, *MATV1_PTR;
//��Դ�йص����ݽṹ
typedef struct LIGHTV1_TYP{
	int state;					//��Դ״̬
	int id;						//��Դid
	int attr;					//��Դ���ͼ���������
	
	RGBAV1 c_ambient;			//������ǿ��
	RGBAV1 c_diffuse;			//ɢ���ǿ��
	RGBAV1 c_specular;			//���淴���ǿ��
	POINT4D pos;				//��Դλ��
	VECTOR4D dir;				//��Դ����
	float kc, kl, kq;			//˥������
	float spot_inner;			//�۹����׶��
	float spot_outer;			//�۹����׶��
	float pf;					//�۹��ָ������
}LIGHTV1, *LIGHTV1_PTR;
//-----------------------------------------v1�汾�ṹ��-------------------------------------
//-----------------------------------------v2�汾�ṹ��-------------------------------------
typedef struct VERTEX4DTV1_TYP{
	union{
		float M[12];
		struct{
			float x, y, z, w;		//��
			float nx, ny, nz, nw;	//����(�������)
			float u0, v0;			//��������
			float i;				//�������մ����Ķ�����ɫ
			int attr;				//����
		};
		struct{
			POINT4D v;				//����
			VECTOR4D n;				//����
			POINT2D t;				//��������
		};
	};
}VERTEX4DTV1, *VERTEX4DTV1_PTR;
typedef struct BITMAP_IMAGE_TYP
{
	int state;          // state of bitmap			����ͼ״̬λ
	int attr;           // attributes of bitmap		����
	int x, y;            // position of bitmap		
	int width, height;  // size of bitmap
	int num_bytes;      // total bytes of bitmap	��������ͼ��ռ���ֽڴ�С
	int bpp;            // bits per pixel			һ��С���ص���ռ���ֽڴ�С
	UCHAR *buffer;      // pixels of bitmap			����ͼ����	

} BITMAP_IMAGE, *BITMAP_IMAGE_PTR;

typedef struct POLY4DV2_TYP{
	int state;							//״̬��Ϣ
	int attr;							//����ε���ɫ
	int color;
	int light_color;
	int lit_color[3];					//���ڴ洢�������մ�������ɫ
										//���ں㶨��ɫ���������ɫ�洢�ڵ�һ��Ԫ����
										//����Gouraud��ɫ��������ɫ�ֱ�洢������Ԫ����
	BITMAP_IMAGE_PTR texture;			//ָ�������ָ�룬���ڼ򵥵�����ӳ��
	int mati;							//-1��ʾû�в���
	VERTEX4DTV1_PTR vlist;				//�����б�
	POINT2D_PTR tlist;					//���������б�
	int vert[3];						//ָ�򶥵��б������
	int text[3];						//ָ�����������б������
	float nlength;						//���߳���
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
	int id;								//���������ID
	char name[64];						//���������
	int state;							//�����״̬
	int attr;							//���������
	int mati;							//�����ģ�-1��ʾû�в���
	float *avg_radius;					//����OBJECTT4DV2_MAX_FRAMES��Ԫ��
	float *max_radius;					//����OBJECT4DV2_MAX_FRAMES��Ԫ�أ���������뾶
	POINT4D world_pos;					//����������������е�λ��
	VECTOR4D dir;						//�����ھֲ�����ϵ�µ���ת�Ƕ�
										//�����λ��������
	VECTOR4D ux, uy, uz;				//���ڼ�¼����ľֲ�������
										//���屻��תʱ���ǽ��Զ�����
	int num_vertices;					//ÿ֡�����Ķ�����
	int num_frames;						//�����ģ�֡��
	int total_vertices;					//ȫ������
	int curr_frame;						//��ǰ֡�����ֻ��һ֡����Ϊ0
	
	VERTEX4DTV1_PTR vlist_local;		//�ֲ���������
	VERTEX4DTV1_PTR vlist_trans;		//�任��Ķ�������
	//��������ָ�����ָ�򶥵��б�Ŀ�ͷ
	VERTEX4DTV1_PTR head_vlist_local;
	VERTEX4DTV1_PTR head_vlist_trans;

	POINT2D_PTR tlist;					//���������б�
	BITMAP_IMAGE_PTR texture;			//ָ��������Ϣ��ָ��

	int num_polys;						//���������еĶ������
	POLY4DV2_PTR plist;					//ָ�����ε�ָ��
	//ָ����ǰ֡
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
	//��С����
	if (y < x)	SWAP(x, y, temp);
	if (z < y)  SWAP(y, z, temp);
	if (y < x)  SWAP(y, x, temp);
	int dist = (z + 11 * (y >> 5) + (x >> 2));
	return ((float)(dist >> 10));
}
//--------------------------------------------------------v2�汾�ṹ�����------------------------------------------------------------------



//--------------------------------------------------------v1�汾����------------------------------------------------------------------------
void Build_XYZ_Rotation_MATRIX4X4(float theta_x, float theta_y, float theta_z, MATRIX4X4_PTR mrot);
//������Ⱦ�б�������������Ⱦ�б�����������һ֡ʹ��
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
	int cam_attr,				//�������
	POINT4D_PTR cam_pos,		//����ĳ�ʼλ��
	VECTOR4D_PTR cam_dir,		//����ĳ�ʼ�Ƕ�
	POINT4D_PTR cam_target,		//UVN����ĳ�ʼĿ��
	float near_clip_z,			//���ü����Զ�ü���
	float far_clip_z,			//
	float fov,					//��Ұ����λΪ��
	float viewport_width,		//��Ļ/�ӿڵĴ�С
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
void Perspective_To_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);					//ת������Ļ����
void Perspective_To_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
void Camera_To_Perspective_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
void Camera_To_Perspective_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
void Cull_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list);
//��Ⱦ����
void Draw_OBJECT4DV1_Wire16(OBJECT4DV1_PTR obj, HDC hdc, int lpitch);
void Draw_RENDERLIST4DV1_Wrie16(RENDERLIST4DV1_PTR rend_list, HDC hdc, int lpitch);
void Draw_OBJECT4DV1_Solid16(OBJECT4DV1_PTR obj, HDC hdc, int lpitch);
void Draw_RENDERLIST4DV1_Solid16(RENDERLIST4DV1_PTR rend_list, HDC hdc, int lpitch);

//���ʺ���
void Destroy_Bitmap(BITMAP *bitmap);		//���ٲ���
int Reset_Materials_MATV1(void);
//��Դ����
int Reset_Lights_LIGHTV1(void);
int Init_Light_LIGHTV1(int index, int _state, int _attr, RGBAV1 _c_ambient, RGBAV1 _c_diffuse, RGBAV1 _c_specular, POINT4D_PTR _pos, VECTOR4D_PTR _dir, float kc, float kl, float kq, float _apot_inner, float _spot_outer, float _pf);
int Light_OBJECT4DV1_World16(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam,int max_lights);
int Light_RENDERLIST4DV1_World(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam, int max_lights);


///�������ܺ���
void Sort_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, int sort_method);
inline int Compare_AvgZ_POLYF4DV1(const void *arg1, const void *arg2);
inline int Compare_NearZ_POLY4DV1(const void *arg1, const void *arg2);
inline int Compare_FarZ_POLY4DV1(const void *arg1, const void *arg2);
void qsort(POLYF4DV1_PTR *data, int s, int e, int *compare(const void *element1, const void *element2));

//-----------------------------------------------------------v1�汾��������--------------------------------------------------------------------------





//-----------------------------------------------------------v2�汾�ĺ���----------------------------------------------------------------------------
/////////////////////////�����������/////////////////////
void Transform_OBJECT4DV2(OBJECT4DV2_PTR obj, MATRIX4X4_PTR mt, int coord_select, int transform_basis);
void Build_CAM4DV2_Matrix_Euler(CAM4DV1_PTR cam, int cam_ort_seq);
void Build_CAM4DV2_Matrix_UVN(CAM4DV1_PTR cam, int mode);
void Model_To_World_OBJECT4DV2(OBJECT4DV2_PTR obj, int coord_select = TRANSFORM_LOCAL_TO_TRANS);
void World_To_Camera_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
int Cull_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam, int cull_flags);
void Reset_OBJECT4DV2(OBJECT4DV2_PTR obj);
void Remove_Backfaces_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
void Camera_To_Perspective_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
void Perspective_To_Screen_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);					//ת������Ļ����
void Camera_To_Perspective_Screen_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
int Light_OBJECT4DV2_World16(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam, int max_lights);
void Draw_OBJECT4DV2_Wire16(OBJECT4DV2_PTR obj, HDC hdc, int lpitch);
void Draw_OBJECT4DV12_Solid16(OBJECT4DV2_PTR obj, HDC hdc, int lpitch);
//////////////////////��Ⱦ�б�Ĳ���/////////////////////
void Reset_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list);
void Transform_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, MATRIX4X4_PTR mt, int coord_select);
void Remove_Backfaces_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);
void World_To_Camera_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);
void Camera_To_Perspective_RENDERLSIT4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);
void Perspective_To_Screen_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);
void Camera_To_Perspective_Screen_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);
int Insert_POLY4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, POLY4DV2_PTR poly);						//��POLY4DV2���뵽RENDLIST
int Insert_POLYF4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, POLYF4DV2_PTR poly);						//POLYF4DV2���뵽RENDLIST
int Insert_OBJECT4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, OBJECT4DV2_PTR obj, int insret_local = 0);						//��OBJECT4DV2CT���뵽RENDLIST
int Light_RENDERLIST4DV2_World16(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam, int max_ligths);
void Draw_RENDERLIST4DV2_Wire16(RENDERLIST4DV2_PTR rend_list, HDC hdc, int lpitch);
void Draw_RENDERLIST4DV2_Solid16(RENDERLIST4DV2_PTR rend_list, HDC hdc, int lpitch);
void Clip_Polys_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam, int clip_flags);

//��������
void Sort_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, int sort_method);
int Compare_AvgZ_POLYF4DV2(const void *arg1, const void *arg2);
int Compare_NearZ_POLYF4DV2(const void *arg1, const void *arg2);
int Compare_FarZ_POLYF4DV2(const void *arg1, const void *arg2);




//int Ligth_RENDERLIST4DV2_World(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam, int max_lights);

//��Ⱦ����

//�ü�����
//void Clip_Polys_RENDERLIST4DV2(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam, int clip_flag);
int Generate_Terrain_OBJECT4DV2(OBJECT4DV2_PTR obj, 
			float twidth,			//����������ϵ�еĿ��(X��)
			float theight,			//����������ϵ�еĳ���
			float vscale,			//�����������ܸ߶�
			char *height_map_file,	//256ɫ��λͼ���ļ���
			char *texture_map_file,	//����ͼ���ļ���
			int rgbcolor,			//û������ʱ���ε���ɫ
			VECTOR4D_PTR pos,		//��ʼλ��
			VECTOR4D_PTR rot,		//��ʼ��ת�Ƕ�
			int poly_attr
	);
//---------------------------------------------------------v2�汾��������----------------------------------------------------------------------------

OBJECT4DV1 CreateObject();
void DrawObject(HDC hdc, POINT3D points[], int count);



#endif