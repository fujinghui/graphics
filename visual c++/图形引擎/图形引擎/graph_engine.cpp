#include <malloc.h>
#include <windows.h>
#include <stdio.h>
#include "math_engine.h"
#include "graph_engine.h"
float cam_x = 0, cam_y = 0, cam_z = -1;
float ang_x = 0, ang_y = 0, ang_z = 0;
float camera_distance = 1;		//视平面到视点的距离
MATRIX4X4 Tcam_inv = {
	 1, 0, 0, 0 ,
	 0, 1, 0, 0 ,
	 0, 0, 1, 0 ,
	 -cam_x, -cam_y, -cam_z, 1 
};
MATRIX4X4 Rcamx_inv = {
	 1, 0, 0, 0 ,
	 0, cos(-ang_x), sin(-ang_x), 0 ,
	 0, -sin(-ang_x), cos(-ang_x), 0 ,
	 0, 0, 0, 1 
};
MATRIX4X4 Rcamy_inv = {
	 cos(-ang_y), 0, -sin(-ang_y), 0 ,
	 0, 1, 0, 0 ,
	 sin(-ang_y), 0, cos(-ang_y), 0 ,
	 0, 0, 0, 1 
};
MATRIX4X4 Rcamz_inv = {
	 cos(-ang_z), sin(-ang_z), 0, 0 ,
	 -sin(-ang_z), cos(-ang_z), 0, 0 ,
	 0, 0, 1, 0 ,
	 0, 0, 0, 1 
};

//RENDERLIST4DV1 rend_list;







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
		for(int vertex = 0; vertex < obj->num_vertices; vertex++)
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
	Mat_Init_4X4(&mt_inv,
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
	Mat_Init_4X4(&mx_inv, 1, 0, 0, 0, 
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
	//参数cull_flags指定在那些轴上执行剔除
	//其取值为各种剔除标记的OR
	//如果物体被剔除，将相应的设置器状态
	//这个函数假设相机和物体都是有效的
	//第1步
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
		if (((sphere_pos.x - obj->max_radius) > z_test)			//右边
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
		if ((sphere_pos.y - obj->max_radius) > z_test||((sphere_pos.y + obj->max_radius) < -z_test))
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
		 return ;
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
		obj->vlist_trans[vertex].x =  cam->view_dist * obj->vlist_trans[vertex].x / z;
		obj->vlist_trans[vertex].y =  cam->view_dist * obj->vlist_trans[vertex].y * cam->aspect_ratio / z;
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
			curr_poly->tvlist[vertex].x =  cam->view_dist * curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y =  cam->view_dist * curr_poly->tvlist[vertex].y * cam->aspect_ratio / z;
			
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
		HPEN hPen = CreatePen(0, 1, obj->plist[poly].color);
		SelectObject(hdc, hPen);
		DeleteObject(hPen);

		MoveToEx(hdc, (int)obj->vlist_trans[vindex_0].x, (int)obj->vlist_trans[vindex_0].y, NULL);
		LineTo(hdc, (int)obj->vlist_trans[vindex_1].x, (int)obj->vlist_trans[vindex_1].y);
		LineTo(hdc, (int)obj->vlist_trans[vindex_2].x, (int)obj->vlist_trans[vindex_2].y);
		LineTo(hdc, (int)obj->vlist_trans[vindex_0].x, (int)obj->vlist_trans[vindex_0].y);

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
		HPEN hPen = CreatePen(0, 1, rend_list->poly_ptrs[poly]->color);
		SelectObject(hdc, hPen);
		DeleteObject(hPen);
		MoveToEx(hdc, (int)rend_list->poly_ptrs[poly]->tvlist[0].x, (int)rend_list->poly_ptrs[poly]->tvlist[0].y, NULL);
		LineTo(hdc, (int)rend_list->poly_ptrs[poly]->tvlist[1].x, (int)rend_list->poly_ptrs[poly]->tvlist[1].y);
		LineTo(hdc, (int)rend_list->poly_ptrs[poly]->tvlist[2].x, (int)rend_list->poly_ptrs[poly]->tvlist[2].y);
		LineTo(hdc, (int)rend_list->poly_ptrs[poly]->tvlist[0].x, (int)rend_list->poly_ptrs[poly]->tvlist[0].y);

	}
}




