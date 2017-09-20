#include <malloc.h>
#include <windows.h>
#include <stdio.h>
#include <float.h>
#include "math_engine.h"
#include "graph_engine.h"
#include "draw.h"
float cam_x = 0, cam_y = 0, cam_z = -1;
float ang_x = 0, ang_y = 0, ang_z = 0;
float camera_distance = 1;		//��ƽ�浽�ӵ�ľ���

MATV1 materials[MAX_MATERIALS];	//ϵͳ�еĲ���
int num_materials;				//��ǰ��������

LIGHTV1 lights[MAX_LIGHTS];		//ϵͳ�еĹ�Դ����
int num_lights;
MATRIX4X4 Tcam_inv = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	-cam_x, -cam_y, -cam_z, 1
};
MATRIX4X4 Rcamx_inv = {
	1, 0, 0, 0,
	0, cos(-ang_x), sin(-ang_x), 0,
	0, -sin(-ang_x), cos(-ang_x), 0,
	0, 0, 0, 1
};
MATRIX4X4 Rcamy_inv = {
	cos(-ang_y), 0, -sin(-ang_y), 0,
	0, 1, 0, 0,
	sin(-ang_y), 0, cos(-ang_y), 0,
	0, 0, 0, 1
};
MATRIX4X4 Rcamz_inv = {
	cos(-ang_z), sin(-ang_z), 0, 0,
	-sin(-ang_z), cos(-ang_z), 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

//RENDERLIST4DV1 rend_list;



int OBJECT4DV2::Set_Frame(int frame){
	if (!this)
	{
		return 0;
	}
	if (!(this->attr & OBJECT4DV2_ATTR_MULTI_FRAME))
		return 0;
	if (frame < 0)
		frame = 0;
	else
	{
		if (frame >= this->num_frames)
			frame = this->num_frames - 1;
	}
	//���õ�ǰ֡
	this->curr_frame = frame;
	//��ָ��ָ�򶥵��б��е���Ӧ��֡
	this->vlist_local = &(this->head_vlist_local[frame * this->num_vertices]);
	this->vlist_trans = &(this->head_vlist_trans[frame * this->num_vertices]);

	return 1;
}



OBJECT4DV1 CreateObject() {
	OBJECT4DV1 object;
	object.id = 0;
	object.state = 0;
	object.avg_radius = 17.3f;
	object.max_radius = 17.3f;
	//��������������ϵ�е�λ��
	object.world_pos.x = 0;
	object.world_pos.y = 0;
	object.world_pos.z = 0;
	//����ĳ���
	object.num_vertices = 8;	//��������
	POINT3D temp_verts[8] = {
		{ 10, 10, 10 },
		{ 10, 10, -10 },
		{ -10, 10, -10 },
		{ -10, 10, 10 },
		{ 10, -10, 10 },
		{ -10, -10, 10 },
		{ -10, -10, -10 },
		{ 10, -10, -10 }
	};
	for (int vertex = 0; vertex < 8; vertex++)
	{
		object.vlist_local[vertex].x = temp_verts[vertex].x;
		object.vlist_local[vertex].y = temp_verts[vertex].y;
		object.vlist_local[vertex].z = temp_verts[vertex].z;
		object.vlist_local[vertex].w = 1;
	}
	object.num_polys = 12;		//һ���������������ι���,�ܹ�6����
	int temp_poly_indices[12 * 3] = {
		0, 1, 2, 0, 2, 3,
		0, 7, 1, 0, 4, 7,
		1, 7, 6, 1, 6, 2,
		2, 6, 5, 2, 3, 5,
		0, 5, 4, 0, 3, 5,
		5, 6, 7, 4, 5, 7
	};
	for (int tri = 0; tri < 12; tri++)
	{
		object.plist[tri].state = 0;
		object.plist[tri].attr = 0;
		object.plist[tri].color = 0;
		object.plist[tri].vlist = object.vlist_local;

		object.plist[tri].vert[0] = temp_poly_indices[tri * 3 + 0];
		object.plist[tri].vert[1] = temp_poly_indices[tri * 3 + 1];
		object.plist[tri].vert[2] = temp_poly_indices[tri * 3 + 2];
	}

	return object;
}
//��������ϵ->��������ϵ->�������ϵ
void DrawObject(HDC hdc, POINT3D points[], int count){
	POINT3D *cube_world = NULL;
	POINT3D *cube_camera = NULL;		//yxz
	POINT3D *cube_per = NULL;
	POINT3D *cube_screen = NULL;
	MATRIX4X4 temp_matrix1, temp_matrix2, Tcam;
	int i;
	//ת������������
	cube_world = (POINT3D_PTR)malloc(sizeof(POINT3D)* count);
	if (cube_world == NULL)
		return;
	for (i = 0; i < count; i++)
	{
		//��ת
		//points[i].x = points[i].x * (float)cos(PI / 180.0) - points[i].z * (float)sin(PI / 180.0);
		//points[i].z = points[i].x * (float)cos(PI / 180.0) + points[i].z * (float)sin(PI / 180.0);

		cube_world[i].x = points[i].x;
		cube_world[i].y = points[i].y;
		cube_world[i].z = points[i].z;
	}
	//ת�������������
	cube_camera = (POINT3D_PTR)malloc(sizeof(POINT3D)* count);
	if (cube_camera == NULL)
	{
		free(cube_world);
		return;
	}
	Mat_Mul_4X4(&Tcam_inv, &Rcamy_inv, &temp_matrix1);
	Mat_Mul_4X4(&Rcamx_inv, &Rcamz_inv, &temp_matrix2);
	Mat_Mul_4X4(&temp_matrix1, &temp_matrix2, &Tcam);
	for (i = 0; i < count; i++)
	{
		Mat_Mul_VECTOR3D_4X4(&cube_world[i], &Tcam, &cube_camera[i]);
	}
	//�ü������ȵ�һϵ�б任
	//......

	//͸��ͶӰ�任
	cube_per = (POINT3D_PTR)malloc(sizeof(POINT3D)* count);
	if (cube_per == NULL)
	{
		free(cube_camera);
		free(cube_world);
		return;
	}
	for (i = 0; i < count; i++)
	{
		cube_per[i].x = cube_camera[i].x * camera_distance / cube_camera[i].z;
		cube_per[i].y = cube_camera[i].y * camera_distance / cube_camera[i].z;
		cube_per[i].z = cube_camera[i].z;
	}

	//ת������Ļ����
	cube_screen = (POINT3D_PTR)malloc(sizeof(POINT3D)* count);
	if (cube_screen == NULL)
	{
		free(cube_per);
		free(cube_camera);
		free(cube_world);
		return;
	}
	for (i = 0; i < count; i++)
	{
		cube_screen[i].x = (float)((cube_per[i].x + 1) * (0.5 * SCREEN_WIDTH - 0.5));
		cube_screen[i].y = (float)((SCREEN_HEIGHT - 1) - (cube_per[i].y + 1) * (0.5 * SCREEN_HEIGHT - 0.5));
	}
	for (i = 0; i < count; i += 3)
	{
		MoveToEx(hdc, (int)cube_screen[i].x, (int)cube_screen[i].y, NULL);
		LineTo(hdc, (int)cube_screen[i + 1].x, (int)cube_screen[i + 1].y);
		LineTo(hdc, (int)cube_screen[i + 2].x, (int)cube_screen[i + 2].y);
		LineTo(hdc, (int)cube_screen[i].x, (int)cube_screen[i].y);
	}
	TCHAR buffer[100];
	int y = 30;
	for (i = 0; i < count; i++)
	{
		sprintf_s(buffer, "x:%.2f y:%.2f z:%.2f\0", cube_camera[i].x, cube_camera[i].y, cube_camera[i].z);
		TextOut(hdc, 10, y, buffer, strlen(buffer));
		y += 20;
	}
	for (i = 0; i < count; i++)
	{
		sprintf_s(buffer, "x:%.2f y:%.2f z:%.2f\0", cube_per[i].x, cube_per[i].y, cube_per[i].z);
		TextOut(hdc, 10, y, buffer, strlen(buffer));
		y += 20;
	}
	MATRIX4X4 m1 = {
		1, 1, 1, 1,
		0, 4, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	MATRIX4X4 m2 = {
		2, 1, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	MATRIX4X4 m3;
	int j;
	Mat_Mul_4X4(&m1, &m2, &m3);
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			sprintf_s(buffer, "%.2f\0", m3.M[i][j]);
			TextOut(hdc, 200 + j * 40, 10 + i * 20, buffer, strlen(buffer));
		}
	}

}


void Build_XYZ_Rotation_MATRIX4X4(float theta_x,
	float theta_y,
	float theta_z,
	MATRIX4X4_PTR mrot)
{
	MATRIX4X4 mx, my, mz, mtmp;       // working matrices
	float sin_theta = 0, cos_theta = 0;   // used to initialize matrices
	int rot_seq = 0;                  // 1 for x, 2 for y, 4 for z

	// step 0: fill in with identity matrix
	MAT_IDENTITY_4X4(mrot);

	// step 1: based on zero and non-zero rotation angles, determine
	// rotation sequence
	if (fabs(theta_x) > EPSILON_E5) // x
		rot_seq = rot_seq | 1;

	if (fabs(theta_y) > EPSILON_E5) // y
		rot_seq = rot_seq | 2;

	if (fabs(theta_z) > EPSILON_E5) // z
		rot_seq = rot_seq | 4;

	// now case on sequence
	switch (rot_seq)
	{
	case 0: // no rotation
	{
				// what a waste!
				return;
	} break;

	case 1: // x rotation
	{
				// compute the sine and cosine of the angle
				cos_theta = Fast_Cos(theta_x);
				sin_theta = Fast_Sin(theta_x);

				// set the matrix up 
				Mat_Init_4X4(&mx, 1, 0, 0, 0,
					0, cos_theta, sin_theta, 0,
					0, -sin_theta, cos_theta, 0,
					0, 0, 0, 1);
				MAT_COPY_4X4(&mx, mrot);
				return;

	} break;
	case 2: // y rotation
	{
				// compute the sine and cosine of the angle
				cos_theta = Fast_Cos(theta_y);
				sin_theta = Fast_Sin(theta_y);

				// set the matrix up 
				Mat_Init_4X4(&my, cos_theta, 0, -sin_theta, 0,
					0, 1, 0, 0,
					sin_theta, 0, cos_theta, 0,
					0, 0, 0, 1);


				// that's it, copy to output matrix
				MAT_COPY_4X4(&my, mrot);
				return;

	} break;

	case 3: // xy rotation
	{
				// compute the sine and cosine of the angle for x
				cos_theta = Fast_Cos(theta_x);
				sin_theta = Fast_Sin(theta_x);

				// set the matrix up 
				Mat_Init_4X4(&mx, 1, 0, 0, 0,
					0, cos_theta, sin_theta, 0,
					0, -sin_theta, cos_theta, 0,
					0, 0, 0, 1);

				// compute the sine and cosine of the angle for y
				cos_theta = Fast_Cos(theta_y);
				sin_theta = Fast_Sin(theta_y);

				// set the matrix up 
				Mat_Init_4X4(&my, cos_theta, 0, -sin_theta, 0,
					0, 1, 0, 0,
					sin_theta, 0, cos_theta, 0,
					0, 0, 0, 1);

				// concatenate matrices 
				Mat_Mul_4X4(&mx, &my, mrot);
				return;

	} break;

	case 4: // z rotation
	{
				// compute the sine and cosine of the angle
				cos_theta = Fast_Cos(theta_z);
				sin_theta = Fast_Sin(theta_z);

				// set the matrix up 
				Mat_Init_4X4(&mz, cos_theta, sin_theta, 0, 0,
					-sin_theta, cos_theta, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);


				// that's it, copy to output matrix
				MAT_COPY_4X4(&mz, mrot);
				return;

	} break;

	case 5: // xz rotation
	{
				// compute the sine and cosine of the angle x
				cos_theta = Fast_Cos(theta_x);
				sin_theta = Fast_Sin(theta_x);

				// set the matrix up 
				Mat_Init_4X4(&mx, 1, 0, 0, 0,
					0, cos_theta, sin_theta, 0,
					0, -sin_theta, cos_theta, 0,
					0, 0, 0, 1);

				// compute the sine and cosine of the angle z
				cos_theta = Fast_Cos(theta_z);
				sin_theta = Fast_Sin(theta_z);

				// set the matrix up 
				Mat_Init_4X4(&mz, cos_theta, sin_theta, 0, 0,
					-sin_theta, cos_theta, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);

				// concatenate matrices 
				Mat_Mul_4X4(&mx, &mz, mrot);
				return;

	} break;

	case 6: // yz rotation
	{
				// compute the sine and cosine of the angle y
				cos_theta = Fast_Cos(theta_y);
				sin_theta = Fast_Sin(theta_y);

				// set the matrix up 
				Mat_Init_4X4(&my, cos_theta, 0, -sin_theta, 0,
					0, 1, 0, 0,
					sin_theta, 0, cos_theta, 0,
					0, 0, 0, 1);

				// compute the sine and cosine of the angle z
				cos_theta = Fast_Cos(theta_z);
				sin_theta = Fast_Sin(theta_z);

				// set the matrix up 
				Mat_Init_4X4(&mz, cos_theta, sin_theta, 0, 0,
					-sin_theta, cos_theta, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);

				// concatenate matrices 
				Mat_Mul_4X4(&my, &mz, mrot);
				return;

	} break;

	case 7: // xyz rotation
	{
				// compute the sine and cosine of the angle x
				cos_theta = Fast_Cos(theta_x);
				sin_theta = Fast_Sin(theta_x);

				// set the matrix up 
				Mat_Init_4X4(&mx, 1, 0, 0, 0,
					0, cos_theta, sin_theta, 0,
					0, -sin_theta, cos_theta, 0,
					0, 0, 0, 1);

				// compute the sine and cosine of the angle y
				cos_theta = Fast_Cos(theta_y);
				sin_theta = Fast_Sin(theta_y);

				// set the matrix up 
				Mat_Init_4X4(&my, cos_theta, 0, -sin_theta, 0,
					0, 1, 0, 0,
					sin_theta, 0, cos_theta, 0,
					0, 0, 0, 1);

				// compute the sine and cosine of the angle z
				cos_theta = Fast_Cos(theta_z);
				sin_theta = Fast_Sin(theta_z);

				// set the matrix up 
				Mat_Init_4X4(&mz, cos_theta, sin_theta, 0, 0,
					-sin_theta, cos_theta, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);

				// concatenate matrices, watch order!
				Mat_Mul_4X4(&mx, &my, &mtmp);
				Mat_Mul_4X4(&mtmp, &mz, mrot);

	} break;

	default: break;
	}
}





void Reset_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list){
	//���������ʼ�������ô��ݽ�������Ⱦ�б�
	//��Ϊ����β��뵽��������׼��
	//����汾����Ⱦ�б���һ��FACE4DV1ָ���������
	//ÿһ֡����Ҫ�����������

	//����ʹ��num_polys��������Ⱦ�б��а����Ķ������Ŀ
	//��˽�������Ϊ0
	//�����Ҫʹ��Ⱦ�б��ͨ�ã���Ҫ���ø���׳�ķ���
	//������������ָ���б�Ĺ����ж�
	rend_list->num_polys = 0;		//Ӳ������
}	//Reset_RENDERLI



