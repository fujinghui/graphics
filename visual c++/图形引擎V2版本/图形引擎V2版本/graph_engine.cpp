#include <malloc.h>
#include <windows.h>
#include <stdio.h>
#include <float.h>
#include "math_engine.h"
#include "graph_engine.h"
#include "draw.h"
float cam_x = 0, cam_y = 0, cam_z = -1;
float ang_x = 0, ang_y = 0, ang_z = 0;
float camera_distance = 1;		//视平面到视点的距离

MATV1 materials[MAX_MATERIALS];	//系统中的材质
int num_materials;				//当前材质数量

LIGHTV1 lights[MAX_LIGHTS];		//系统中的光源数组
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
	//设置当前帧
	this->curr_frame = frame;
	//让指针指向顶点列表中的相应的帧
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
	//物体在世界坐标系中的位置
	object.world_pos.x = 0;
	object.world_pos.y = 0;
	object.world_pos.z = 0;
	//物体的朝向
	object.num_vertices = 8;	//顶点数量
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
	object.num_polys = 12;		//一个面由两个三角形构成,总共6个面
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
//物体坐标系->世界坐标系->相机坐标系
void DrawObject(HDC hdc, POINT3D points[], int count){
	POINT3D *cube_world = NULL;
	POINT3D *cube_camera = NULL;		//yxz
	POINT3D *cube_per = NULL;
	POINT3D *cube_screen = NULL;
	MATRIX4X4 temp_matrix1, temp_matrix2, Tcam;
	int i;
	//转换成世界坐标
	cube_world = (POINT3D_PTR)malloc(sizeof(POINT3D)* count);
	if (cube_world == NULL)
		return;
	for (i = 0; i < count; i++)
	{
		//旋转
		//points[i].x = points[i].x * (float)cos(PI / 180.0) - points[i].z * (float)sin(PI / 180.0);
		//points[i].z = points[i].x * (float)cos(PI / 180.0) + points[i].z * (float)sin(PI / 180.0);

		cube_world[i].x = points[i].x;
		cube_world[i].y = points[i].y;
		cube_world[i].z = points[i].z;
	}
	//转换成摄像机坐标
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
	//裁减，，等等一系列变换
	//......

	//透视投影变换
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

	//转换成屏幕坐标
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
	//这个函数初始化和重置传递进来的渲染列表
	//将为多边形插入到其中做好准备
	//这个版本的渲染列表由一个FACE4DV1指针数组组成
	//每一帧都需要调用这个函数

	//这里使用num_polys来跟踪渲染列表中包含的多边形数目
	//因此将其设置为0
	//如果需要使渲染列表更通用，需要采用更健壮的反感
	//并将其与多边形指针列表的关联切断
	rend_list->num_polys = 0;		//硬件编码
}	//Reset_RENDERLI



