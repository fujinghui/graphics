#ifndef _GRAPH_ENGINE_H_
#define _GRAPH_ENGINE_H_
#include "math_engine.h"
#include <windows.h>
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
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

#define POLY4DV1_ATTR_SHADE_MODE_PURE 0x0020
#define POLY4DV1_ATTR_SHADE_MODE_FLAT 0x0040
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD 0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG 0x0100
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
#define CULL_OBJECT_XYZ_PLANES        (CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)

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
#define SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))
typedef struct POLY4DV1_TYP{
	int state;			//״̬��Ϣ
	int attr;			//����ε���������
	int color;			//����ε���ɫ
	POINT4D_PTR vlist;	//�����б�
	int vert[3];		//�����б���Ԫ�ص�����
}POLY4DV1, *POLY4DV1_PTR;
typedef struct POLYF4DV1_TYP{
	int state;			//״̬��Ϣ
	int attr;			//����ε���������
	int color;			//����ε���ɫ
	POINT4D vlist[3];	//�������εĶ���
	POINT4D tvlist[3];	//�任��Ķ���
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
	POLY4DV1 plist[OBJECT4DV1_MAX_POLYS];
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
	VECTOR4D v;
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
void Draw_OBJECT4DV1_Wire16(OBJECT4DV1_PTR obj, HDC hdc, int lpitch);
void Draw_RENDERLIST4DV1_Wrie16(RENDERLIST4DV1_PTR rend_list, HDC hdc, int lpitch);



OBJECT4DV1 CreateObject();
void DrawObject(HDC hdc, POINT3D points[], int count);

#endif