void Transform_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, MATRIX4X4_PTR mt, int coord_select){
	//�������ʹ�ô��ݽ����ľ���
	//����Ⱦ�б��оֲ����������任��Ķ���������
	//���еĶ���ζ�����б任
	switch (coord_select){
	case TRANSFORM_LOCAL_ONLY:
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			//��ÿһ������ν��б任���Ȼ�ȡ��ǰ�����
			POLYF4DV1_PTR cur_poly = rend_list->poly_ptrs[poly];
			if ((cur_poly == NULL) || !(cur_poly->state & POLY4DV1_STATE_ACTIVE) || (cur_poly->state & POLY4DV1_STATE_CLIPPED) || (cur_poly->state & POLY4DV1_STATE_BACKFACE))
			{
				continue;			//��ǰ����β����б任
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				POINT4D presult;
				Mat_Mul_VECTOR4D_4X4(&cur_poly->vlist[vertex], mt, &presult);
				VECTOR4D_COPY(&cur_poly->vlist[vertex], &presult);
			}
		}
		break;
	case TRANSFORM_TRANS_ONLY:
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
			if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				POINT4D presult;
				Mat_Mul_VECTOR4D_4X4(&curr_poly->tvlist[vertex], mt, &presult);
				VECTOR4D_COPY(&curr_poly->tvlist[vertex], &presult);
			}
		}
		break;
	case TRANSFORM_LOCAL_TO_TRANS:
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
			if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex], mt, &curr_poly->tvlist[vertex]);
			}
		}
		break;
	default:break;
	}
}

void Transform_OBJECT4DV1(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mt, int coord_select, int transform_basis){
	switch (coord_select){
	case TRANSFORM_LOCAL_ONLY:
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			POINT4D presult;
			Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], mt, &presult);
			VECTOR4D_COPY(&obj->vlist_local[vertex], &presult);
		}
		break;
	case TRANSFORM_TRANS_ONLY:
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			POINT4D presult;
			Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex], mt, &presult);
			VECTOR4D_COPY(&obj->vlist_trans[vertex], &presult);
		}
		break;
	case TRANSFORM_LOCAL_TO_TRANS:
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			//	POINT4D presult;
			Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], mt, &obj->vlist_trans[vertex]);
		}
		break;
	default:break;
	}
	if (transform_basis)
	{
		VECTOR4D vresult;
		//��תux
		Mat_Mul_VECTOR4D_4X4(&obj->ux, mt, &vresult);
		//��תuy
		Mat_Mul_VECTOR4D_4X4(&obj->uy, mt, &vresult);
		//��תuz
		Mat_Mul_VECTOR4D_4X4(&obj->uz, mt, &vresult);

		VECTOR4D_COPY(&obj->uz, &vresult);
	}
}
void Model_To_World_OBJECT4DV1(OBJECT4DV1_PTR obj, int coord_select){
	//û��ʹ�þ���
	//������ľֲ�/ģ������任Ϊ��������
	//������洢������ı任��Ķ����б���
	//���������б�����world_pos�������������ƽ��
	//������洢������vlist_trans[] ��
	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			VECTOR4D_Add(&obj->vlist_local[vertex], &obj->world_pos, &obj->vlist_trans[vertex]);
		}
	}
	else
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			VECTOR4D_Add(&obj->vlist_trans[vertex], &obj->world_pos, &obj->vlist_trans[vertex]);
		}
	}
}

void Model_To_World_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, POINT4D_PTR world_pos, int coord_select){
	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			//��õ�ǰ�����
			POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
			//��ǰ���������û�б�����Ͳü�����ͬʱ���ڻ״̬�ҿɼ�ʱ
			if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			//����������������б任
			for (int vertex = 0; vertex < 3; vertex++)
			{
				VECTOR4D_Add(&curr_poly->vlist[vertex], world_pos, &curr_poly->tvlist[vertex]);
			}
		}
	}
	else
	{
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
			if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				VECTOR4D_Add(&curr_poly->tvlist[vertex], world_pos, &curr_poly->tvlist[vertex]);
			}
		}
	}
}
int Insert_POLY4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, POLY4DV1_PTR poly){
	if (rend_list->num_polys >= RENDERLIST4DV1_MAX_POLYS)
		return 0;
	rend_list->poly_ptrs[rend_list->num_polys] = &rend_list->poly_data[rend_list->num_polys];

	rend_list->poly_data[rend_list->num_polys].state = poly->state;
	rend_list->poly_data[rend_list->num_polys].attr = poly->attr;
	rend_list->poly_data[rend_list->num_polys].color = poly->color;

	VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].tvlist[0], &poly->vlist[poly->vert[0]]);
	VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].tvlist[1], &poly->vlist[poly->vert[1]]);
	VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].tvlist[2], &poly->vlist[poly->vert[2]]);

	VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].vlist[0], &poly->vlist[poly->vert[0]]);
	VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].vlist[1], &poly->vlist[poly->vert[1]]);
	VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].vlist[2], &poly->vlist[poly->vert[2]]);

	if (rend_list->num_polys == 0)
	{
		rend_list->poly_data[0].next = NULL;
		rend_list->poly_data[0].prev = NULL;
	}
	else
	{
		rend_list->poly_data[rend_list->num_polys].next = NULL;
		rend_list->poly_data[rend_list->num_polys].prev = &rend_list->poly_data[rend_list->num_polys - 1];
		rend_list->poly_data[rend_list->num_polys - 1].next = &rend_list->poly_data[rend_list->num_polys];
	}
	rend_list->num_polys++;
	return 1;
}