void Transform_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, MATRIX4X4_PTR mt, int coord_select){
	//这个函数使用传递进来的矩阵
	//对渲染列表中局部顶点数组或变换后的顶点数组总
	//所有的多边形顶点进行变换
	switch (coord_select){
	case TRANSFORM_LOCAL_ONLY:
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			//对每一个多边形进行变换，先获取当前多边形
			POLYF4DV1_PTR cur_poly = rend_list->poly_ptrs[poly];
			if ((cur_poly == NULL) || !(cur_poly->state & POLY4DV1_STATE_ACTIVE) || (cur_poly->state & POLY4DV1_STATE_CLIPPED) || (cur_poly->state & POLY4DV1_STATE_BACKFACE))
			{
				continue;			//当前多边形不进行变换
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
		//旋转ux
		Mat_Mul_VECTOR4D_4X4(&obj->ux, mt, &vresult);
		//旋转uy
		Mat_Mul_VECTOR4D_4X4(&obj->uy, mt, &vresult);
		//旋转uz
		Mat_Mul_VECTOR4D_4X4(&obj->uz, mt, &vresult);

		VECTOR4D_COPY(&obj->uz, &vresult);
	}
}
void Model_To_World_OBJECT4DV1(OBJECT4DV1_PTR obj, int coord_select){
	//没有使用矩阵
	//将物体的局部/模型坐标变换为世界坐标
	//结果被存储在物体的变换后的顶点列表中
	//遍历顶点列表，更具world_pos对所有坐标进行平移
	//将结果存储在数组vlist_trans[] 中
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
			//获得当前多边形
			POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
			//当前仅当多边形没有被提出和裁减掉，同时处于活动状态且可见时
			if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			//满足条件，对其进行变换
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

		//未激活，已被删除，显示了背面
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
	//将物体转化为一个多边形面列表
	//然后将可见，活动，没有被剔除和裁剪掉的多边形插入到渲染列表中
	//参数insert_local指定使用顶点列表vlist_local还是vlist_trans
	//lighting_on指出之前是否执行了光照
	//如果lighting_on = 1则将多边形插入渲染列表时，将使用这种颜色值覆盖多边形原来的颜色
	//该物体不处于活动状态，被剔除掉或不可见
	if (!(obj->state & OBJECT4DV1_STATE_ACTIVE) || (obj->state & OBJECT4DV1_STATE_CULLED) || (obj->state & OBJECT4DV1_STATE_VISIBLE))
		return 0;
	//提取物体包含的多边形
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		//获取多边形
		POLY4DV1_PTR curr_poly = &obj->plist[poly];
		//多边形是否可见
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue;
		//如果使用局部坐标，则改变多边形指向顶点列表
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
		//判断之前是否已执行了光照
		if (lighting_on)
		{
			base_color = (unsigned int)(curr_poly->color);
			curr_poly->color = (int)(base_color >> 16);
		}
		//插入多边形
		if (!Insert_POLY4DV1_RENDERLIST4DV1(rend_list, curr_poly));
		{
			//插入失败
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
	int cam_attr,				//相机属性
	POINT4D_PTR cam_pos,		//相机的初始位置
	VECTOR4D_PTR cam_dir,		//相机的初始角度
	POINT4D_PTR cam_target,		//UVN相机的初始目标
	float near_clip_z,			//近裁剪面和远裁剪面
	float far_clip_z,			//
	float fov,					//视野，单位为度
	float viewport_width,		//屏幕/视口的大小
	float viewport_height		//
	){
	cam->attr = cam_attr;
	VECTOR4D_COPY(&cam->pos, cam_pos);
	VECTOR4D_COPY(&cam->dir, cam_dir);
	///对于UVN相机来说
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
	//将所有矩阵都设置为单位矩阵
	MAT_IDENTITY_4X4(&cam->mcam);
	MAT_IDENTITY_4X4(&cam->mper);
	MAT_IDENTITY_4X4(&cam->mscr);
	//设置相关变量
	cam->fov = fov;
	//将视平面大小设置为2X(2/ar)
	cam->viewplane_width = 2.0f;
	cam->viewplane_height = 2.0f / cam->aspect_ratio;
	//根据fov和视平面大小计算视距
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
		//建立裁剪面
		POINT3D pt_origin;
		VECTOR3D_INITXYZ(&pt_origin, 0, 0, 0);
		VECTOR3D vn;				//面法线
		//右裁剪面
		VECTOR3D_INITXYZ(&vn, 1, 0, -1);			//平面x=z
		PLANE3D_Init(&cam->rt_clip_plane, &pt_origin, &vn, 1);
		//左裁剪面 平面-x=z
		VECTOR3D_INITXYZ(&vn, -1, 0, -1);
		PLANE3D_Init(&cam->lt_clip_plane, &pt_origin, &vn, 1);
		//上裁剪面 平面 y=z
		VECTOR3D_INITXYZ(&vn, 0, 1, -1);
		PLANE3D_Init(&cam->tp_clip_plane, &pt_origin, &vn, 1);
		//下裁剪面
		VECTOR3D_INITXYZ(&vn, 0, -1, -1);
		PLANE3D_Init(&cam->bt_clip_plane, &pt_origin, &vn, 1);
	}
	else
	{
		POINT3D pt_origin;		//平面上的一个点
		VECTOR3D_INITXYZ(&pt_origin, 0, 0, 0);
		VECTOR3D vn;			//面的法线
		//由于fov不为90度，计算比较复杂
		//首先计算表示裁剪面在平面X-Z和Y-Z上的2D投影的向量
		//然后计算与这两个向量垂直的向量，他就是裁剪面的法线
		//右裁剪面，请在草稿纸上验证这种方法
		VECTOR3D_INITXYZ(&vn, cam->view_dist, 0, -cam->viewplane_width / 2.0f);
		PLANE3D_Init(&cam->rt_clip_plane, &pt_origin, &vn, 1);
		//左裁剪面，可以绕z轴反射有裁剪面的法线，来的到左裁剪面的法线
		//因为这两个裁剪面是关于Z轴对称的，因此只需对x求负
		VECTOR3D_INITXYZ(&vn, -cam->view_dist, 0, -cam->viewplane_width / 2.0f);
		PLANE3D_Init(&cam->lt_clip_plane, &pt_origin, &vn, 1);
		//上裁剪面
		VECTOR3D_INITXYZ(&vn, 0, cam->view_dist, -cam->viewplane_width / 2.0f);
		PLANE3D_Init(&cam->tp_clip_plane, &pt_origin, &vn, 1);
		//下裁剪面
		VECTOR3D_INITXYZ(&vn, 0, -cam->view_dist, -cam->viewplane_width / 2.0f);
		PLANE3D_Init(&cam->bt_clip_plane, &pt_origin, &vn, 1);
	}
}
void Build_CAM4DV1_Matrix_Euler(CAM4DV1_PTR cam, int cam_rot_seq){
	//这个函数根据欧拉角度计算相机变换矩阵
	//并将其存储到传入的相机对象中
	MATRIX4X4 mt_inv,			//相机平移矩阵的逆矩阵
		mx_inv,			//相机绕x轴的旋转矩阵的逆矩阵
		my_inv,			//相机绕y轴的旋转矩阵的逆矩阵
		mz_inv,			//相机绕z轴的旋转矩阵的逆矩阵
		mrot,			//所有旋转矩阵的逆矩阵
		mtmp;			//用于存储临时矩阵
	//第一步，根据相机位置计算相机平移矩阵的逆矩阵
	Mat_Init_4X4(&mt_inv,	//单位平移矩阵
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam->pos.x, -cam->pos.y, -cam->pos.z, 1
		);
	//第二部，创建旋转矩阵的逆矩阵
	//要计算正规矩阵的逆矩阵，可以将其转置
	//也可以将每个旋转角度取负
	//首先计算3个旋转矩阵的逆矩阵
	float theta_x = cam->dir.x;
	float theta_y = cam->dir.y;
	float theta_z = cam->dir.z;
	//计算角度x的正弦和余弦
	float cos_theta = Fast_Cos(theta_x);
	float sin_theta = -Fast_Sin(theta_x);
	//建立矩阵
	Mat_Init_4X4(&mx_inv, 
		1, 0, 0, 0,
		0, cos_theta, sin_theta, 0,
		0, -sin_theta, cos_theta, 0,
		0, 0, 0, 1
		);

	//计算角度y的正弦和余弦
	cos_theta = Fast_Cos(theta_y);
	sin_theta = -Fast_Sin(theta_y);
	Mat_Init_4X4(&my_inv,
		cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1
		);
	//计算角度z的正弦和余弦
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
		//使用球面坐标模式
		float phi = cam->dir.x;
		float theta = cam->dir.y;

		float sin_phi = Fast_Sin(phi);
		float cos_phi = Fast_Cos(phi);

		float sin_theta = Fast_Sin(theta);
		float cos_theta = Fast_Cos(theta);
		//计算目标点在单位球面上的位置
		cam->target.x = -1 * sin_phi * sin_theta;
		cam->target.y = 1 * cos_phi;
		cam->target.z = 1 * sin_phi * cos_theta;
	}
	//第一步 n = <目标位置-观察参考点>
	VECTOR4D_Build(&cam->pos, &cam->target, &cam->n);
	//第二部 将v设置为<0, 1, 0>
	VECTOR4D_INITXYZ(&cam->v, 0, 1, 0);
	//第三部 u = (v *（叉积） n)
	VECTOR4D_Cross(&cam->v, &cam->n, &cam->u);
	//第四部 v = (n * u)
	VECTOR4D_Cross(&cam->n, &cam->u, &cam->v);
	//第五步 对所有向量都进行归一化
	VECTOR4D_Normalize(&cam->u);
	VECTOR4D_Normalize(&cam->v);
	VECTOR4D_Normalize(&cam->n);
	//将u，v，n带入，得到uvn旋转矩阵
	Mat_Init_4X4(&mt_uvn, cam->u.x, cam->v.x, cam->n.x, 0,
		cam->u.y, cam->v.y, cam->n.y, 0,
		cam->u.z, cam->v.z, cam->n.z, 0,
		0, 0, 0, 1
		);
	Mat_Mul_4X4(&mt_inv, &mt_uvn, &cam->mcam);
}
void World_To_Camera_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam){
	//这是一个基于矩阵的函数
	//他根据传入的相机变换矩阵，将物体的世界坐标变换为相机坐标
	//他完全不考虑多边形本身，知识对vlist_trans[]中的顶点进行变换
	//这是变换方法之一,您可能选择对渲染列表进行变换
	//因为渲染列表中的多边形表示的几何体都通过哦背面提出

	//将物体的每个顶点变换为相机坐标
	//这里假设顶点已经被变换为世界坐标
	//且结果储存在vlist_trans[]中
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		//使用相机对象中的矩阵mcam对顶点进行变换
		POINT4D presult;
		//对顶点进行变换
		Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex], &cam->mcam, &presult);
		//将结果存回去
		VECTOR4D_COPY(&obj->vlist_trans[vertex], &presult);
	}
}
void World_To_Camera_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam){
	//这个函数时基于矩阵
	//他根据传入的相机变换矩阵将渲染列表中每个多边形变换为相机坐标
	//如果在流水线的上游已经将每个物体转换为多边形
	//并将他们插入到渲染列表中

	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		//这个多边形是否有效
		//当前仅当多边形处于活动状态，可见时才对他进行变换
		//但对于线框引擎，背面的概念无关紧要
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		for (int vertex = 0; vertex < 3; vertex++)
		{
			//使用相机对象中的矩阵mcam对顶点进行变换
			POINT4D presult;
			//对顶点进行变换
			Mat_Mul_VECTOR4D_4X4(&curr_poly->tvlist[vertex], &cam->mcam, &presult);
			//将结果存回去
			VECTOR4D_COPY(&curr_poly->tvlist[vertex], &presult);
		}
	}
}
int Cull_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam, int cull_flags){
	//这个函数时基于矩阵的
	//他根据传入的相机信息判断物体是佛阻碍视景体内
	//参数cull_flags指定在那些轴上\执行剔除
	//其取值为各种剔除标记的OR
	//如果物体被剔除，将相应的设置器状态
	//这个函数假设相机和物体都是有效的
	//第1步+
	POINT4D sphere_pos;			//用于存储包围球球心变换后的坐标
	//对点进行变换
	Mat_Mul_VECTOR4D_4X4(&obj->world_pos, &cam->mcam, &sphere_pos);
	//第2步：根据剔除标记对物体执行剔除操作
	if (cull_flags & CULL_OBJECT_Z_PLANE)
	{
		//只根据远近裁剪面来剔除物体
		//使用远近裁剪面进行测试
		if (((sphere_pos.z + obj->max_radius) > cam->far_clip_z) || ((sphere_pos.z - obj->max_radius) < cam->near_clip_z))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}
	if (cull_flags & CULL_OBJECT_X_PLANE)
	{
		//只根据左右裁剪面进行物体剔除
		float z_test = (0.5f) * cam->viewplane_width * sphere_pos.z / cam->view_dist;
		if (((sphere_pos.x - obj->max_radius) > z_test)				//右边
			|| ((sphere_pos.x + obj->max_radius) < -z_test))		//左边
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}
	if (cull_flags & CULL_OBJECT_Y_PLANE)
	{
		//只根据上下裁剪面对物体进行剔除
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
	//这个函数时重置传入传入的物体的裁剪状态
	//每一个游戏帧都需要调用此函数
	RESET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV1_PTR curr_poly = &obj->plist[poly];
		//判断多边形是否可见
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE))
		{
			continue;
		}
		//重置被裁剪掉和背面标记
		RESET_BIT(curr_poly->state, POLY4DV1_STATE_CLIPPED);
		RESET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
	}


}
void Remove_Backfaces_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam){
	//这个函数不是基于矩阵的
	//他是根据组vlist_trans中的顶点数据以及相机位置
	//消除物体的背面多边形
	//这里只设置多边形的背面状态
	//检测物体是否已被剔除
	if (obj->state & OBJECT4DV1_STATE_CULLED)
	{
		return;
	}
	//处理物体的每个多边形
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		//获取多边形
		POLY4DV1_PTR curr_poly = &obj->plist[poly];
		//该多边形是否有效
		//判断多边形是否没有被裁减掉，没有被剔除，处于活动状态，可见且不是双面的
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->attr & POLY4DV1_ATTR_2SIDED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		//获取顶点列表中的顶点索引
		//多边形不是自包含的，而是基于物体的顶顶啊列表
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];
		//我们将使用变换后的多边形顶点列表
		//因为背面消除智能在顶点被转换为世界坐标之后才能进行
		//需要计算多边形的面法线
		//顶点是按顺时针方向排列的
		VECTOR4D u, v, n;
		//已知两点求向量，
		VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
		VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
		VECTOR4D_Cross(&u, &v, &n);								//叉积，该平面的法线
		//创建指向视点的向量
		VECTOR4D view;
		VECTOR4D_Build(&obj->vlist_trans[vindex_0], &cam->pos, &view);
		//计算点积
		float dp = VECTOR4D_Dot(&n, &view);
		//如果dp<0 则多边形不可见
		if (dp <= 0.0f)
		{
			SET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
		}
	}
}
void Remove_Backfaces_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam){
	//根据多边形列表数据tvlist和相机位置来消除多边形列表中属于背面的多边形
	//顺时针是显示该面，逆时针是隐藏该面
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		//获取当前多边形
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		//该多边形是否有效
		//判断多边形是否没有被裁剪掉，没有被剔除，处于活动状态，可见且不是双面的
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->attr & POLY4DV1_ATTR_2SIDED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		VECTOR4D u, v, n;
		//计算u和v

		VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
		VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
		//计算叉积
		VECTOR4D_Cross(&u, &v, &n);
		//创建指向视点的向量
		VECTOR4D view;
		VECTOR4D_Build(&curr_poly->tvlist[0], &cam->pos, &view);
		//计算点积
		float dp = VECTOR4D_Dot(&n, &view);
		if (dp < 0.0f)
			SET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
	}
}
void Camera_To_Perspective_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam){
	//这个函数不是基于矩阵的
	//他根据传入的相机对象将物体的相机坐标转换为透视坐标
	//它根本不关心多边形本省
	//而只是对vlist_trans[]中的顶点进行变换
	//这只是执行透视变换的方法之一，您可能不采用这种方法
	//而是对渲染列表进行变换，因为渲染列表中的多边形表示的是通过了背面剔除
	//最后，这个函数知识基于实验目的而编写的
	//您可能不会将物体完整的保留到流水线的这个阶段
	//因为物体可能只有一个多边形是可见的，而这个函数对所有多边形都进行变换
	//将物体的每个顶点变换为透视坐标
	//这里假设物体已经被变换为相机坐标，且结果存储在vlist_trans[]中
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		float z = obj->vlist_trans[vertex].z;
		//根据相机的观察参数对顶点进行变换
		obj->vlist_trans[vertex].x = cam->view_dist * obj->vlist_trans[vertex].x / z;
		obj->vlist_trans[vertex].y = cam->view_dist * obj->vlist_trans[vertex].y * cam->aspect_ratio / z;
	}
}
void Camera_To_Perspective_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		//获取当前多边形
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		//该多边形是否有效
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
	//只是vlist_trans[]中的顶点进行边关
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
		//根据相机的观察参数对顶点进行变换
		obj->vlist_trans[vertex].x = cam->view_dist * obj->vlist_trans[vertex].x / z;
		obj->vlist_trans[vertex].y = cam->view_dist * obj->vlist_trans[vertex].y * cam->aspect_ratio / z;

		//转换成屏幕坐标
		obj->vlist_trans[vertex].x = alpha + alpha * obj->vlist_trans[vertex].x;
		obj->vlist_trans[vertex].y = beta - beta * obj->vlist_trans[vertex].y;
	}
}
void Camera_To_Perspective_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam){
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		//获取当前多边形
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
		//该多边形是否有效
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
			//相机到透视坐标
			curr_poly->tvlist[vertex].x = cam->view_dist * curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = cam->view_dist * curr_poly->tvlist[vertex].y * cam->aspect_ratio / z;

			//透视坐标到屏幕坐标
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
		//提取指向主列表的顶点索引
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];
		//控制三角形
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