int Insert_POLYF4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, POLYF4DV1_PTR poly){
	if (rend_list->num_polys >= RENDERLIST4DV1_MAX_POLYS)
		return(0);
	rend_list->poly_ptrs[rend_list->num_polys] = &rend_list->poly_data[rend_list->num_polys];

	memcpy((void *)&rend_list->poly_data[rend_list->num_polys], (void *)poly, sizeof(POLYF4DV1));
	if (rend_list->num_polys == 0)
	{
		rend_list->poly_data[0].next = NULL;
		rend_list->poly_data[0].prev = NULL;
	}
	else
	{
		rend_list->poly_data[rend_list->num_polys].next = NULL;
		rend_list->poly_data[rend_list->num_polys].prev =
			&rend_list->poly_data[rend_list->num_polys - 1];

		rend_list->poly_data[rend_list->num_polys - 1].next =
			&rend_list->poly_data[rend_list->num_polys];
	}
	rend_list->num_polys++;
	return(1);
}
int Insert_OBJECT4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, OBJECT4DV1_PTR obj, int insert_local){
	if (!(obj->state & OBJECT4DV1_STATE_ACTIVE) || (obj->state & OBJECT4DV1_STATE_CULLED) || !(obj->state & OBJECT4DV1_STATE_VISIBLE))
		return 0;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV1_PTR curr_poly = &obj->plist[poly];

		//δ����ѱ�ɾ������ʾ�˱���
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue;
		POINT4D_PTR vlist_old = curr_poly->vlist;
		if (insert_local)
		{
			curr_poly->vlist = obj->vlist_local;
		}
		else
		{
			curr_poly->vlist = obj->vlist_trans;
		}
		if (!Insert_POLY4DV1_RENDERLIST4DV1(rend_list, curr_poly))
		{
			curr_poly->vlist = vlist_old;
			return 0;
		}
		curr_poly->vlist = vlist_old;
	}
	return (1);
}
int Insert_OBJECT4DV1_RENDERLIST4DV2(RENDERLIST4DV1_PTR rend_list, OBJECT4DV1_PTR obj, int insert_local, int lighting_on){
	//������ת��Ϊһ����������б�
	//Ȼ�󽫿ɼ������û�б��޳��Ͳü����Ķ���β��뵽��Ⱦ�б���
	//����insert_localָ��ʹ�ö����б�vlist_local����vlist_trans
	//lighting_onָ��֮ǰ�Ƿ�ִ���˹���
	//���lighting_on = 1�򽫶���β�����Ⱦ�б�ʱ����ʹ��������ɫֵ���Ƕ����ԭ������ɫ
	//�����岻���ڻ״̬�����޳����򲻿ɼ�
	if (!(obj->state & OBJECT4DV1_STATE_ACTIVE) || (obj->state & OBJECT4DV1_STATE_CULLED) || (obj->state & OBJECT4DV1_STATE_VISIBLE))
		return 0;
	//��ȡ��������Ķ����
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		//��ȡ�����
		POLY4DV1_PTR curr_poly = &obj->plist[poly];
		//������Ƿ�ɼ�
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue;
		//���ʹ�þֲ����꣬��ı�����ָ�򶥵��б�
		POINT4D_PTR vlist_old = curr_poly->vlist;
		if (insert_local)
		{
			curr_poly->vlist = obj->vlist_local;
		}
		else
		{
			curr_poly->vlist = obj->vlist_trans;
		}
		unsigned int base_color;
		//�ж�֮ǰ�Ƿ���ִ���˹���
		if (lighting_on)
		{
			base_color = (unsigned int)(curr_poly->color);
			curr_poly->color = (int)(base_color >> 16);
		}
		//��������
		if (!Insert_POLY4DV1_RENDERLIST4DV1(rend_list, curr_poly));
		{
			//����ʧ��
			curr_poly->vlist = vlist_old;
			curr_poly->color = base_color;
			return 0;
		}
		if (lighting_on)
		{
			curr_poly->color = base_color;
		}
		curr_poly->vlist = vlist_old;
	}
	return (1);
}
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
	){
	cam->attr = cam_attr;
	VECTOR4D_COPY(&cam->pos, cam_pos);
	VECTOR4D_COPY(&cam->dir, cam_dir);
	///����UVN�����˵
	VECTOR4D_INITXYZ(&cam->u, 1, 0, 0);
	VECTOR4D_INITXYZ(&cam->v, 0, 1, 0);
	VECTOR4D_INITXYZ(&cam->n, 0, 0, 1);

	if (cam_target != NULL)
		VECTOR4D_COPY(&cam->target, cam_target);
	else
		VECTOR4D_ZERO(&cam->target);
	cam->near_clip_z = near_clip_z;
	cam->far_clip_z = far_clip_z;
	cam->viewport_width = viewport_width;
	cam->viewport_height = viewport_height;

	cam->aspect_ratio = (float)viewport_width / (float)viewport_height;
	//�����о�������Ϊ��λ����
	MAT_IDENTITY_4X4(&cam->mcam);
	MAT_IDENTITY_4X4(&cam->mper);
	MAT_IDENTITY_4X4(&cam->mscr);
	//������ر���
	cam->fov = fov;
	//����ƽ���С����Ϊ2X(2/ar)
	cam->viewplane_width = 2.0f;
	cam->viewplane_height = 2.0f / cam->aspect_ratio;
	//����fov����ƽ���С�����Ӿ�
	float tan_fov_div2 = 1.0f / (float)tan(DEG_TO_RAD(fov / 2));
	/*
	fa*         *
	   *       *
	    *******
	     * * *
		  ***
		   *
	*/
	cam->view_dist = 0.5f * cam->viewplane_width * tan_fov_div2;
	if (fov == 90.0)
	{
		//�����ü���
		POINT3D pt_origin;
		VECTOR3D_INITXYZ(&pt_origin, 0, 0, 0);
		VECTOR3D vn;				//�淨��
		//�Ҳü���
		VECTOR3D_INITXYZ(&vn, 1, 0, -1);			//ƽ��x=z
		PLANE3D_Init(&cam->rt_clip_plane, &pt_origin, &vn, 1);
		//��ü��� ƽ��-x=z
		VECTOR3D_INITXYZ(&vn, -1, 0, -1);
		PLANE3D_Init(&cam->lt_clip_plane, &pt_origin, &vn, 1);
		//�ϲü��� ƽ�� y=z
		VECTOR3D_INITXYZ(&vn, 0, 1, -1);
		PLANE3D_Init(&cam->tp_clip_plane, &pt_origin, &vn, 1);
		//�²ü���
		VECTOR3D_INITXYZ(&vn, 0, -1, -1);
		PLANE3D_Init(&cam->bt_clip_plane, &pt_origin, &vn, 1);
	}
	else
	{
		POINT3D pt_origin;		//ƽ���ϵ�һ����
		VECTOR3D_INITXYZ(&pt_origin, 0, 0, 0);
		VECTOR3D vn;			//��ķ���
		//����fov��Ϊ90�ȣ�����Ƚϸ���
		//���ȼ����ʾ�ü�����ƽ��X-Z��Y-Z�ϵ�2DͶӰ������
		//Ȼ�������������������ֱ�������������ǲü���ķ���
		//�Ҳü��棬���ڲݸ�ֽ����֤���ַ���
		VECTOR3D_INITXYZ(&vn, cam->view_dist, 0, -cam->viewplane_width / 2.0f);
		PLANE3D_Init(&cam->rt_clip_plane, &pt_origin, &vn, 1);
		//��ü��棬������z�ᷴ���вü���ķ��ߣ����ĵ���ü���ķ���
		//��Ϊ�������ü����ǹ���Z��ԳƵģ����ֻ���x��
		VECTOR3D_INITXYZ(&vn, -cam->view_dist, 0, -cam->viewplane_width / 2.0f);
		PLANE3D_Init(&cam->lt_clip_plane, &pt_origin, &vn, 1);
		//�ϲü���
		VECTOR3D_INITXYZ(&vn, 0, cam->view_dist, -cam->viewplane_width / 2.0f);
		PLANE3D_Init(&cam->tp_clip_plane, &pt_origin, &vn, 1);
		//�²ü���
		VECTOR3D_INITXYZ(&vn, 0, -cam->view_dist, -cam->viewplane_width / 2.0f);
		PLANE3D_Init(&cam->bt_clip_plane, &pt_origin, &vn, 1);
	}
}
void Build_CAM4DV1_Matrix_Euler(CAM4DV1_PTR cam, int cam_rot_seq){
	//�����������ŷ���Ƕȼ�������任����
	//������洢����������������
	MATRIX4X4 mt_inv,			//���ƽ�ƾ���������
		mx_inv,			//�����x�����ת����������
		my_inv,			//�����y�����ת����������
		mz_inv,			//�����z�����ת����������
		mrot,			//������ת����������
		mtmp;			//���ڴ洢��ʱ����
	//��һ�����������λ�ü������ƽ�ƾ���������
	Mat_Init_4X4(&mt_inv,	//��λƽ�ƾ���
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam->pos.x, -cam->pos.y, -cam->pos.z, 1
		);
	//�ڶ�����������ת����������
	//Ҫ����������������󣬿��Խ���ת��
	//Ҳ���Խ�ÿ����ת�Ƕ�ȡ��
	//���ȼ���3����ת����������
	float theta_x = cam->dir.x;
	float theta_y = cam->dir.y;
	float theta_z = cam->dir.z;
	//����Ƕ�x�����Һ�����
	float cos_theta = Fast_Cos(theta_x);
	float sin_theta = -Fast_Sin(theta_x);
	//��������
	Mat_Init_4X4(&mx_inv, 
		1, 0, 0, 0,
		0, cos_theta, sin_theta, 0,
		0, -sin_theta, cos_theta, 0,
		0, 0, 0, 1
		);

	//����Ƕ�y�����Һ�����
	cos_theta = Fast_Cos(theta_y);
	sin_theta = -Fast_Sin(theta_y);
	Mat_Init_4X4(&my_inv,
		cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1
		);
	//����Ƕ�z�����Һ�����
	cos_theta = Fast_Cos(theta_z);
	sin_theta = -Fast_Sin(theta_z);
	Mat_Init_4X4(&mz_inv,
		cos_theta, sin_theta, 0, 0,
		-sin_theta, cos_theta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		);
	switch (cam_rot_seq)
	{
	case CAM_ROT_SEQ_XYZ:
		Mat_Mul_4X4(&mx_inv, &my_inv, &mtmp);
		Mat_Mul_4X4(&mtmp, &mz_inv, &mrot);
		break;
	case CAM_ROT_SEQ_YXZ:
		Mat_Mul_4X4(&my_inv, &mx_inv, &mtmp);
		Mat_Mul_4X4(&mtmp, &mz_inv, &mrot);
		break;
	case CAM_ROT_SEQ_XZY:
		Mat_Mul_4X4(&mx_inv, &mz_inv, &mtmp);
		Mat_Mul_4X4(&mtmp, &my_inv, &mrot);
		break;
	case CAM_ROT_SEQ_YZX:
		Mat_Mul_4X4(&my_inv, &mz_inv, &mtmp);
		Mat_Mul_4X4(&mtmp, &mx_inv, &mrot);
		break;
	case CAM_ROT_SEQ_ZYX:
		Mat_Mul_4X4(&mz_inv, &my_inv, &mtmp);
		Mat_Mul_4X4(&mtmp, &mx_inv, &mrot);
		break;
	case CAM_ROT_SEQ_ZXY:
		Mat_Mul_4X4(&mz_inv, &mx_inv, &mtmp);
		Mat_Mul_4X4(&mtmp, &my_inv, &mrot);
		break;
	default:
		break;
	}
	Mat_Mul_4X4(&mt_inv, &mrot, &cam->mcam);
}
void Build_CAM4DV1_Matrix_UVN(CAM4DV1_PTR cam, int mode){
	MATRIX4X4 mt_inv, mt_uvn, mtmp;
	Mat_Init_4X4(&mt_inv, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam->pos.x, -cam->pos.y, -cam->pos.z, 1);
	if (mode == UVN_MODE_SPHERICAL)
	{
		//ʹ����������ģʽ
		float phi = cam->dir.x;
		float theta = cam->dir.y;

		float sin_phi = Fast_Sin(phi);
		float cos_phi = Fast_Cos(phi);

		float sin_theta = Fast_Sin(theta);
		float cos_theta = Fast_Cos(theta);
		//����Ŀ����ڵ�λ�����ϵ�λ��
		cam->target.x = -1 * sin_phi * sin_theta;
		cam->target.y = 1 * cos_phi;
		cam->target.z = 1 * sin_phi * cos_theta;
	}
	//��һ�� n = <Ŀ��λ��-�۲�ο���>
	VECTOR4D_Build(&cam->pos, &cam->target, &cam->n);
	//�ڶ��� ��v����Ϊ<0, 1, 0>
	VECTOR4D_INITXYZ(&cam->v, 0, 1, 0);
	//������ u = (v *������� n)
	VECTOR4D_Cross(&cam->v, &cam->n, &cam->u);
	//���Ĳ� v = (n * u)
	VECTOR4D_Cross(&cam->n, &cam->u, &cam->v);
	//���岽 ���������������й�һ��
	VECTOR4D_Normalize(&cam->u);
	VECTOR4D_Normalize(&cam->v);
	VECTOR4D_Normalize(&cam->n);
	//��u��v��n���룬�õ�uvn��ת����
	Mat_Init_4X4(&mt_uvn, cam->u.x, cam->v.x, cam->n.x, 0,
		cam->u.y, cam->v.y, cam->n.y, 0,
		cam->u.z, cam->v.z, cam->n.z, 0,
		0, 0, 0, 1
		);
	Mat_Mul_4X4(&mt_inv, &mt_uvn, &cam->mcam);
}
void World_To_Camera_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam){
	//����һ�����ھ���ĺ���
	//�����ݴ��������任���󣬽��������������任Ϊ�������
	//����ȫ�����Ƕ���α���֪ʶ��vlist_trans[]�еĶ�����б任
	//���Ǳ任����֮һ,������ѡ�����Ⱦ�б���б任
	//��Ϊ��Ⱦ�б��еĶ���α�ʾ�ļ����嶼ͨ��Ŷ�������

	//�������ÿ������任Ϊ�������
	//������趥���Ѿ����任Ϊ��������
	//�ҽ��������vlist_trans[]��
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		//ʹ����������еľ���mcam�Զ�����б任
		POINT4D presult;
		//�Զ�����б任
		Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex], &cam->mcam, &presult);
		//��������ȥ
		VECTOR4D_COPY(&obj->vlist_trans[vertex], &presult);
	}
}
void World_To_Camera_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam){
	//�������ʱ���ھ���
	//�����ݴ��������任������Ⱦ�б���ÿ������α任Ϊ�������
	//�������ˮ�ߵ������Ѿ���ÿ������ת��Ϊ�����
	//�������ǲ��뵽��Ⱦ�б���

	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		//���������Ƿ���Ч
		//��ǰ��������δ��ڻ״̬���ɼ�ʱ�Ŷ������б任
		//�������߿����棬����ĸ����޹ؽ�Ҫ
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		for (int vertex = 0; vertex < 3; vertex++)
		{
			//ʹ����������еľ���mcam�Զ�����б任
			POINT4D presult;
			//�Զ�����б任
			Mat_Mul_VECTOR4D_4X4(&curr_poly->tvlist[vertex], &cam->mcam, &presult);
			//��������ȥ
			VECTOR4D_COPY(&curr_poly->tvlist[vertex], &presult);
		}
	}
}
int Cull_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam, int cull_flags){
	//�������ʱ���ھ����
	//�����ݴ���������Ϣ�ж������Ƿ��谭�Ӿ�����
	//����cull_flagsָ������Щ����\ִ���޳�
	//��ȡֵΪ�����޳���ǵ�OR
	//������屻�޳�������Ӧ��������״̬
	//�������������������嶼����Ч��
	//��1��+
	POINT4D sphere_pos;			//���ڴ洢��Χ�����ı任�������
	//�Ե���б任
	Mat_Mul_VECTOR4D_4X4(&obj->world_pos, &cam->mcam, &sphere_pos);
	//��2���������޳���Ƕ�����ִ���޳�����
	if (cull_flags & CULL_OBJECT_Z_PLANE)
	{
		//ֻ����Զ���ü������޳�����
		//ʹ��Զ���ü�����в���
		if (((sphere_pos.z + obj->max_radius) > cam->far_clip_z) || ((sphere_pos.z - obj->max_radius) < cam->near_clip_z))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}
	if (cull_flags & CULL_OBJECT_X_PLANE)
	{
		//ֻ�������Ҳü�����������޳�
		float z_test = (0.5f) * cam->viewplane_width * sphere_pos.z / cam->view_dist;
		if (((sphere_pos.x - obj->max_radius) > z_test)				//�ұ�
			|| ((sphere_pos.x + obj->max_radius) < -z_test))		//���
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}
	if (cull_flags & CULL_OBJECT_Y_PLANE)
	{
		//ֻ�������²ü������������޳�
		float z_test = (0.5f) * cam->viewplane_height * sphere_pos.z / cam->view_dist;
		if ((sphere_pos.y - obj->max_radius) > z_test || ((sphere_pos.y + obj->max_radius) < -z_test))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}
	return 0;
}
void Reset_OBJECT4DV1(OBJECT4DV1_PTR obj){
	//�������ʱ���ô��봫�������Ĳü�״̬
	//ÿһ����Ϸ֡����Ҫ���ô˺���
	RESET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV1_PTR curr_poly = &obj->plist[poly];
		//�ж϶�����Ƿ�ɼ�
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE))
		{
			continue;
		}
		//���ñ��ü����ͱ�����
		RESET_BIT(curr_poly->state, POLY4DV1_STATE_CLIPPED);
		RESET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
	}


}
void Remove_Backfaces_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam){
	//����������ǻ��ھ����
	//���Ǹ�����vlist_trans�еĶ��������Լ����λ��
	//��������ı�������
	//����ֻ���ö���εı���״̬
	//��������Ƿ��ѱ��޳�
	if (obj->state & OBJECT4DV1_STATE_CULLED)
	{
		return;
	}
	//���������ÿ�������
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		//��ȡ�����
		POLY4DV1_PTR curr_poly = &obj->plist[poly];
		//�ö�����Ƿ���Ч
		//�ж϶�����Ƿ�û�б��ü�����û�б��޳������ڻ״̬���ɼ��Ҳ���˫���
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->attr & POLY4DV1_ATTR_2SIDED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		//��ȡ�����б��еĶ�������
		//����β����԰����ģ����ǻ�������Ķ������б�
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];
		//���ǽ�ʹ�ñ任��Ķ���ζ����б�
		//��Ϊ�������������ڶ��㱻ת��Ϊ��������֮����ܽ���
		//��Ҫ�������ε��淨��
		//�����ǰ�˳ʱ�뷽�����е�
		VECTOR4D u, v, n;
		//��֪������������
		VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
		VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
		VECTOR4D_Cross(&u, &v, &n);								//�������ƽ��ķ���
		//����ָ���ӵ������
		VECTOR4D view;
		VECTOR4D_Build(&obj->vlist_trans[vindex_0], &cam->pos, &view);
		//������
		float dp = VECTOR4D_Dot(&n, &view);
		//���dp<0 �����β��ɼ�
		if (dp <= 0.0f)
		{
			SET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
		}
	}
}
void Remove_Backfaces_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam){
	//���ݶ�����б�����tvlist�����λ��������������б������ڱ���Ķ����
	//˳ʱ������ʾ���棬��ʱ�������ظ���
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		//��ȡ��ǰ�����
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		//�ö�����Ƿ���Ч
		//�ж϶�����Ƿ�û�б��ü�����û�б��޳������ڻ״̬���ɼ��Ҳ���˫���
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->attr & POLY4DV1_ATTR_2SIDED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		VECTOR4D u, v, n;
		//����u��v

		VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
		VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
		//������
		VECTOR4D_Cross(&u, &v, &n);
		//����ָ���ӵ������
		VECTOR4D view;
		VECTOR4D_Build(&curr_poly->tvlist[0], &cam->pos, &view);
		//������
		float dp = VECTOR4D_Dot(&n, &view);
		if (dp < 0.0f)
			SET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
	}
}
void Camera_To_Perspective_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam){
	//����������ǻ��ھ����
	//�����ݴ�����������������������ת��Ϊ͸������
	//�����������Ķ���α�ʡ
	//��ֻ�Ƕ�vlist_trans[]�еĶ�����б任
	//��ֻ��ִ��͸�ӱ任�ķ���֮һ�������ܲ��������ַ���
	//���Ƕ���Ⱦ�б���б任����Ϊ��Ⱦ�б��еĶ���α�ʾ����ͨ���˱����޳�
	//����������֪ʶ����ʵ��Ŀ�Ķ���д��
	//�����ܲ��Ὣ���������ı�������ˮ�ߵ�����׶�
	//��Ϊ�������ֻ��һ��������ǿɼ��ģ���������������ж���ζ����б任
	//�������ÿ������任Ϊ͸������
	//������������Ѿ����任Ϊ������꣬�ҽ���洢��vlist_trans[]��
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		float z = obj->vlist_trans[vertex].z;
		//��������Ĺ۲�����Զ�����б任
		obj->vlist_trans[vertex].x = cam->view_dist * obj->vlist_trans[vertex].x / z;
		obj->vlist_trans[vertex].y = cam->view_dist * obj->vlist_trans[vertex].y * cam->aspect_ratio / z;
	}
}
void Camera_To_Perspective_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		//��ȡ��ǰ�����
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		//�ö�����Ƿ���Ч
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;
			curr_poly->tvlist[vertex].x = cam->view_dist * curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = cam->view_dist * curr_poly->tvlist[vertex].y * cam->aspect_ratio / z;

		}
	}
}
void Perspective_To_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam){
	//ֻ��vlist_trans[]�еĶ�����б߹�
	float alpha = (0.5f * cam->viewport_width - 0.5f);
	float beta = (0.5f * cam->viewport_height - 0.5f);
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		obj->vlist_trans[vertex].x = alpha + alpha * obj->vlist_trans[vertex].x;
		obj->vlist_trans[vertex].y = beta - beta * obj->vlist_trans[vertex].y;
	}
}
void Perspective_To_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		float alpha = (0.5f * cam->viewport_width - 0.5f);
		float beta = (0.5f * cam->viewport_height - 0.5f);
		for (int vertex = 0; vertex < 3; vertex++)
		{
			curr_poly->tvlist[vertex].x = alpha + alpha * curr_poly->tvlist[vertex].x;
			curr_poly->tvlist[vertex].y = beta - beta * curr_poly->tvlist[vertex].y;
		}
	}
}
void Camera_To_Perspective_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam){

	float alpha = (0.5f * cam->viewport_width - 0.5f);
	float beta = (0.5f * cam->viewport_height - 0.5f);

	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		float z = obj->vlist_trans[vertex].z;
		//��������Ĺ۲�����Զ�����б任
		obj->vlist_trans[vertex].x = cam->view_dist * obj->vlist_trans[vertex].x / z;
		obj->vlist_trans[vertex].y = cam->view_dist * obj->vlist_trans[vertex].y * cam->aspect_ratio / z;

		//ת������Ļ����
		obj->vlist_trans[vertex].x = alpha + alpha * obj->vlist_trans[vertex].x;
		obj->vlist_trans[vertex].y = beta - beta * obj->vlist_trans[vertex].y;
	}
}
void Camera_To_Perspective_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		//��ȡ��ǰ�����
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		//�ö�����Ƿ���Ч
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}

		float alpha = (0.5f * cam->viewport_width - 0.5f);
		float beta = (0.5f * cam->viewport_height - 0.5f);
		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;
			//�����͸������
			curr_poly->tvlist[vertex].x = cam->view_dist * curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = cam->view_dist * curr_poly->tvlist[vertex].y * cam->aspect_ratio / z;

			//͸�����굽��Ļ����
			curr_poly->tvlist[vertex].x = alpha + alpha * curr_poly->tvlist[vertex].x;
			curr_poly->tvlist[vertex].y = beta - beta * curr_poly->tvlist[vertex].y;
		}
	}
}
void Cull_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		if (curr_poly->tvlist[0].z < 1 || curr_poly->tvlist[1].z < 1 || curr_poly->tvlist[2].z < 1)
			SET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
	}
}


void Draw_OBJECT4DV1_Wire16(OBJECT4DV1_PTR obj, HDC hdc, int lpitch){
	if (obj->state & OBJECT4DV1_STATE_CULLED)
		return;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		if (!(obj->plist[poly].state & POLY4DV1_STATE_ACTIVE) || (obj->plist[poly].state & POLY4DV1_STATE_CLIPPED) || (obj->plist[poly].state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		int vindex_0 = obj->plist[poly].vert[0];
		int vindex_1 = obj->plist[poly].vert[1];
		int vindex_2 = obj->plist[poly].vert[2];
		HPEN hPen = CreatePen(PS_SOLID, 1, obj->plist[poly].color_light_trans);
		HPEN old_hpen = (HPEN)SelectObject(hdc, hPen);
		DeleteObject(old_hpen);

		MoveToEx(hdc, (int)obj->vlist_trans[vindex_0].x, (int)obj->vlist_trans[vindex_0].y, NULL);
		LineTo(hdc, (int)obj->vlist_trans[vindex_1].x, (int)obj->vlist_trans[vindex_1].y);
		LineTo(hdc, (int)obj->vlist_trans[vindex_2].x, (int)obj->vlist_trans[vindex_2].y);
		LineTo(hdc, (int)obj->vlist_trans[vindex_0].x, (int)obj->vlist_trans[vindex_0].y);
	}
}
void Draw_OBJECT4DV1_Solid16(OBJECT4DV1_PTR obj, HDC hdc, int lpitch){
	if (obj->state & OBJECT4DV1_STATE_CULLED)
		return;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV1_PTR curr_poly = &obj->plist[poly];
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (obj->plist[poly].state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		//��ȡָ�����б�Ķ�������
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];
		//����������
		Draw_Triangle_2D(obj->vlist_trans[vindex_0].x, obj->vlist_trans[vindex_0].y, obj->vlist_trans[vindex_1].x, obj->vlist_trans[vindex_1].y, obj->vlist_trans[vindex_2].x, obj->vlist_trans[vindex_2].y, obj->plist[poly].color_light_trans, hdc, lpitch);
	//	Draw_Gouraud_Trangle(obj->vlist_trans[vindex_0].x, obj->vlist_trans[vindex_0].y, (float)0xffffff,
	//		obj->vlist_trans[vindex_1].x, obj->vlist_trans[vindex_1].y, (float)0xffffff,
	//		obj->vlist_trans[vindex_2].x, obj->vlist_trans[vindex_2].y, (float)0xffffff,
	//		hdc, lpitch);
	}
}
void Draw_RENDERLIST4DV1_Wrie16(RENDERLIST4DV1_PTR rend_list, HDC hdc, int lpitch){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		if (!(rend_list->poly_ptrs[poly]->state & POLY4DV1_STATE_ACTIVE) ||
			(rend_list->poly_ptrs[poly]->state & POLY4DV1_STATE_CLIPPED) ||
			(rend_list->poly_ptrs[poly]->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		HPEN hPen = CreatePen(0, 1, rend_list->poly_ptrs[poly]->color_light_trans);
		HPEN hPenOld = (HPEN)SelectObject(hdc, hPen);
		DeleteObject(hPenOld);
		MoveToEx(hdc, (int)rend_list->poly_ptrs[poly]->tvlist[0].x, (int)rend_list->poly_ptrs[poly]->tvlist[0].y, NULL);
		LineTo(hdc, (int)rend_list->poly_ptrs[poly]->tvlist[1].x, (int)rend_list->poly_ptrs[poly]->tvlist[1].y);
		LineTo(hdc, (int)rend_list->poly_ptrs[poly]->tvlist[2].x, (int)rend_list->poly_ptrs[poly]->tvlist[2].y);
		LineTo(hdc, (int)rend_list->poly_ptrs[poly]->tvlist[0].x, (int)rend_list->poly_ptrs[poly]->tvlist[0].y);
	}
}

void Draw_RENDERLIST4DV1_Solid16(RENDERLIST4DV1_PTR rend_list, HDC hdc, int lpitch){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		
		Draw_Triangle_2D(
			curr_poly->tvlist[0].x, curr_poly->tvlist[0].y,
			curr_poly->tvlist[1].x, curr_poly->tvlist[1].y,
			curr_poly->tvlist[2].x, curr_poly->tvlist[2].y,
			curr_poly->color_light_trans,
			hdc,
			lpitch
			);
		/*
		Draw_Gouraud_Trangle(
			curr_poly->tvlist[0].x, curr_poly->tvlist[0].y, curr_poly->color_light_trans,
			curr_poly->tvlist[1].x, curr_poly->tvlist[1].y, curr_poly->color_light_trans,
			curr_poly->tvlist[2].x, curr_poly->tvlist[2].y, curr_poly->color_light_trans,
			hdc, 
			lpitch
			);
			*/
	}
}
void Destroy_Bitmap(BITMAP *bitmap){
	free(bitmap->bmBits);
	bitmap->bmBits = NULL;
}

//���ʺ���
int Reset_Materials_MATV1(void){
	static int first_time = 1;
	//������ǵ�һ�ε��øú��������������в���
	if (first_time)
	{
		memset(materials, 0, MAX_MATERIALS * sizeof(MATV1));
		first_time = 0;
	}
	for (int curr_matt = 0; curr_matt < MAX_MATERIALS; curr_matt++)
	{
		//���ܲ����Ƿ񱻼�����ͷ���֮��ص�����ͼ
		Destroy_Bitmap(&materials[curr_matt].texture);
		memset(&materials[curr_matt], 0, sizeof(MATV1));
	}
	return 1;

}