//材质函数
int Reset_Materials_MATV1(void){
	static int first_time = 1;
	//如果这是第一次调用该函数，则重置所有材质
	if (first_time)
	{
		memset(materials, 0, MAX_MATERIALS * sizeof(MATV1));
		first_time = 0;
	}
	for (int curr_matt = 0; curr_matt < MAX_MATERIALS; curr_matt++)
	{
		//不管材质是否被激活，都释放与之相关的纹理图
		Destroy_Bitmap(&materials[curr_matt].texture);
		memset(&materials[curr_matt], 0, sizeof(MATV1));
	}
	return 1;

}

//光源函数
int Reset_Lights_LIGHTV1(void){
	static int first_time = 1;
	memset(lights, 0, MAX_LIGHTS * sizeof(LIGHTV1));
	num_lights = 0;
	first_time = 0;
	return 1;
}
int Init_Light_LIGHTV1(
	int index,						//要创建的光源的索引(0到MAX_LIGHTS-1)
	int _state,						//光源状态
	int _attr,						//光源类型及其属性
	RGBAV1 _c_ambient,				//环境光强度
	RGBAV1 _c_diffuse,				//散射光强度
	RGBAV1 _c_specular,				//镜面反射光强度
	POINT4D_PTR _pos,				//光源位置
	VECTOR4D_PTR _dir,				//光源方向
	float _kc, float _kl, float _kq,	//衰减因子
	float _spot_inner,				//聚光灯内锥角
	float _spot_outer,				//聚光灯外锥角
	float _pf)						//聚光灯指数因子
{
	//这个函数根据传入的参数初始化光源
	//调用该函数时，为确保创建的光源有效将不需要的参数值设置为0
	if (index < 0 || index >= MAX_LIGHTS)
		return 0;
	lights[index].state			= _state;		//光源状态
	lights[index].id			= index;		//光源id
	lights[index].attr = _attr;					//光源类型及其他属相
	lights[index].c_ambient = _c_ambient;		//环境光强度
	lights[index].c_diffuse = _c_diffuse;		//散射光强度
	lights[index].c_specular = _c_specular;		//镜面反射光强度
	lights[index].kc = _kc;						//常量，线性，二次衰减因子
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
	//16为光照函数
	//该函数根据传入的光源列表和相机对物体执行光照计算
	//他支持固定着色已经根据环境光源，无穷远光源，点光源和聚光灯进行恒定着色
	unsigned int r_base, g_base, b_base,		//原来的颜色值
		r_sum, g_sum, b_sum,			//全部光源的总体光照效果
		shaded_color;					//最后的颜色
	float dp,				//点积
		dist,				//表面和光源之间的距离
		i,					//强度
		nl,					//法线长度
		atten;				//衰减计算结果
	if (!(obj->state & OBJECT4DV1_STATE_ACTIVE) || (obj->state & OBJECT4DV1_STATE_CULLED) || !(obj->state & OBJECT4DV1_STATE_VISIBLE))
	{
		return 0;
	}
	//::MessageBox(NULL, "fadfsa", "fad", MB_OK);
	//处理每个多边形
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		//获取多边形
		POLY4DV1_PTR curr_poly = &obj->plist[poly];
		curr_poly->color_light_trans = curr_poly->color;
		//多边形是否有效
		//多边形没有被裁剪和剔除掉，处于活动状态，不是背面且可见
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) || (curr_poly->state & POLY4DV1_STATE_CLIPPED) || (curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue;
		//提取指向主列表的顶点索引
		//多边形不是自包含的，而是基于物体的顶点列表的
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];
		//使用变换后的多边形顶点列表
		//因为背面消除只有进入流水线的世界坐标阶段后才有意义
		//检查多边形的着色模式
		if ((curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_FLAT) || (curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_GOURAUD))
		{
			//将565模式颜色转换成888模式
			r_base = curr_poly->color & 0xff;		//31
			g_base = (curr_poly->color >> 8) & 0xff;//63
			b_base = (curr_poly->color >> 16) & 0xff;
			
			r_sum = 0; g_sum = 0; b_sum = 0;
			for (int curr_light = 0; curr_light < max_lights; curr_light++)
			{
				//判断光源是否打开
				if (lights[curr_light].state == LIGHTV1_STATE_OFF)
					continue;
				//判断光源类型
				if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)
				{
					//当每个分量与多边形颜色相乘，并除以256，以确保结果为0-255之间
					r_sum += ((lights[curr_light].c_ambient.r * r_base) / 256);
					g_sum += ((lights[curr_light].c_ambient.g * g_base) / 256);
					b_sum += ((lights[curr_light].c_ambient.b * b_base) / 256);
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)
				{
					//无穷远光源
					//需要计算多边形面的法线
					//顶点按顺时针方向排列的，因此u=p0->p1,v=p0->p2 n = u *(叉乘) v
					VECTOR4D u, v, n;
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
					//计算叉积
					VECTOR4D_Cross(&v, &u, &n);
					float n1 = VECTOR4D_Length_Fast(&n);
					//散射项的计算公式如下：
					//因此只需要将他们乘起来即可
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
						//执行点光源光照计算
						//点光源的光照模型如下
						//			   IOpoint * Clpoint
						//I(d)point = -------------------
						//			   kc + kl*d + kq*d2
						VECTOR4D u, v, n, l;
						//计算u和v
						VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
						VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
						//计算叉积
						VECTOR4D_Cross(&u, &v, &n);
						float nl = VECTOR4D_Length_Fast(&n);
						//计算从表面到光源的向量
						VECTOR4D_Build(&obj->vlist_trans[vindex_0], &lights[curr_light].pos, &l);
						//计算距离和衰减
						dist = VECTOR4D_Length_Fast(&l);
						//对于散射光
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
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)					//简单聚光灯(1号类型聚光灯)
				{
					//顶点按顺时针方向排列，因此u=p0->p1,v=p0->p2,n=u*(叉乘)v
					VECTOR4D u, v, n, l;
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
					//计算叉积
					VECTOR4D_Cross(&v, &u, &n);
					//为优化这个步骤，可预先计算出所有多边形法线的长度
					nl = VECTOR4D_Length_Fast(&n);
					VECTOR4D_Build(&obj->vlist_trans[vindex_0], &lights[curr_light].pos, &l);
					//计算距离和衰减
					dist = VECTOR4D_Length_Fast(&l);
					//对于散射光
					//Itotald=Rsdiffuse*Idiffuse * (n . l);
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					//仅当dp>0时，才需要考虑光源的影响
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
						i = 128 * dp / (nl * atten);
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)			//2号类别的聚光灯
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
						//计算s的长度，以归一化
						dist = VECTOR4D_Length_Fast(&s);
						float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir) / dist;
						if (dpsl > 0)
						{
							atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
							float dpsl_exp = dpsl;
							//计算整数次方
							for (int e_index = 01; e_index < (int)lights[curr_light].pf; e_index++)
							{
								dpsl_exp *= dpsl;
							}
							//现在dpls_exp存储的是(dpsl)^pf,即(s.l)^pf
							i = 128 * dp * dpsl_exp / (nl * atten);
							r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
						}//end if
					}	//end if
				}	//end if spot light
			}	//end for light
			//确保颜色分量不溢出
			if (r_sum > 255) r_sum = 255;
			if (g_sum > 255) g_sum = 255;
			if (b_sum > 255) b_sum = 255;
			//写入颜色
			shaded_color = RGB(r_sum, g_sum, b_sum);
			curr_poly->color_light_trans = shaded_color;// (int)((shaded_color) | curr_poly->color);
		}
		else
		{
			curr_poly->color_light_trans = 0x000000;/// curr_poly->color;
			//固定着色
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
				if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)					//环境光源
				{
					r_sum += ((lights[curr_light].c_ambient.r * r_base) / 256);
					g_sum += ((lights[curr_light].c_ambient.g * g_base) / 256);
					b_sum += ((lights[curr_light].c_ambient.b * b_base) / 256);
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)			//无穷远处光源
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
				else if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)				//点光源
				{
					VECTOR4D u, v, n, l;
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
					VECTOR4D_Cross(&u, &v, &n);

					nl = VECTOR4D_Length_Fast(&n);

					VECTOR4D_Build(&curr_poly->tvlist[0], &lights[curr_light].pos, &l);
					//光源到物体的距离
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
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)							//1号点光源
				{
					VECTOR4D u, v, n, l;
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
					VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
					VECTOR4D_Cross(&v, &u, &n);					//该平面的反向法线,主要与聚光灯的方向有关

					nl = VECTOR4D_Length_Fast(&n);

					//求物体到聚光灯的距离
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
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)			//2号类别的聚光灯
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
						dist = VECTOR4D_Length_Fast(&s);			//邱光远到该物体的距离
						//求光源到物体的向量与光源方向向量的向量积
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


//----------------------------------V2版本的函数--------------------------------------
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
		//旋转ux
		Mat_Mul_VECTOR4D_4X4(&obj->ux, mt, &vresult);
		//旋转uy
		Mat_Mul_VECTOR4D_4X4(&obj->uy, mt, &vresult);
		//旋转uz
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
	Mat_Mul_VECTOR4D_4X4(&obj->world_pos, &cam->mcam, &sphere_pos);			//执行相机变换,物体的世界坐标转换成相机坐标系
	//第2步：根据剔除标记对物体执行剔除操作
	if (cull_flags & CULL_OBJECT_Z_PLANE)
	{
		//只根据远近裁剪面来剔除物体
		//使用远近裁剪面进行测试
		if (((sphere_pos.z + obj->max_radius[0]) > cam->far_clip_z) || ((sphere_pos.z - obj->max_radius[0]) < cam->near_clip_z))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}
	if (cull_flags & CULL_OBJECT_X_PLANE)
	{
		//只根据左右裁剪面进行物体剔除
		float z_test = (0.5f) * cam->viewplane_width * sphere_pos.z / cam->view_dist;
		if (((sphere_pos.x - obj->max_radius[0]) > z_test)				//右边
			|| ((sphere_pos.x + obj->max_radius[0]) < -z_test))		//左边
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}
	if (cull_flags & CULL_OBJECT_Y_PLANE)
	{
		//只根据上下裁剪面对物体进行剔除
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
	RESET_BIT(obj->state, OBJECT4DV2_STATE_CULLED);		//消除物体被剔除状态
	//重置物体的每个面的状态
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR curr_poly = &obj->plist[poly];
		//判断多边形是否可见
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
		//当前面不需要移除
		if (!(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->attr & POLY4DV2_ATTR_2SIDED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			continue;
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];
		VECTOR4D u, v, n;
		VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_1].v, &u);
		VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_2].v, &v);
		VECTOR4D_Cross(&u, &v, &n);			//求解出向量

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
		//cam->view_dist是摄像机打投影平面的距离
		float z = obj->vlist_trans[vertex].z;
		obj->vlist_trans[vertex].x = cam->view_dist * obj->vlist_trans[vertex].x / z;
		obj->vlist_trans[vertex].y = cam->view_dist * obj->vlist_trans[vertex].y * cam->aspect_ratio / z;
	}
}
void Perspective_To_Screen_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam){					//转换成屏幕坐标
	float alpha = (0.5 * cam->viewport_width - 0.5);				//x轴中心点
	float beta = (0.5 * cam->viewport_height - 0.5);				//y轴中心点
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
	//物体不可见或未激活
	if (!(obj->state & OBJECT4DV2_STATE_ACTIVE) || (obj->state & OBJECT4DV2_STATE_CULLED) || !(obj->state & OBJECT4DV2_STATE_VISIBLE))
		return 0;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR curr_poly = &obj->plist[poly];						//获取当前这个面的数据
		//判断当前这个面是否可见，也就是能够显示
		if (!(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			;// continue;

		//--------------------------
		SET_BIT(curr_poly->state, POLY4DV2_STATE_LIT);
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];
		//FLAT 着色
		if (curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_FLAT)
		{
			r_base = curr_poly->color & 0xff;
			g_base = (curr_poly->color >> 8) & 0xff;
			b_base = (curr_poly->color >> 16) & 0xff;
			r_sum = 0;
			g_sum = 0;
			b_sum = 0;
			n.z = FLT_MAX;
			
			//循环每一个光源，执行光照计算
			for (int curr_light = 0; curr_light < max_lights; curr_light++)
			{
				//当前这个光源的状态是关闭的
				if (lights[curr_light].state == LIGHTV1_STATE_OFF)
				{
					continue;
				}
				//MessageBox(NULL, "????", "????", MB_OK);
				if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)						//环境光源
				{
					r_sum += (lights[curr_light].c_ambient.r * r_base) / 256;
					g_sum += (lights[curr_light].c_ambient.g * g_base) / 256;
					b_sum += (lights[curr_light].c_ambient.b * b_base) / 256;
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)				//无穷远光源
				{
					if (n.z == FLT_MAX)
					{
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_1].v, &u);
						VECTOR4D_Build(&obj->vlist_trans[vindex_1].v, &obj->vlist_trans[vindex_2].v, &v);
						//求出向量叉积     
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
						VECTOR4D_Cross(&u, &v, &n);			//叉积
					}
					nl = curr_poly->nlength;
					VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast(&l);
					//求解面与法向量的点积
					dp = VECTOR4D_Dot(&n, &l);
					//当期的点光源能够照到此平面
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
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)				//1号类别的聚光灯(简单聚光灯)
				{
					//计算叉积公式,求解当前这个平面的法向量（即垂直于当前平面的直线）
					if (n.z == FLT_MAX)
					{
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_1].v, &u);
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_2].v, &v);
						VECTOR4D_Cross(&u, &v, &n);			//叉积
					}
					nl = curr_poly->nlength;

					//求出物体到光源的位置
					VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &lights[curr_light].pos, &l);
					dist = VECTOR4D_Length_Fast(&l);
					//求出聚光灯的方向与面的法向量的点积
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					//满足大于0，则表明光源刚好能够照到该面
					if (dp > 0)
					{
						atten = (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						i = 128 * dp / (nl * dist * atten);
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)				//2号类别的聚光灯(复杂聚光灯)
				{
					//计算叉积公式（上同）
					if (n.z == FLT_MAX)
					{
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_1].v, &u);
						VECTOR4D_Build(&obj->vlist_trans[vindex_0].v, &obj->vlist_trans[vindex_2].v, &v);
						VECTOR4D_Cross(&u, &v, &n);			//叉积
					}
					nl = curr_poly->nlength;
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);	
					if (dp > 0)
					{
						VECTOR4D_Build(&lights[curr_light].pos, &obj->vlist_trans[vindex_0].v, &s);
						dists = VECTOR4D_Length_Fast(&s);
						//物体越在聚光灯的外围，dpsl的值越小
						float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir) / dists;
						//当前面没有超出聚光灯的可照亮范围
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
		else if (curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_GOURAUD)				//另一种着色模式
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
				if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)					//环境光，每个点的光源值都是一样的
				{
					ri = ((lights[curr_light].c_ambient.r * r_base) / 255);
					gi = ((lights[curr_light].c_ambient.g * g_base) / 255);
					bi = ((lights[curr_light].c_ambient.b * b_base) / 255);
					r_sum0 += ri; g_sum0 += gi; b_sum0 += bi;
					r_sum1 += ri; g_sum1 += gi; b_sum1 += bi;
					r_sum2 += ri; g_sum2 += gi; b_sum2 += bi;
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)								//无穷远处光源
				{
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_0].n, &lights[curr_light].dir);			//求光源方向与物体的这个面的这个点的点积
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
				else if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)								//点光源
				{
					//点0到光源的向量
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
					//点1到光源的向量
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
					//点2到光源的向量
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
					//计算当前这个面的此顶点与光源的位置
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
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)					//2号复杂聚光灯
				{
					//判断当前这个点的法向量是否在聚光灯内
					dp = VECTOR4D_Dot(&obj->vlist_trans[vindex_0].n, &lights[curr_light].dir);
					if (dp > 0)
					{
						//判断当前这个带你是否在聚光灯内
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
					//判断点的法向量与光源的位置
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
			//设置当前这个面的三个点的颜色（GROUAUD设置)
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

//渲染函数
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
		//普通着色模式(PHONG) //ping
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
		//普通着色模式
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

//////////////////////渲染列表的操作/////////////////////
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
int Insert_POLY4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, POLY4DV2_PTR poly){						//将POLY4DV2插入到RENDLIST
	//渲染列表中已满，无法再插入面数据
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

	//纹理
	rend_list->poly_data[rend_list->num_polys].tvlist[0].t = poly->tlist[poly->text[0]];
	rend_list->poly_data[rend_list->num_polys].tvlist[1].t = poly->tlist[poly->text[1]];
	rend_list->poly_data[rend_list->num_polys].tvlist[2].t = poly->tlist[poly->text[2]];

	rend_list->poly_data[rend_list->num_polys].vlist[0].t = poly->tlist[poly->text[0]];
	rend_list->poly_data[rend_list->num_polys].vlist[1].t = poly->tlist[poly->text[1]];
	rend_list->poly_data[rend_list->num_polys].vlist[2].t = poly->tlist[poly->text[2]];

}
int Insert_POLYF4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, POLYF4DV2_PTR poly){						//POLYF4DV2插入到RENDLIST
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
int Insert_OBJECT4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, OBJECT4DV2_PTR obj, int insert_local){						//将OBJECT4DV2CT插入到RENDLIST
	unsigned int base_color;
	if (!(obj->state & OBJECT4DV2_STATE_ACTIVE) || (obj->state & OBJECT4DV2_STATE_CULLED) || !(obj->state & OBJECT4DV2_STATE_VISIBLE))
		return 0;
	//将物体里的每一个面都插入到渲染列表中
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR curr_poly = &obj->plist[poly];
		if (!(curr_poly->state & POLY4DV2_STATE_ACTIVE) || (curr_poly->state & POLY4DV2_STATE_CLIPPED) || (curr_poly->state & POLY4DV2_STATE_BACKFACE))
			continue;
		//保存当前边的数据指针
		VERTEX4DTV1_PTR vlist_old = curr_poly->vlist;
		//根据用户的选择将当前边的数据指针指向新的信息内存中
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
		//恢复当前边的老的数据指针
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
		//FLAT面着色模式
		if (curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_FLAT)
		{
			//取得原始面的颜色
			//根据原始颜色进行光照计算
			r_base = curr_poly->color & 0xff;
			g_base = curr_poly->color >> 8 & 0xff;
			b_base = curr_poly->color >>16 & 0xff;
			r_sum = 0; g_sum0; b_sum = 0;
			n.z = FLT_MAX;
			for (int curr_light = 0; curr_light < max_ligths; curr_light++)
			{
				if (lights[curr_light].state == LIGHTV1_STATE_OFF)
					continue;
				//环境光
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
						//求当前面的法向量
						VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[1].v, &u);
						VECTOR4D_Build(&curr_poly->tvlist[0].v, &curr_poly->tvlist[2].v, &v);

						VECTOR4D_Cross(&u, &v, &n);
					}
					nl = curr_poly->nlength;
					dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
					if (dp > 0)
					{
						i = 128 * dp / nl;				//最大不过128....至少我这么认为，难道还有其他的意义？？？？
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				} //end if LIGHTV1_ATTR_INFINITE
				else if (lights[curr_light].attr == LIGHTV1_ATTR_POINT)
				{
					//如果之前当前面的法向量已经求过了，则不再需要求了
					if (n.z == FLT_MAX)
					{
						//求当前面的法向量
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
					//如果之前当前面的法向量已经求过了，则不再需要求了
					if (n.z == FLT_MAX)
					{
						//求当前面的法向量
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
					//如果之前当前面的法向量已经求过了，则不再需要求了
					if (n.z == FLT_MAX)
					{
						//求当前面的法向量
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
			//GROUND着色模式
			//留待之后实现
		}
	}
	return 1;
}
void Draw_RENDERLIST4DV2_Wire16(RENDERLIST4DV2_PTR rend_list, HDC hdc, int lpitch);
void Draw_RENDERLIST4DV2_Solid16(RENDERLIST4DV2_PTR rend_list, HDC hdc, int lpitch);
void Clip_Polys_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam, int clip_flags){
	//内部裁剪码
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
	int vertex_ccodes[3];			//用于存储裁剪标记
	int num_verts_in;
	int v0, v1, v2;
	float z_factor, z_test;
	float xi, yi, x01i, y01i, x02i, y02i,	//交点坐标
		ti, t2,								//参数化t值
	ui, vi, u01i, v01i, u02i, v02i;		//
	int last_poly_index,				//渲染列表中最后一个有效的多边形
		insert_poly_index;				//新多边形的插入位置
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
//辅助功能
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