//��Դ����
int Reset_Lights_LIGHTV1(void){
	static int first_time = 1;
	memset(lights, 0, MAX_LIGHTS * sizeof(LIGHTV1));
	num_lights = 0;
	first_time = 0;
	return 1;
}
int Init_Light_LIGHTV1(
	int index,						//Ҫ�����Ĺ�Դ������(0��MAX_LIGHTS-1)
	int _state,						//��Դ״̬
	int _attr,						//��Դ���ͼ�������
	RGBAV1 _c_ambient,				//������ǿ��
	RGBAV1 _c_diffuse,				//ɢ���ǿ��
	RGBAV1 _c_specular,				//���淴���ǿ��
	POINT4D_PTR _pos,				//��Դλ��
	VECTOR4D_PTR _dir,				//��Դ����
	float _kc, float _kl, float _kq,	//˥������
	float _spot_inner,				//�۹����׶��
	float _spot_outer,				//�۹����׶��
	float _pf)						//�۹��ָ������
{
	//����������ݴ���Ĳ�����ʼ����Դ
	//���øú���ʱ��Ϊȷ�������Ĺ�Դ��Ч������Ҫ�Ĳ���ֵ����Ϊ0
	if (index < 0 || index >= MAX_LIGHTS)
		return 0;
	lights[index].state			= _state;		//��Դ״̬
	lights[index].id			= index;		//��Դid
	lights[index].attr = _attr;					//��Դ���ͼ���������
	lights[index].c_ambient = _c_ambient;		//������ǿ��
	lights[index].c_diffuse = _c_diffuse;		//ɢ���ǿ��
	lights[index].c_specular = _c_specular;		//���淴���ǿ��
	lights[index].kc = _kc;						//���������ԣ�����˥������
	lights[index].kl = _kl;
	lights[index].kq = _kq;
	if (_pos)
		VECTOR4D_COPY(&lights[index].pos, _pos);
	if (_dir)
	{
		VECTOR4D_COPY(&lights[index].dir, _dir);
		VECTOR4D_Normalize(&lights[index].dir);
	}
	lights[index].spot_inner = _spot_inner;
	lights[index].spot_outer = _spot_outer;
	lights[index].pf = _pf;
	return index;
}
int Light_OBJECT4DV1_World16(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam, int max_lights){
	//16Ϊ���պ���
	//�ú������ݴ���Ĺ�Դ�б�����������ִ�й��ռ���
	//��֧�̶ֹ���ɫ�Ѿ����ݻ�����Դ������Զ��Դ�����Դ�;۹�ƽ��к㶨��ɫ
	unsigned int r_base, g_base, b_base,		//ԭ������ɫֵ
		r_sum, g_sum, b_sum,			//ȫ����Դ���������Ч��
		shaded_color;					//������ɫ
	float dp,				//���
		dist,				//����͹�Դ֮��ľ���
		i,					//ǿ��
		nl,					//���߳���
		atten;				//˥��������
	if (!(obj->state & OBJECT4DV1_STATE_ACTIVE) || (obj->state & OBJECT4DV1_STATE_CULLED) || !(obj->state & OBJECT4DV1_STATE_VISIBLE))
	{
		return 0;
	}
	//::MessageBox(NULL, "fadfsa", "fad", MB_OK);
	//����ÿ�������
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		//��ȡ�����
		POLY4DV1_PTR curr_poly = &obj->plist[poly];
		curr_poly->color_light_trans = curr_poly->color;
		//������Ƿ���Ч
		//�����û�б��ü����޳��������ڻ״̬�����Ǳ����ҿɼ�
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue;
		//��ȡָ�����б�Ķ�������
		//����β����԰����ģ����ǻ�������Ķ����б��
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];
		//ʹ�ñ任��Ķ���ζ����б�
		//��Ϊ��������ֻ�н�����ˮ�ߵ���������׶κ��������
		//������ε���ɫģʽ
		if ((curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_FLAT) || (curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_GOURAUD))
		{
			//��565ģʽ��ɫת����888ģʽ
			r_base = curr_poly->color & 0xff;		//31
			g_base = (curr_poly->color >> 8) & 0xff;//63
			b_base = (curr_poly->color >> 16) & 0xff;
			
			r_sum = 0; g_sum = 0; b_sum = 0;
			for (int curr_light = 0; curr_light < max_lights; curr_light++)
			{
				//�жϹ�Դ�Ƿ��
				if (lights[curr_light].state == LIGHTV1_STATE_OFF)
					continue;
				//�жϹ�Դ����
				if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)
				{
					//��ÿ��������������ɫ��ˣ�������256����ȷ�����Ϊ0-255֮��
					r_sum += ((lights[curr_light].c_ambient.r * r_base) / 256);
					g_sum += ((lights[curr_light].c_ambient.g * g_base) / 256);
					b_sum += ((lights[curr_light].c_ambient.b * b_base) / 256);
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)
				{
					//����Զ��Դ
					//��Ҫ����������ķ���
					//���㰴˳ʱ�뷽�����еģ����u=p0->p1,v=p0->p2 n = u *(���) v
					VECTOR4D u, v, n;
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
					//������
					VECTOR4D_Cross(&v, &u, &n);
					float n1 = VECTOR4D_Length_Fast(&n);
					//ɢ����ļ��㹫ʽ���£�
					//���ֻ��Ҫ�����ǳ���������
					float dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);

					if (dp > 0)
					{
						i = 128 * dp / n1;
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)
				{
						//ִ�е��Դ���ռ���
						//���Դ�Ĺ���ģ������
						//			   IOpoint * Clpoint
						//I(d)point = -------------------
						//			   kc + kl*d + kq*d2
						VECTOR4D u, v, n, l;
						//����u��v
						VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
						VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
						//������
						VECTOR4D_Cross(&u, &v, &n);
						float nl = VECTOR4D_Length_Fast(&n);
						//����ӱ��浽��Դ������
						VECTOR4D_Build(&obj->vlist_trans[vindex_0], &lights[curr_light].pos, &l);
						//��������˥��
						dist = VECTOR4D_Length_Fast(&l);
						//����ɢ���
						dp = VECTOR4D_Dot(&n, &l);
						if (dp > 0)
						{
							atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
							i = 128 * dp / (nl * dist * atten);
							r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
						}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)					//�򵥾۹��(1�����;۹��)
				{
					//���㰴˳ʱ�뷽�����У����u=p0->p1,v=p0->p2,n=u*(���)v
					VECTOR4D u, v, n, l;
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
					//������
					VECTOR4D_Cross(&v, &u, &n);
					//Ϊ�Ż�������裬��Ԥ�ȼ�������ж���η��ߵĳ���
					nl = VECTOR4D_Length_Fast(&n);
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &lights[curr_light].pos, &l);
					//��������˥��
					dist = VECTOR4D_Length_Fast(&l);
					//����ɢ���
					//Itotald=Rsdiffuse*Idiffuse * (n . l);
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					//����dp>0ʱ������Ҫ���ǹ�Դ��Ӱ��
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
						i = 128 * dp / (nl * atten);
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)			//2�����ľ۹��
				{
					VECTOR4D u, v, n, d, s;
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
					VECTOR4D_Cross(&v, &u, &n);

					nl = VECTOR4D_Length_Fast(&n);
					
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);

					if (dp > 0)
					{
						VECTOR4D_Build(&lights[curr_light].pos, &obj->vlist_trans[vindex_0], &s);
						//����s�ĳ��ȣ��Թ�һ��
						dist = VECTOR4D_Length_Fast(&s);
						float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir) / dist;
						if (dpsl > 0)
						{
							atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
							float dpsl_exp = dpsl;
							//���������η�
							for (int e_index = 01; e_index < (int)lights[curr_light].pf; e_index++)
							{
								dpsl_exp *= dpsl;
							}
							//����dpls_exp�洢����(dpsl)^pf,��(s.l)^pf
							i = 128 * dp * dpsl_exp / (nl * atten);
							r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
						}//end if
					}	//end if
				}	//end if spot light
			}	//end for light
			//ȷ����ɫ���������
			if (r_sum > 255) r_sum = 255;
			if (g_sum > 255) g_sum = 255;
			if (b_sum > 255) b_sum = 255;
			//д����ɫ
			shaded_color = RGB(r_sum, g_sum, b_sum);
			curr_poly->color_light_trans = shaded_color;// (int)((shaded_color) | curr_poly->color);
		}
		else
		{
			curr_poly->color_light_trans = 0x000000;/// curr_poly->color;
			//�̶���ɫ
		}
	}

	return 1;
}
int Light_RENDERLIST4DV1_World(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam, int max_lights){
	unsigned int 
		r_base, g_base, b_base,
		r_sum, g_sum, b_sum,
		shaded_color;
	float dp,
		dist,
		i,
		nl,
		atten;
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		curr_poly->color_light_trans = curr_poly->color;

		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue;
		if ((curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_FLAT) || (curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_GOURAUD))
		{
			r_base = curr_poly->color & 0xff;
			g_base = (curr_poly->color & 0xff00) >> 8;
			b_base = (curr_poly->color & 0xff0000) >> 16;
			r_sum = 0; g_sum = 0; b_sum = 0;
			for (int curr_light = 0; curr_light < max_lights; curr_light++)
			{
				if (lights[curr_light].state == LIGHTV1_STATE_OFF)
					continue;
				if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)					//������Դ
				{
					r_sum += ((lights[curr_light].c_ambient.r * r_base) / 256);
					g_sum += ((lights[curr_light].c_ambient.g * g_base) / 256);
					b_sum += ((lights[curr_light].c_ambient.b * b_base) / 256);
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)			//����Զ����Դ
				{
					VECTOR4D u, v, n;
					//build u, v
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
					VECTOR4D_Cross(&v, &u, &n);

					nl = VECTOR4D_Length_Fast(&n);

					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					if (dp > 0)
					{
						i = 128 * dp / nl;
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)				//���Դ
				{
					VECTOR4D u, v, n, l;
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
					VECTOR4D_Cross(&u, &v, &n);

					nl = VECTOR4D_Length_Fast(&n);

					VECTOR4D_Build(&curr_poly->tvlist[0], &lights[curr_light].pos, &l);
					//��Դ������ľ���
					dist = VECTOR4D_Length_Fast(&l);

					dp = VECTOR4D_Dot(&n, &l);
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
						i = 128 * dp / (nl * dist * atten);
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}

				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)							//1�ŵ��Դ
				{
					VECTOR4D u, v, n, l;
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
					VECTOR4D_Cross(&v, &u, &n);					//��ƽ��ķ�����,��Ҫ��۹�Ƶķ����й�

					nl = VECTOR4D_Length_Fast(&n);

					//�����嵽�۹�Ƶľ���
					VECTOR4D_Build(&curr_poly->tvlist[0], &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast(&l);

					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
						i = 128 * dp / (nl * atten);
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)			//2�����ľ۹��
				{
					VECTOR4D u, v, n, d, s;
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
					VECTOR4D_Cross(&v, &u, &n);

					nl = VECTOR4D_Length_Fast(&n);

					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					
					if (dp > 0)
					{
						VECTOR4D_Build(&lights[curr_light].pos, &curr_poly->tvlist[0], &s);
						dist = VECTOR4D_Length_Fast(&s);			//���Զ��������ľ���
						//���Դ��������������Դ����������������
						float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir) / dist;
						if (dpsl > 0)
						{
							atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
							float dpsl_exp = dpsl;
							for (int e_index = 1; e_index < (int)lights[curr_light].pf; e_index++)
							{
								dpsl_exp *= dpsl;
							}
							i = 128 * dp * dpsl_exp / (nl * atten);

							r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
						}	//end if
					}	//end if
				}
			}
			if (r_sum > 255)	r_sum = 255;
			if (g_sum > 255)	g_sum = 255;
			if (b_sum > 255)	b_sum = 255;
			curr_poly->color_light_trans = RGB(r_sum, g_sum, b_sum);
		}
	}

	return 1;
}


//----------------------------------V2�汾�ĺ���--------------------------------------
void Transform_OBJECT4DV2(OBJECT4DV2_PTR obj, MATRIX4X4_PTR mt, int coord_select, int transform_basis){
	POINT4D presult;
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex].v, mt, &presult);
			VECTOR4D_COPY(&obj->vlist_local[vertex].v, &presult);
		}
		break;
	case TRANSFORM_TRANS_ONLY:
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex].v, mt, &presult);
			VECTOR4D_COPY(&obj->vlist_trans[vertex].v, &presult);
		}
		break;
	case TRANSFORM_LOCAL_TO_TRANS:
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex].v, mt, &presult);
			VECTOR4D_COPY(&obj->vlist_trans[vertex].v, &presult);
		}
		break;
	}
	if (transform_basis)
	{
		VECTOR4D vresult;
		//��תux
		Mat_Mul_VECTOR4D_4X4(&obj->ux, mt, &vresult);
		//��תuy
		Mat_Mul_VECTOR4D_4X4(&obj->uy, mt, &vresult);
		//��תuz
		Mat_Mul_VECTOR4D_4X4(&obj->uz, mt, &vresult);

		VECTOR4D_COPY(&obj->uz, &vresult);
	}
}
void Model_To_World_OBJECT4DV2(OBJECT4DV2_PTR obj, int coord_select){
	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			VECTOR4D_Add(&obj->vlist_local[vertex].v, &obj->world_pos, &obj->vlist_trans[vertex].v);
		}
	}
	else
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			VECTOR4D_Add(&obj->vlist_trans[vertex].v, &obj->world_pos, &obj->vlist_trans[vertex].v);
		}
	}
}
void World_To_Camera_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam){
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
	//	obj->vlist_trans[vertex].v.x -= cam->pos.x;
	//	obj->vlist_trans[vertex].v.y -= cam->pos.y;
	//	obj->vlist_trans[vertex].v.z -= cam->pos.z;
		VECTOR4D presult;
		Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex].v, &cam->mcam, &presult);
		VECTOR4D_COPY(&obj->vlist_trans[vertex].v, &presult);
	}
}
int Cull_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam, int cull_flags){
	POINT4D sphere_pos;
	Mat_Mul_VECTOR4D_4X4(&obj->world_pos, &cam->mcam, &sphere_pos);			//ִ������任,�������������ת�����������ϵ
	//��2���������޳���Ƕ�����ִ���޳�����
	if (cull_flags & CULL_OBJECT_Z_PLANE)
	{
		//ֻ����Զ���ü������޳�����
		//ʹ��Զ���ü�����в���
		if (((sphere_pos.z + obj->max_radius[0]) > cam->far_clip_z) || ((sphere_pos.z - obj->max_radius[0]) < cam->near_clip_z))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}
	if (cull_flags & CULL_OBJECT_X_PLANE)
	{
		//ֻ�������Ҳü�����������޳�
		float z_test = (0.5f) * cam->viewplane_width * sphere_pos.z / cam->view_dist;
		if (((sphere_pos.x - obj->max_radius[0]) > z_test)				//�ұ�
			|| ((sphere_pos.x + obj->max_radius[0]) < -z_test))		//���
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}
	if (cull_flags & CULL_OBJECT_Y_PLANE)
	{
		//ֻ�������²ü������������޳�
		float z_test = (0.5f) * cam->viewplane_height * sphere_pos.z / cam->view_dist;
		if ((sphere_pos.y - obj->max_radius[0]) > z_test || ((sphere_pos.y + obj->max_radius[0]) < -z_test))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}
	return 0; 
}
void Reset_OBJECT4DV2(OBJECT4DV2_PTR obj){
	RESET_BIT(obj->state, OBJECT4DV2_STATE_CULLED);		//�������屻�޳�״̬
	//���������ÿ�����״̬
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR curr_poly = &obj->plist[poly];
		//�ж϶�����Ƿ�ɼ�
		if (!(curr_poly->state & POLY4DV2_STATE_ACTIVE))
		{
			continue;
		}
		RESET_BIT(curr_poly->state, POLY4DV2_STATE_CLIPPED);
		RESET_BIT(curr_poly->state, POLY4DV2_STATE_BACKFACE);
	}
}
void Remove_Backfaces_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam){
	if (obj->state & OBJECT4DV2_STATE_CULLED)
		return;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR curr_poly = &obj->plist[obj->curr_frame * obj->num_polys + poly];
		//��ǰ�治��Ҫ�Ƴ�
		if (!(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->attr & POLY4DV2_ATTR_2SIDED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			continue;
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];
		VECTOR4D u, v, n;
		VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_1].v, &u);
		VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_2].v, &v);
		VECTOR4D_Cross(&u, &v, &n);			//��������

		VECTOR4D view;
		VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &cam->pos, &view);
		float dp = VECTOR4D_Dot(&n, &view);
		if (dp <= 0.0)
		{
			SET_BIT(curr_poly->state, POLY4DV2_STATE_BACKFACE);
		}
	}
}
void Camera_To_Perspective_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam){
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		//cam->view_dist���������ͶӰƽ��ľ���
		float z = obj->vlist_trans[vertex].z;
		obj->vlist_trans[vertex].x = cam->view_dist * obj->vlist_trans[vertex].x / z;
		obj->vlist_trans[vertex].y = cam->view_dist * obj->vlist_trans[vertex].y * cam->aspect_ratio / z;
	}
}
void Perspective_To_Screen_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam){					//ת������Ļ����
	float alpha = (0.5 * cam->viewport_width - 0.5);				//x�����ĵ�
	float beta = (0.5 * cam->viewport_height - 0.5);				//y�����ĵ�
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		obj->vlist_trans[vertex].x = alpha + alpha * obj->vlist_trans[vertex].x;
		obj->vlist_trans[vertex].y = beta - beta * obj->vlist_trans[vertex].y;
	}
}
void Camera_To_Perspective_Screen_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam){
	float alpha = (0.5 * cam->viewport_width - 0.5);
	float beta = (0.5 * cam->viewport_height - 0.5);
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		float z = obj->vlist_trans[vertex].z;
		//aspect_ratio = width  / height
		obj->vlist_trans[vertex].x = alpha + alpha * (cam->view_dist * obj->vlist_trans[vertex].x / z);
		obj->vlist_trans[vertex].y = beta - beta * (cam->view_dist * obj->vlist_trans[vertex].y * cam->aspect_ratio / z);
	}
}

int Light_OBJECT4DV2_World16(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam, int max_lights){
	unsigned int
		r_base, g_base, b_base,					// base color being lit
		r_sum, g_sum, b_sum,					//sum of ligthing process all lights
		r_sum0, g_sum0, b_sum0,
		r_sum1, g_sum1, b_sum1,
		r_sum2, g_sum2, b_sum2,
		ri, gi, bi,
		shaded_color;							//final color
	float dp, dist, dists, i, nl, atten;
	VECTOR4D u, v, n, l, d, s;
	//���岻�ɼ���δ����
	if (!(obj->state & OBJECT4DV2_STATE_ACTIVE) || (obj->state & OBJECT4DV2_STATE_CULLED) || !(obj->state & OBJECT4DV2_STATE_VISIBLE))
		return 0;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR curr_poly = &obj->plist[poly];						//��ȡ��ǰ����������
		//�жϵ�ǰ������Ƿ�ɼ���Ҳ�����ܹ���ʾ
		if (!(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			;// continue;

		//--------------------------
		SET_BIT(curr_poly->state, POLY4DV2_STATE_LIT);
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];
		//FLAT ��ɫ
		if (curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_FLAT)
		{
			r_base = curr_poly->color & 0xff;
			g_base = (curr_poly->color >> 8) & 0xff;
			b_base = (curr_poly->color >> 16) & 0xff;
			r_sum = 0;
			g_sum = 0;
			b_sum = 0;
			n.z = FLT_MAX;
			
			//ѭ��ÿһ����Դ��ִ�й��ռ���
			for (int curr_light = 0; curr_light < max_lights; curr_light++)
			{
				//��ǰ�����Դ��״̬�ǹرյ�
				if (lights[curr_light].state == LIGHTV1_STATE_OFF)
				{
					continue;
				}
				//MessageBox(NULL, "????", "????", MB_OK);
				if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)						//������Դ
				{
					r_sum += (lights[curr_light].c_ambient.r * r_base) / 256;
					g_sum += (lights[curr_light].c_ambient.g * g_base) / 256;
					b_sum += (lights[curr_light].c_ambient.b * b_base) / 256;
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)				//����Զ��Դ
				{
					if (n.z == FLT_MAX)
					{
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_1].v, &u);
						VECTOR4D_Build(&obj->vlist_trans[vindex_1].v, &obj->vlist_trans[vindex_2].v, &v);
						//����������     
						VECTOR4D_Cross(&u, &v, &n);
					}
					nl = curr_poly->nlength;
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					if (dp > 0)
					{
						i = 128 * dp / nl;
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i / (256 * 128));
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i / (256 * 128));
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i / (256 * 128));
					}

				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)
				{
					if (n.z == FLT_MAX)
					{
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_1].v, &u);
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_2].v, &v);
						VECTOR4D_Cross(&u, &v, &n);			//���
					}
					nl = curr_poly->nlength;
					VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast(&l);
					//������뷨�����ĵ��
					dp = VECTOR4D_Dot(&n, &l);
					//���ڵĵ��Դ�ܹ��յ���ƽ��
					if (dp > 0)
					{
						//kc
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq*dist * dist);
						i = 128 * dp / (nl * dist * atten);
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 218);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)				//1�����ľ۹��(�򵥾۹��)
				{
					//��������ʽ,��⵱ǰ���ƽ��ķ�����������ֱ�ڵ�ǰƽ���ֱ�ߣ�
					if (n.z == FLT_MAX)
					{
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_1].v, &u);
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_2].v, &v);
						VECTOR4D_Cross(&u, &v, &n);			//���
					}
					nl = curr_poly->nlength;

					//������嵽��Դ��λ��
					VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast(&l);
					//����۹�Ƶķ�������ķ������ĵ��
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					//�������0���������Դ�պ��ܹ��յ�����
					if (dp > 0)
					{
						atten = (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						i = 128 * dp / (nl * dist * atten);
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)				//2�����ľ۹��(���Ӿ۹��)
				{
					//��������ʽ����ͬ��
					if (n.z == FLT_MAX)
					{
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_1].v, &u);
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_2].v, &v);
						VECTOR4D_Cross(&u, &v, &n);			//���
					}
					nl = curr_poly->nlength;
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);	
					if (dp > 0)
					{
						VECTOR4D_Build(&lights[curr_light].pos, &obj->vlist_trans[vindex_0].v, &s);
						dists = VECTOR4D_Length_Fast(&s);
						//����Խ�ھ۹�Ƶ���Χ��dpsl��ֵԽС
						float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir) / dists;
						//��ǰ��û�г����۹�ƵĿ�������Χ
						if (dpsl > 0)
						{
							atten = (lights[curr_light].kc + lights[curr_light].kl * dists + lights[curr_light].kq * dists * dists);
							float dpsl_exp = dpsl;
 							for (int e_index = 1; e_index < (int)lights[curr_light].pf; e_index++)
							{
								dpsl_exp *= dpsl;
							}
							i = 128 * dp * dpsl_exp / (nl * atten);
							r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
						}	//end if
					}	//end if
				}	//end for light
			}

			if (r_sum > 255) r_sum = 255;
			if (g_sum > 255) g_sum = 255;
			if (b_sum > 255) b_sum = 255;
			curr_poly->light_color = RGB(r_sum, g_sum, g_sum);


			TCHAR text[128];
			sprintf_s(text, "red:%u green:%u blue:%u raw:%x\0", r_sum, g_sum, b_sum, curr_poly->color);
			//MessageBox(NULL, text, "title", MB_OK);
		}
		else if (curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_GOURAUD)				//��һ����ɫģʽ
		{
			//MessageBox(NULL, "tiando", )
			r_base = curr_poly->color & 0xff;
			g_base = (curr_poly->color >> 8) & 0xff;
			b_base = (curr_poly->color >> 16) & 0xff;
			
			r_sum0 = 0; g_sum0 = 0; b_sum0 = 0;
			r_sum1 = 0; g_sum1 = 0; b_sum1 = 0;
			r_sum2 = 0; g_sum2 = 0; b_sum2 = 0;
			for (int curr_light = 0; curr_light < max_lights; curr_light++)
			{
				if (lights[curr_light].state == LIGHTV1_STATE_OFF)
					continue;
				if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)					//�����⣬ÿ����Ĺ�Դֵ����һ����
				{
					ri = ((lights[curr_light].c_ambient.r * r_base) / 255);
					gi = ((lights[curr_light].c_ambient.g * g_base) / 255);
					bi = ((lights[curr_light].c_ambient.b * b_base) / 255);
					r_sum0 += ri; g_sum0 += gi; b_sum0 += bi;
					r_sum1 += ri; g_sum1 += gi; b_sum1 += bi;
					r_sum2 += ri; g_sum2 += gi; b_sum2 += bi;
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)								//����Զ����Դ
				{
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_0].n, &lights[curr_light].dir);			//���Դ����������������������ĵ��
					if (dp > 0)
					{
						i = 128 * dp;
						r_sum0 += (lights[curr_light].c_diffuse.r*r_base*i / (256 * 128));
						g_sum0 += (lights[curr_light].c_diffuse.g*g_base*i / (256 * 128)); 
						b_sum0 += (lights[curr_light].c_diffuse.b*b_base*i / (256 * 128));
					}
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_1].n, &lights[curr_light].dir);
					if (dp > 0)
					{
						i = 128 * dp;
						r_sum1 += (lights[curr_light].c_diffuse.r*r_base*i / (256 * 128));
						g_sum1 += (lights[curr_light].c_diffuse.g*g_base*i / (256 * 128));
						b_sum1 += (lights[curr_light].c_diffuse.b*b_base*i / (256 * 128));
					}
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_2].n, &lights[curr_light].dir);
					if (dp > 0)
					{
						i = 128 * dp;
						r_sum2 += (lights[curr_light].c_diffuse.r*r_base*i / (256 * 128));
						g_sum2 += (lights[curr_light].c_diffuse.g*g_base*i / (256 * 128));
						g_sum2 += (lights[curr_light].c_diffuse.b*b_base*i / (256 * 128));
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)								//���Դ
				{
					//��0����Դ������
					VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast(&l);
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_0].n, &l);
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq *dist * dist);
						i = 128 * dp / (dist * atten);
						r_sum0 += (lights[curr_light].c_diffuse.r*r_base*i) / (256 * 128);
						g_sum0 += (lights[curr_light].c_diffuse.g*g_base*i) / (256 * 128);
						b_sum0 += (lights[curr_light].c_diffuse.b*b_base*i) / (256 * 128);
					}
					//��1����Դ������
					VECTOR4D_Build(&obj->vlist_trans[vindex_1].v, &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast(&l);
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_1].n, &l);
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq *dist * dist);
						i = 128 * dp / (dist * atten);
						r_sum1 += (lights[curr_light].c_diffuse.r*r_base*i) / (256 * 128);
						g_sum1 += (lights[curr_light].c_diffuse.g*g_base*i) / (256 * 128);
						b_sum1 += (lights[curr_light].c_diffuse.b*b_base*i) / (256 * 128);
					}
					//��2����Դ������
					VECTOR4D_Build(&obj->vlist_trans[vindex_2].v, &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast(&l);
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_2].n, &l);
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq *dist * dist);
						i = 128 * dp / (dist * atten);
						r_sum2 += (lights[curr_light].c_diffuse.r*r_base*i) / (256 * 128);
						g_sum2 += (lights[curr_light].c_diffuse.g*g_base*i) / (256 * 128);
						b_sum2 += (lights[curr_light].c_diffuse.b*b_base*i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)
				{
					//���㵱ǰ�����Ĵ˶������Դ��λ��
					VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast(&l);
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_0].n, &lights[curr_light].dir);
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);

						i = 128 * dp / (atten);
						r_sum0 += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum0 += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum0 += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_1].n, &lights[curr_light].dir);
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
						i = 128 * dp / (atten);
						r_sum1 = (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum1 = (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum1 = (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_2].n, &lights[curr_light].dir);
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
						i = 128 * dp / (atten);
						r_sum2 = (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum2 = (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum2 = (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)					//2�Ÿ��Ӿ۹��
				{
					//�жϵ�ǰ�����ķ������Ƿ��ھ۹����
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_0].n, &lights[curr_light].dir);
					if (dp > 0)
					{
						//�жϵ�ǰ��������Ƿ��ھ۹����
						VECTOR4D_Build(&lights[curr_light].pos, &obj->vlist_trans[vindex_0].n, &s);
						dists = VECTOR4D_Length_Fast(&s);
						float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir) / dists;
						if (dpsl > 0)
						{
							atten = (lights[curr_light].kc + lights[curr_light].kl * dists + lights[curr_light].kq * dists * dists);
							float dpsl_exp = dpsl;
							for (int e_index = 1; e_index < (int)lights[curr_light].pf; e_index++)
								dpsl_exp *= dpsl;
							i = 128 * dp * dpsl_exp / (atten);
							
							r_sum0 += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum0 += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum0 += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
						}
					}
					//vertex  1;
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_1].n, &lights[curr_light].dir);
					if (dp > 0)
					{
						VECTOR4D_Build(&lights[curr_light].pos, &obj->vlist_trans[vindex_1].v, &s);
						dists = VECTOR4D_Length_Fast(&s);
						float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir) / dists;
						if (dpsl > 0)
						{
							atten = (lights[curr_light].kc + lights[curr_light].kl * dists + lights[curr_light].kq * dists * dists);
							float dpsl_exp = dpsl;
							for (int e_index = 1; e_index < (int)lights[curr_light].pf; e_index++)
								dpsl_exp *= dpsl;
							i = 128 * dp * dpsl_exp / (atten);
							r_sum1 += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum1 += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum1 += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
						}
					}
					//vertex 2:
					//�жϵ�ķ��������Դ��λ��
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_2].n, &lights[curr_light].dir);
					if (dp > 0)
					{
						VECTOR4D_Build(&lights[curr_light].pos, &obj->vlist_trans[vindex_2].v, &s);
						dists = VECTOR4D_Length_Fast(&s);
						float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir) / dists;
						if (dpsl > 0)
						{
							atten = (lights[curr_light].kc + lights[curr_light].kl * dists + lights[curr_light].kq * dists * dists);
							float dpsl_exp = dpsl;
							for (int e_index = 1; e_index < (int)lights[curr_light].pf; e_index++)
								dpsl_exp *= dpsl;
							i = 128 * dp * dpsl_exp / (atten);
							r_sum2 += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum2 += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum2 += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
						}	//end if
					}	//end if

					curr_poly->lit_color[2] = RGB(r_sum2, g_sum2, b_sum2);
				}
			}	//end light for
			//vertex 0
			if (r_sum0 > 255) r_sum0 = 255;
			if (g_sum0 > 255) g_sum0 = 255;
			if (b_sum0 > 255) b_sum0 = 255;
			//vertex 1
			if (r_sum1 > 255) r_sum1 = 255;
			if (g_sum1 > 255) g_sum1 = 255;
			if (b_sum1 > 255) b_sum1 = 255;
			//vertex 2
			if (r_sum2 > 255) r_sum2 = 255;
			if (g_sum2 > 255) g_sum2 = 255;
			if (b_sum2 > 255) b_sum2 = 255;
			//���õ�ǰ���������������ɫ��GROUAUD����)
			curr_poly->lit_color[0] = RGB(r_sum0, g_sum0, b_sum0);
			curr_poly->lit_color[1] = RGB(r_sum1, g_sum1, b_sum1);
			curr_poly->lit_color[2] = RGB(r_sum2, g_sum2, b_sum2);
		}
		else
		{
			curr_poly->lit_color[0] = curr_poly->lit_color[1] = curr_poly->lit_color[2] = curr_poly->color;
		}

	}
	return 1;
}

//��Ⱦ����
void Draw_OBJECT4DV2_Wire16(OBJECT4DV2_PTR obj, HDC hdc, int lpitch){
	if (obj->state & OBJECT4DV2_STATE_CULLED)
		return;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		if (!(obj->plist[poly].state & POLY4DV2_STATE_ACTIVE) || (obj->plist[poly].state & POLY4DV2_STATE_CLIPPED) || (obj->plist[poly].state & POLY4DV2_STATE_BACKFACE))
		{
			continue;
		}
		int vindex_0 = obj->plist[poly].vert[0];
		int vindex_1 = obj->plist[poly].vert[1];
		int vindex_2 = obj->plist[poly].vert[2];
		//��ͨ��ɫģʽ(PHONG) //ping
		if (obj->plist[poly].attr & POLY4DV2_ATTR_SHADE_MODE_FLAT)
		{
			HPEN hPen = CreatePen(PS_SOLID, 1, obj->plist[poly].light_color);
			HPEN OldHPen = (HPEN)SelectObject(hdc, hPen);
			DeleteObject(OldHPen);
			MoveToEx(hdc, (int)obj->vlist_trans[vindex_0].x, (int)obj->vlist_trans[vindex_0].y, NULL);
			LineTo(hdc, (int)obj->vlist_trans[vindex_1].x, (int)obj->vlist_trans[vindex_1].y);
			LineTo(hdc, (int)obj->vlist_trans[vindex_2].x, (int)obj->vlist_trans[vindex_2].y);
			LineTo(hdc, (int)obj->vlist_trans[vindex_0].x, (int)obj->vlist_trans[vindex_0].y);
		}
		else
		{

			HPEN hPen = CreatePen(PS_SOLID, 1, obj->plist[poly].lit_color[0]);
			HPEN OldHPen = (HPEN)SelectObject(hdc, hPen);
			DeleteObject(OldHPen);
			MoveToEx(hdc, (int)obj->vlist_trans[vindex_0].x, (int)obj->vlist_trans[vindex_0].y, NULL);
			LineTo(hdc, (int)obj->vlist_trans[vindex_1].x, (int)obj->vlist_trans[vindex_1].y);
			LineTo(hdc, (int)obj->vlist_trans[vindex_2].x, (int)obj->vlist_trans[vindex_2].y);
			LineTo(hdc, (int)obj->vlist_trans[vindex_0].x, (int)obj->vlist_trans[vindex_0].y);
		}
	}
}
void Draw_OBJECT4DV12_Solid16(OBJECT4DV2_PTR obj, HDC hdc, int lpitch){
	if (obj->state & OBJECT4DV2_STATE_CULLED)
		return;
	int count = 0;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		if (!(obj->plist[poly].state & POLY4DV2_STATE_ACTIVE) || (obj->plist[poly].state & POLY4DV2_STATE_CLIPPED) || (obj->plist[poly].state & POLY4DV2_STATE_BACKFACE))
		{
			continue;
		}
		int vindex_0 = obj->plist[poly].vert[0];
		int vindex_1 = obj->plist[poly].vert[1];
		int vindex_2 = obj->plist[poly].vert[2];
		//��ͨ��ɫģʽ
		if (obj->plist[poly].attr & POLY4DV2_ATTR_SHADE_MODE_FLAT)
		{
			Draw_Triangle_2D(
				obj->vlist_trans[vindex_0].x, obj->vlist_trans[vindex_0].y,
				obj->vlist_trans[vindex_1].x, obj->vlist_trans[vindex_1].y,
				obj->vlist_trans[vindex_2].x, obj->vlist_trans[vindex_2].y,
				obj->plist[poly].color,
				hdc,
				lpitch
				);
			Draw_Gouraud_Trangle(
				obj->vlist_trans[vindex_0].x, obj->vlist_trans[vindex_0].y, (float)0xffff00,
				obj->vlist_trans[vindex_1].x, obj->vlist_trans[vindex_1].y, (float)0xffff00,
				obj->vlist_trans[vindex_2].x, obj->vlist_trans[vindex_2].y, (float)0x00ffff,
				hdc, 1
				);


			HPEN hPen = CreatePen(PS_SOLID, 1, 0xaaaaaa);
			HPEN OldHPen = (HPEN)SelectObject(hdc, hPen);
			DeleteObject(OldHPen);
			MoveToEx(hdc, (int)obj->vlist_trans[vindex_0].x, (int)obj->vlist_trans[vindex_0].y, NULL);
			LineTo(hdc, (int)obj->vlist_trans[vindex_1].x, (int)obj->vlist_trans[vindex_1].y);
			LineTo(hdc, (int)obj->vlist_trans[vindex_2].x, (int)obj->vlist_trans[vindex_2].y);
			LineTo(hdc, (int)obj->vlist_trans[vindex_0].x, (int)obj->vlist_trans[vindex_0].y);
			count++;
		}
		else if (obj->plist[poly].attr & POLY4DV2_ATTR_SHADE_MODE_GOURAUD)
		{
		//	Draw_Gouraud_Trangle(
		//		obj->vlist_trans[vindex_0].x, obj->vlist_trans[vindex_0].y, (float)obj->plist[poly].lit_color[0],
		//		obj->vlist_trans[vindex_1].x, obj->vlist_trans[vindex_1].y, (float)obj->plist[poly].lit_color[1],
		//		obj->vlist_trans[vindex_2].x, obj->vlist_trans[vindex_2].y, (float)obj->plist[poly].lit_color[2],
		//		hdc, 1
		//		);
		Draw_Gouraud_Trangle(
					obj->vlist_trans[vindex_0].x, obj->vlist_trans[vindex_0].y, (float)0xff0000,
					obj->vlist_trans[vindex_1].x, obj->vlist_trans[vindex_1].y, (float)0x00ff00,
					obj->vlist_trans[vindex_2].x, obj->vlist_trans[vindex_2].y, (float)0x0000ff,
					hdc, 1
					);
		}
	}

}

//////////////////////��Ⱦ�б�Ĳ���/////////////////////
void Reset_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list){
	rend_list->num_polys = 0;
}
void Transform_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, MATRIX4X4_PTR mt, int coord_select){
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
			if (curr_poly == NULL || !(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
				continue;
			for (int vertex = 0; vertex < 3; vertex++)
			{
				POINT4D presult;
				Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex].v, mt, &presult);
				VECTOR4D_COPY(&curr_poly->vlist[vertex].v, &presult);
			}
		}
		break;
	case TRANSFORM_TRANS_ONLY:
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
			if (curr_poly == NULL || !(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
				continue;
			for (int vertex = 0; vertex < 3; vertex++)
			{
				POINT4D presult;
				Mat_Mul_VECTOR4D_4X4(&curr_poly->tvlist[vertex].v, mt, &presult);
				VECTOR4D_COPY(&curr_poly->tvlist[vertex].v, &presult);
			}
		}
		break;
	case TRANSFORM_LOCAL_TO_TRANS:
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
			if (curr_poly == NULL || !(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
				continue;
			for (int vertex = 0; vertex < 3; vertex++)
			{
				Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex].v, mt, &curr_poly->tvlist[vertex].v);
			}
		}
		break;
	default:
		break;
	}
}

void Remove_Backfaces_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
		if (curr_poly == NULL || !(curr_poly->state&POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_ATTR_2SIDED) ||(curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			continue;
		VECTOR4D u, v, n;
		VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[1].v, &u);
		VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[2].v, &v);
		
		VECTOR4D_Cross(&u, &v, &n);
		VECTOR4D view;
		VECTOR4D_Build(&curr_poly->tvlist[0].v, &cam->pos, &view);
		float dp = VECTOR4D_Dot(&n, &view);
		if (dp <= 0.0)
		{
			SET_BIT(curr_poly->state, POLY4DV2_STATE_BACKFACE);
		}

	}
}
void World_To_Camera_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
		if (curr_poly == NULL || !(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			continue;
		for (int vertex = 0; vertex < 3; vertex++)
		{
			POINT4D presult;
			Mat_Mul_VECTOR4D_4X4(&curr_poly->tvlist[vertex].v, &cam->mcam, &presult);
			VECTOR4D_COPY(&curr_poly->tvlist[vertex].v, &presult);
		}
	}
}
void Camera_To_Perspective_RENDERLSIT4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
		if (curr_poly == NULL || !(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			continue;
		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;
			curr_poly->tvlist[vertex].x = cam->view_dist*curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = cam->view_dist*curr_poly->tvlist[vertex].y*cam->aspect_ratio / z;
		}
	}

}
void Perspective_To_Screen_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			continue;
		float alpha = (0.5 * cam->viewport_width + 0.5);
		float beta = (0.5 * cam->viewport_height + 0.5);
		for (int vertex = 0; vertex < 3; vertex++)
		{
			curr_poly->tvlist[vertex].x = alpha + curr_poly->tvlist[vertex].x;
			curr_poly->tvlist[vertex].y = beta - curr_poly->tvlist[vertex].y;
		}
	}
}
void Camera_To_Perspective_Screen_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			continue;
		float alpha = (0.5 * cam->viewport_width - 0.5);
		float beta = (0.5 * cam->viewport_height - 0.5);
		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;
			curr_poly->tvlist[vertex].x = cam->view_dist * curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = cam->view_dist * curr_poly->tvlist[vertex].y / z;

			curr_poly->tvlist[vertex].x = curr_poly->tvlist[vertex].x + alpha;
			curr_poly->tvlist[vertex].y = -curr_poly->tvlist[vertex].y + beta;
		}
	}
}
int Insert_POLY4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, POLY4DV2_PTR poly){						//��POLY4DV2���뵽RENDLIST
	//��Ⱦ�б����������޷��ٲ���������
	if (rend_list->num_polys >= RENDERLIST4DV2_MAX_POLYS)
		return 0;
	rend_list->poly_ptrs[rend_list->num_polys] = &rend_list->poly_data[rend_list->num_polys];
	
	rend_list->poly_data[rend_list->num_polys].state = poly->state;
	rend_list->poly_data[rend_list->num_polys].attr = poly->attr;
	rend_list->poly_data[rend_list->num_polys].color = poly->color;
	rend_list->poly_data[rend_list->num_polys].nlength = poly->nlength;
	rend_list->poly_data[rend_list->num_polys].texture = poly->texture;

	rend_list->poly_data[rend_list->num_polys].lit_color[0] = poly->lit_color[0];
	rend_list->poly_data[rend_list->num_polys].lit_color[1] = poly->lit_color[1];
	rend_list->poly_data[rend_list->num_polys].lit_color[2] = poly->lit_color[2];

	VERTEX4DTV1_COPY(&rend_list->poly_data[rend_list->num_polys].tvlist[0], &poly->vlist[poly->vert[0]]);
	VERTEX4DTV1_COPY(&rend_list->poly_data[rend_list->num_polys].tvlist[1], &poly->vlist[poly->vert[1]]);
	VERTEX4DTV1_COPY(&rend_list->poly_data[rend_list->num_polys].tvlist[2], &poly->vlist[poly->vert[2]]);
	
	VERTEX4DTV1_COPY(&rend_list->poly_data[rend_list->num_polys].vlist[0], &poly->vlist[poly->vert[0]]);
	VERTEX4DTV1_COPY(&rend_list->poly_data[rend_list->num_polys].vlist[1], &poly->vlist[poly->vert[1]]);
	VERTEX4DTV1_COPY(&rend_list->poly_data[rend_list->num_polys].vlist[2], &poly->vlist[poly->vert[2]]);

	//����
	rend_list->poly_data[rend_list->num_polys].tvlist[0].t = poly->tlist[poly->text[0]];
	rend_list->poly_data[rend_list->num_polys].tvlist[1].t = poly->tlist[poly->text[1]];
	rend_list->poly_data[rend_list->num_polys].tvlist[2].t = poly->tlist[poly->text[2]];

	rend_list->poly_data[rend_list->num_polys].vlist[0].t = poly->tlist[poly->text[0]];
	rend_list->poly_data[rend_list->num_polys].vlist[1].t = poly->tlist[poly->text[1]];
	rend_list->poly_data[rend_list->num_polys].vlist[2].t = poly->tlist[poly->text[2]];

}
int Insert_POLYF4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, POLYF4DV2_PTR poly){						//POLYF4DV2���뵽RENDLIST
	if (poly == NULL || rend_list->num_polys >= RENDERLIST4DV2_MAX_POLYS)
		return 0;

	rend_list->poly_ptrs[rend_list->num_polys] = &rend_list->poly_data[rend_list->num_polys];
	
	memcpy((void*)&rend_list->poly_data[rend_list->num_polys], &poly, sizeof(POLYF4DV2));

	if (rend_list->num_polys == 0)
	{
		rend_list->poly_data[0].next = NULL;
		rend_list->poly_data[0].prev = NULL;
	}
	else
	{
		rend_list->poly_data[rend_list->num_polys].next = NULL;
		rend_list->poly_data[rend_list->num_polys].prev = &rend_list->poly_data[rend_list->num_polys - 1];
		rend_list->poly_data[rend_list->num_polys - 1].next = &rend_list->poly_data[rend_list->num_polys];
	}
	rend_list->num_polys++;
	return 1;
}
int Insert_OBJECT4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, OBJECT4DV2_PTR obj, int insert_local){						//��OBJECT4DV2CT���뵽RENDLIST
	unsigned int base_color;
	if (!(obj->state & OBJECT4DV2_STATE_ACTIVE) || (obj->state & OBJECT4DV2_STATE_CULLED) || !(obj->state & OBJECT4DV2_STATE_VISIBLE))
		return 0;
	//���������ÿһ���涼���뵽��Ⱦ�б���
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR curr_poly = &obj->plist[poly];
		if (!(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			continue;
		//���浱ǰ�ߵ�����ָ��
		VERTEX4DTV1_PTR vlist_old = curr_poly->vlist;
		//�����û���ѡ�񽫵�ǰ�ߵ�����ָ��ָ���µ���Ϣ�ڴ���
		if (insert_local)
		{
			curr_poly->vlist = obj->vlist_local;
		}
		else
		{
			curr_poly->vlist = obj->vlist_trans;
		}
		if (!Insert_POLY4DV2_RENDERLIST4DV2(rend_list, curr_poly))
		{
			curr_poly->vlist = vlist_old;
			return 0;
		}
		//�ָ���ǰ�ߵ��ϵ�����ָ��
		curr_poly->vlist = vlist_old;
	}
	return 1;

}
int Light_RENDERLIST4DV2_World16(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam, int max_ligths){
	unsigned int r_base, g_base, b_base,
		r_sum, g_sum, b_sum,
		r_sum0, g_sum0, b_sum0,
		r_sum1, g_sum1, b_sum1,
		r_sum2, g_sum2, b_sum2,
		ri, gi, bi,
		shaded_color;
	float dp, dist, dists, i, nl, atten;
	VECTOR4D u, v, n, l, d, s;
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
		if (!(curr_poly->state & POLY4DV2_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV2_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV2_STATE_BACKFACE) ||
			(curr_poly->state & POLY4DV2_STATE_LIT)
			)
		{
			continue;
		}
		SET_BIT(curr_poly->state, POLY4DV2_STATE_LIT);
		//FLAT����ɫģʽ
		if (curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_FLAT)
		{
			//ȡ��ԭʼ�����ɫ
			//����ԭʼ��ɫ���й��ռ���
			r_base = curr_poly->color & 0xff;
			g_base = curr_poly->color >> 8 & 0xff;
			b_base = curr_poly->color >>16 & 0xff;
			r_sum = 0; g_sum0; b_sum = 0;
			n.z = FLT_MAX;
			for (int curr_light = 0; curr_light < max_ligths; curr_light++)
			{
				if (lights[curr_light].state == LIGHTV1_STATE_OFF)
					continue;
				//������
				if (lights[curr_light].attr == LIGHTV1_ATTR_AMBIENT)
				{
					r_sum += r_base * lights[curr_light].c_ambient.r / 256;
					g_sum += g_base * lights[curr_light].c_ambient.g / 256;
					b_sum += b_base * lights[curr_light].c_ambient.b / 256;
				} //end if LIGHTV1_ATTR_AMBIENT
				else if (lights[curr_light].attr == LIGHTV1_ATTR_INFINITE)
				{
					if (n.z == FLT_MAX)
					{
						//��ǰ��ķ�����
						VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[1].v, &u);
						VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[2].v, &v);

						VECTOR4D_Cross(&u, &v, &n);
					}
					nl = curr_poly->nlength;
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					if (dp > 0)
					{
						i = 128 * dp / nl;				//��󲻹�128....��������ô��Ϊ���ѵ��������������壿������
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				} //end if LIGHTV1_ATTR_INFINITE
				else if (lights[curr_light].attr == LIGHTV1_ATTR_POINT)
				{
					//���֮ǰ��ǰ��ķ������Ѿ�����ˣ�������Ҫ����
					if (n.z == FLT_MAX)
					{
						//��ǰ��ķ�����
						VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[1].v, &u);
						VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[2].v, &v);
						VECTOR4D_Cross(&u, &v, &n);
					}
					nl = curr_poly->nlength;
					VECTOR4D_Build(&curr_poly->tvlist[0].v, &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast2(&l);
					dp = VECTOR4D_Dot(&n, &l);
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
						i = 128 * dp / (nl * dist * atten);
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}	//end if dp>0
				}	//end if LIGHTV1_ATTR_POINT
				else if (lights[curr_light].attr == LIGHTV1_ATTR_SPOTLIGHT1)
				{
					//���֮ǰ��ǰ��ķ������Ѿ�����ˣ�������Ҫ����
					if (n.z == FLT_MAX)
					{
						//��ǰ��ķ�����
						VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[1].v, &u);
						VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[2].v, &v);
						VECTOR4D_Cross(&u, &v, &n);
					}
					nl = curr_poly->nlength;
					VECTOR4D_Build(&curr_poly->tvlist[0].v, &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast2(&l);
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
						i = 128 * dp / (nl * atten);
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i);

					}
				}
				else if (lights[curr_light].attr == LIGHTV1_ATTR_SPOTLIGHT2)
				{
					//���֮ǰ��ǰ��ķ������Ѿ�����ˣ�������Ҫ����
					if (n.z == FLT_MAX)
					{
						//��ǰ��ķ�����
						VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[1].v, &u);
						VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[2].v, &v);
						VECTOR4D_Cross(&u, &v, &n);
					}
					nl = curr_poly->nlength;
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					if (dp > 0)
					{
						VECTOR4D_Build(&lights[curr_light].pos, &curr_poly->tvlist[0].v, &s);
						dists = VECTOR4D_Length_Fast2(&s);
						float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].pos);
						if (dpsl > 0)
						{
							atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
							float dpsl_exp = dpsl;
							for (int e_index = 1; e_index < (int)lights[curr_light].pf; e_index++)
							{
								dpsl_exp *= dpsl;
							}
							i = 128 * dp * dpsl_exp / (nl * atten);
							r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
						}	//end if dpsl > 0
					}	//end dp > 0
				}	//end if LIGHTV1_ATTR_SPOTLIGHT2
			}	//end for curr_light
		}	//end if light is FLAT
		else if (curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_GOURAUD)
		{
			//GROUND��ɫģʽ
			//����֮��ʵ��
		}
	}
	return 1;
}
void Draw_RENDERLIST4DV2_Wire16(RENDERLIST4DV2_PTR rend_list, HDC hdc, int lpitch);
void Draw_RENDERLIST4DV2_Solid16(RENDERLIST4DV2_PTR rend_list, HDC hdc, int lpitch);
void Clip_Polys_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam, int clip_flags){
	//�ڲ��ü���
#define CLIP_CODE_GZ 0x0001
#define CLIP_CODE_LZ 0x0002
#define CLIP_CODE_IZ 0x0004

#define CLIP_CODE_GX 0x0001
#define CLIP_CODE_LX 0x0002
#define CLIP_CODE_IX 0x0004

#define CLIP_CODE_GY 0x0001
#define CLIP_CODE_LY 0x0002
#define CLIP_CODE_IY 0x0004

#define CLIP_CODE_NULL 0x0000
	int vertex_ccodes[3];			//���ڴ洢�ü����
	int num_verts_in;
	int v0, v1, v2;
	float z_factor, z_test;
	float xi, yi, x01i, y01i, x02i, y02i,	//��������
		ti, t2,								//������tֵ
	ui, vi, u01i, v01i, u02i, v02i;		//
	int last_poly_index,				//��Ⱦ�б������һ����Ч�Ķ����
		insert_poly_index;				//�¶���εĲ���λ��
	VECTOR4D u, v, n;
	POLYF4DV2 temp_poly;
	insert_poly_index = last_poly_index = rend_list->num_polys;
	
	for (int poly = 0; poly < last_poly_index; poly++)
	{
		POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
		if ((curr_poly = NULL) || !(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
		{
			continue;
		}
		if (clip_flags & CLIP_POLY_X_PLANE)
		{

		}
	}
}
//��������
void Sort_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, int sort_method);
int Compare_AvgZ_POLYF4DV2(const void *arg1, const void *arg2);
int Compare_NearZ_POLYF4DV2(const void *arg1, const void *arg2);
int Compare_FarZ_POLYF4DV2(const void *arg1, const void *arg2);


void Sort_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, int sort_method){
	switch (sort_method)
	{
	case SORT_POLYLIST_AVGZ:
		qsort(rend_list->poly_ptrs, 0, rend_list->num_polys - 1, Compare_AvgZ_POLYF4DV1);
		break;
	case SORT_POLYLIST_NEARZ:
		qsort(rend_list->poly_ptrs, 0, rend_list->num_polys - 1, Compare_NearZ_POLY4DV1);
		break;
	case SORT_POLYLIST_FARZ:
		qsort(rend_list->poly_ptrs, 0, rend_list->num_polys - 1, Compare_FarZ_POLY4DV1);
		break;
	}
}
inline int Compare_AvgZ_POLYF4DV1(const void *arg1, const void *arg2){
	POLYF4DV1_PTR poly_1, poly_2;
	float z1, z2;
	poly_1 = *((POLYF4DV1_PTR*)arg1);
	poly_2 = *((POLYF4DV1_PTR*)arg2);
	z1 = (poly_1->tvlist[0].z + poly_1->tvlist[1].z + poly_1->tvlist[2].z) / 3;
	z2 = (poly_2->tvlist[0].z + poly_2->tvlist[1].z + poly_2->tvlist[2].z) / 3;
	if (z1 > z2)
		return 1;
	else if (z1 < z2)
		return -1;
	else
		return 0;
}
inline int Compare_NearZ_POLY4DV1(const void *arg1, const void *arg2){
	POLYF4DV1_PTR poly_1, poly_2;
	float z1, z2;
	poly_1 = *((POLYF4DV1_PTR*)arg1);
	poly_2 = *((POLYF4DV1_PTR*)arg2);
	z1 = min(poly_1->tvlist[0].z, poly_1->tvlist[1].z);
	z1 = min(poly_1->tvlist[2].z, z1);

	z2 = min(poly_2->tvlist[0].z, poly_2->tvlist[1].z);
	z2 = min(poly_2->tvlist[2].z, z2);
	if (z1 > z2)
		return 1;
	else if (z1 < z2)
		return -1;
	else
		return 0;
}
inline int Compare_FarZ_POLY4DV1(const void *arg1, const void *arg2){
	POLYF4DV1_PTR poly_1, poly_2;
	float z1, z2;
	poly_1 = *((POLYF4DV1_PTR*)arg1);
	poly_2 = *((POLYF4DV1_PTR*)arg2);
	z1 = max(poly_1->tvlist[0].z, poly_1->tvlist[1].z);
	z1 = max(poly_1->tvlist[2].z, z1);

	z2 = max(poly_2->tvlist[0].z, poly_2->tvlist[1].z);
	z2 = max(poly_2->tvlist[2].z, z2);
	if (z1 > z2)
		return 1;
	else if (z1 < z2)
		return -1;
	else
		return 0;
}
void qsort(POLYF4DV1_PTR *data, int s, int e, int *compare(const void *element1, const void *element2)){
	if (s >= e)
		return;
	int position;
	int p = s;
	int ss = s + 1, ee = e;
	POLYF4DV1_PTR cmp = data[p];
	POLYF4DV1_PTR temp = NULL;
	while(ss < ee)
	{
		while (compare(data[ss], cmp) > 0 && ss < ee)
			ss++;
		while (compare(data[ee], cmp) < 0 && ss < ee)
			ee--;
		if (ss >= ee)
			break;
		temp = data[ss];
		data[ss] = data[ee];
		data[ee] = temp;
	}
	if (compare(data[p], data[ss]) < 0)
	{
		temp = data[p];
		data[p] = data[ss];
		data[ss] = temp;
		position = ss;
	}
	else
	{
		temp = data[p];
		data[p] = data[ss - 1];
		data[ss - 1] = data[p];
		position = ss - 1;
	}
	qsort(data, s, position - 1, compare);
	qsort(data, position + 1, e, compare);
}


