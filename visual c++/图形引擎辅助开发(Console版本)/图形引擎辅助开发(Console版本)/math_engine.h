#ifndef _MATH_ENGINE_H_
#define _MATH_ENGINE_H_
#include <string.h>
#include <math.h>



//向量和点的结构体定义
//2d 3d 四元数
typedef struct VECTOR2D_TYP{
	union{
		float M[2];
		struct{
			float x, y;
		};
	};
}VECTOR2D, POINT2D, *VECTOR2D_PTR, *POINT2D_PTR;
typedef struct VECTOR3D_TYP{
	union{
		float M[3];
		struct{
			float x, y, z;
		};
	};
}VECTOR3D, POINT3D, *VECTOR3D_PTR, *POINT3D_PTR;

typedef struct VECTOR4D_TYP{
	union{
		float M[4];
		struct{
			float x, y, z, w;
		};
	};
}VECTOR4D, POINT4D, *VECTOR4D_PTR, *POINT4D_PTR;

//顶点定义
typedef struct VERTEX2DI_TYP{
	int x, y;
}VERTEXT2DI, *VERTEX2DI_PTR;
typedef struct VERTEX2DF_TYP{
	float x, y;
}VERTEX2DF, *VERTEX2DF_PTR;
typedef struct VERTEX3DI_TYP{
	int x, y, z;
}VERTEX3DI, *VERTEX3DI_PTR;
typedef struct VERTEX3DF_TYP{
	float x, y, z;
}VERTEX3DF, *VERTEX3DF_PTR;

//2d参数化直线
typedef struct PARMLINE2D_TYP{
	POINT2D p0;
	POINT2D p1;
	VECTOR2D v;			//方向向量
						//|v| = |p0|-|p1|
}PARMLINE2D, *PARMLINE2D_PTR;
typedef struct PARMLINE3D_TYP{
	POINT3D p0;
	POINT3D p1;
	VECTOR3D v;			//方向向量
						//同2d直线结构体
}PARMLINE3D, *PARMLINE3D_PTR;
typedef struct PLANE3D_TYP{
	POINT3D p0;			//平面上的一个点
	VECTOR3D n;			//平面的法线
	//点法式方程
	//a(x-x0)+b(y-y0)+c(z-z0)=0
	//n.x(x-p0.x)+n.y(y-p0.y)+n.z(z-p0.z)=0
}PLANE3D, *PLANE3D_PTR;


//矩阵结构体
typedef struct MATRIX4X4_TYP{	//4X4
	union{
		float M[4][4];
		struct{
			float M00, M01, M02, M03;
			float M10, M11, M12, M13;
			float M20, M21, M22, M23;
			float M30, M31, M32, M33;
		};
	};
}MATRIX4X4, *MATRIX4X4_PTR;
typedef struct MATRIX4X3_TYP{	//4X3
	union{
		float M[4][3];
		struct{
			float M00, M01, M02;
			float M10, M11, M12;
			float M20, M21, M22;
			float M30, M31, M32;
		};
	};
}MATRIX4X3, *MATRIX4X3_PTR;
typedef struct MATRIX1X4_TYP{	//1X4
	float M[4];
	struct{
		float M00, M01, M02, M03;
	};
}MATRIX1X4, *MATRIX1X4_PTR;
typedef struct MATRIX3X3_TYP{	//3X3
	float M[3][3];
	struct{
		float M00, M01, M02;
		float M10, M11, M12;
		float M20, M21, M22;
	};
}MATRIX3X3, *MATRIX3X3_PTR;
typedef struct MATRIX1X3_TYP{	//1X3
	union{
		float M[3];
		struct {
			float M00, M01, M02;
		};
	};
}MATRIX1X3, *MATRIX1X3_PTR;
typedef struct MATRIX3X2_TYP{	//3X2
	union{
		float M[3][2];
		struct{
			float M00, M01;
			float M10, M11;
			float M20, M21;
		};
	};
}MATRIX3X2, *MATRIX3X2_PTR;
typedef struct MATRIX2X2_TYP{	//2X2
	union{
		float M[2][2];
		struct{
			float M00, M01;
			float M10, M11;
		};
	};
}MATRIX2X2, *MATRIX2X2_PTR;
typedef struct MATRIX1X2_TYP{	//1X2
	union{
		float M[2];
		struct{
			float M00, M01;
		};
	};
}MATRIX1X2, *MATRIX1X2_PTR;

//四元数定义
typedef struct QUAT_TYP{
	union{
		float M[4];		//w, x, y, z
		struct{
			float q0;		//实部
			VECTOR3D qv;	//虚部	
		};
		struct{
			float w, x, y, z;
		};
	};
}QUAT, *QUAT_PTR;
//2D极坐标
typedef struct POLAR2D_TYP{
	float r;		//半径
	float theta;	//角度
}POLAR2D, *POLAR2D_PTR;
typedef struct CYLINDRICAL3D_TYP{
	float r;		//半径
	float theta;	//与z轴夹角
	float z;		//z坐标
}CYLINDRICAL3D, *CYLINDRICAL3D_PTR;
typedef struct SPHERICAL3D_TYP{
	float p;		//到原点的距离
	float theta;	//z 与正z轴之间的夹角
	float phi;		//
}SPHERICAL3D, *SPHERICAL3D_PTR;
typedef int FIXP16;
typedef int *FIXP16_PTR;

//与PI有关的常量
#define PI ((float)3.141592654f)
#define PI2 ((float)6.283185307f)
#define PI_DIV_2 ((float)1.570796327f)
#define PI_DIV_4 ((float)0.785398163f)
#define PI_INV ((float)0.318309886f)
//与定点数有关的常量
#define FIXP16_SHIFT 16
#define FIXP16_MAG 65536
#define FIXP16_DP_MASK 0x0000ffff
#define FIXP16_WP_MASK 0xffff0000
#define FIXP16_ROUND_UP 0x00008000
//非常小的数的常量
#define EPSILON_E4 (float)(1E-4)
#define EPSILON_E5 (float)(1E-5)
#define EPSILON_E6 (float)(1E-6)
//用户参数化直线交点的常量
#define PARM_LINE_NO_INTERSECT 0
#define PARM_LINE_INTERSECT_IN_SEGMENT 1
#define PARM_LINE_INTERSECT_OUT_SEGMENT 2
#define PARM_LINE_INTERSECT_EVERYWHERE 3
//单位矩阵
const MATRIX4X4 IMAT_4X4 = { 
	1, 0, 0, 0,
	0, 1, 0, 0,	
	0, 0, 1, 0,
	0, 0, 0, 1 };
const MATRIX4X3 IMAT_4X3 = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1,
	0, 0, 0
};
const MATRIX3X3 IMAT_3X3 = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
};
const MATRIX2X2 IMAT_2X2 = {
	1, 0,
	0, 1
};
//通用宏
#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#define MAX(a, b) (((a)>(b)) ? (a) : (b))

#define SWAP(a, b, t) {t = a; a = b; b = t;}
#define DEG_TO_RAD(ang) ((ang)*PI/180.0)
#define RAD_TO_DEG(radis) ((rads)*180/PI)

//点和向量宏
inline void VECTOR2D_ZERO(VECTOR2D_PTR v){
	(v)->x = (v)->y = 0.0;
}
inline void VECTOR3D_ZERO(VECTOR3D_PTR v){
	(v)->x = (v)->y = (v)->z = 0;
}
inline void VECTOR4D_ZERO(VECTOR4D_PTR v){
	(v)->x = (v)->y = (v)->z = (v)->w = 0;
}
inline void VECTOR2D_INITXY(VECTOR2D_PTR v, float x, float y){
	(v)->x = (x); (v)->y = (y);
}
inline void VECTOR3D_INITXYZ(VECTOR3D_PTR v, float x, float y, float z){
	(v)->x = (x); (v)->y = (y); (v)->z = z;
}
inline void VECTOR4D_INITXYZ(VECTOR4D_PTR v, float x, float y, float z){
	(v)->x = (x); (v)->y = (y); (v)->z = (z); (v)->w = 1.0;
}
inline void VECTOR2D_INIT(VECTOR2D_PTR vdst, VECTOR2D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
}
inline void VECTRO3D_INIT(VECTOR3D_PTR vdst, VECTOR3D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y; (vdst)->z = (vsrc)->z;
}
inline void VECTOR4D_INIT(VECTOR4D_PTR vdst, VECTOR4D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
	(vdst)->z = (vsrc)->z; (vdst)->w = (vsrc)->w;
}
//复制向量的宏
inline void VECTOR2D_COPY(VECTOR2D_PTR vdst, VECTOR2D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
}
inline void VECTOR3D_COPY(VECTOR3D_PTR vdst, VECTOR3D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y; (vdst)->z = (vsrc)->z;
}
inline void VECTOR4D_COPY(VECTOR4D_PTR vdst, VECTOR4D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
	(vdst)->z = (vsrc)->z; (vdst)->w = (vdst)->w;
}
//初始化点的宏
inline void POINT2D_INIT(POINT2D_PTR vdst, POINT2D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
}
inline void POINT3D_INIT(POINT3D_PTR vdst, POINT3D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y; (vdst)->z = (vsrc)->z;
}
inline void POINT4D_INIT(POINT4D_PTR vdst, POINT4D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
	(vdst)->z = (vsrc)->z; (vdst)->w = (vsrc)->w;
}
//复制点的宏
inline void POINT2D_COPY(POINT2D_PTR vdst, POINT2D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
}
inline void POINT3D_COPY(POINT3D_PTR vdst, POINT3D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y; (vdst)->z = (vsrc)->z;
}
inline void POINT4D_COPY(POINT4D_PTR vdst, POINT4D_PTR vsrc){
	(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
	(vdst)->z = (vsrc)->z; (vdst)->w = (vsrc)->w;
}
//矩阵宏
#define MAT_ZERO_2X2(m) {memset((void*)(m), 0, sizeof(MATRIX2X2));}
#define MAT_ZERO_3X3(m) {memset((void*)(m), 0, sizeof(MATRIX3X3));}
#define MAT_ZERO_4X4(m) {memset((void*)(m), 0, sizeof(MATRIX4X3));}
#define MAT_ZERO_4X3(m) {memset((void*)(m), 0, sizeof(MATRIX4X3));}
//设置单位矩阵的宏
#define MAT_IDENTITY_2X2(m) {memcpy((void*)(m), (void*)&IMAT_2X2, sizeof(MATRIX2X2));}
#define MAT_IDENTITY_3X3(m) {memcpy((void*)(m), (void*)&IMAT_3X3, sizeof(MATRIX3X3));}
#define MAT_IDENTITY_4X4(m) {memcpy((void*)(m), (void*)&IMAT_4X4, sizeof(MATRIX4X4));}
#define MAT_IDENTITY_4X3(m) {memcpy((void*)(m), (void*)&IMAT_4X3, sizeof(MATRIX4X3));}
//复制矩阵的宏
#define MAT_COPY_2X2(src_mat, dest_mat) {memcpy((void*)(dest_mat), (void*)(src_mat), sizeof(MATRIX_2X2);}
#define MAT_COPY_3X3(src_mat, dest_mat) {memcpy((void*)(dest_mat), (void*)(src_mat), sizeof(MATRIX_3X3);}
#define MAT_COPY_4X4(src_mat, dest_mat) {memcpy((void*)(dest_mat), (void*)(src_mat), sizeof(MATRIX_4X4);}
#define MAT_COPY_4X3(src_mat, dest_mat) {memcpy((void*)(dest_mat), (void*)(src_mat), sizeof(MATRIX_4X3);}
//对矩阵进行转置的宏
inline void MAT_TRANSPOSE_3X3(MATRIX3X3_PTR m){
	MATRIX3X3 mt;
	mt.M00 = m->M00; mt.M01 = m->M10; mt.M02 = m->M20;
	mt.M10 = m->M01; mt.M11 = m->M11; mt.M12 = m->M21;
	mt.M20 = m->M02; mt.M21 = m->M12; mt.M22 = m->M22;
	memcpy((void*)m, (void*)&mt, sizeof(MATRIX3X3));
}
inline void MAT_TRANSPOSE_4X4(MATRIX4X4_PTR m){
	MATRIX4X4 mt;
	mt.M00 = m->M00; mt.M01 = m->M10; mt.M02 = m->M20; mt.M03 = m->M30;
	mt.M10 = m->M01; mt.M11 = m->M11; mt.M12 = m->M21; mt.M13 = m->M31;
	mt.M20 = m->M02; mt.M21 = m->M21; mt.M22 = m->M22; mt.M23 = m->M32;
	mt.M30 = m->M03; mt.M31 = m->M13; mt.M32 = m->M23; mt.M33 = m->M33;
	memcpy((void*)m, (void*)&mt, sizeof(MATRIX4X4));
}
//矩阵转置，将m转置到mt
inline void MAT_TRANSPOSE_3X3(MATRIX3X3_PTR m, MATRIX3X3_PTR mt){
	mt->M00 = m->M00; mt->M01 = m->M10; mt->M02 = m->M20;
	mt->M10 = m->M01; mt->M11 = m->M11; mt->M12 = m->M21;
	mt->M20 = m->M02; mt->M21 = m->M12; mt->M22 = m->M22;
}
inline void MAT_TRANSPOSE_4X4(MATRIX4X4_PTR m, MATRIX4X4_PTR mt){
	mt->M00 = m->M00; mt->M01 = m->M10; mt->M02 = m->M20;
	mt->M10 = m->M01; mt->M11 = m->M11; mt->M12 = m->M21;
	mt->M20 = m->M02; mt->M21 = m->M12; mt->M22 = m->M22;
	mt->M30 = m->M03; mt->M31 = m->M13; mt->M32 = m->M23; mt->M33 = m->M33;
}

//小型内联函数
inline void MAT_COLUMN_SWAP_2X2(MATRIX2X2_PTR m, int c, MATRIX1X2_PTR v){
	m->M[0][c] = v->M[0]; m->M[1][c] = v->M[1];
}
inline void MAT_COLUMN_SWAP_3X3(MATRIX3X3_PTR m, int c, MATRIX1X3_PTR v){
	m->M[0][c] = v->M[0]; m->M[1][c] = v->M[1]; m->M[2][c] = v->M[3];
}
inline void MAT_COLUMN_SWAP_4X4(MATRIX4X4_PTR m, int c, MATRIX1X3_PTR v){
	m->M[0][c] = v->M[0]; m->M[1][c] = v->M[1]; m->M[2][c] = v->M[3]; m->M[3][c] = v->M[3];
}
inline void MAT_COLUMN_SWAP_4X3(MATRIX4X3_PTR m, int c, MATRIX1X3_PTR v){
	m->M[0][c] = v->M[0]; m->M[1][c] = v->M[1]; m->M[2][c] = v->M[2]; m->M[0][c] = v->M[3];
}
//四元数宏
inline void QUAT_ZERO(QUAT_PTR q){
	(q)->x = (q)->y = (q)->z = (q)->w = 0.0;
}
inline void QUAT_INITWXYZ(QUAT_PTR q, float w, float x, float y, float z){
	(q)->w = (w); (q)->x = (x); (q)->y = (y); (q)->z = (z);
}
inline void QUAT_INITVECTOR3D(QUAT_PTR q, VECTOR3D_PTR v){
	(q)->w = 0; (q)->x = (v->x); (q)->y = (v->y); (q)->z = (v->z);
}
inline void QUAT_INIT(QUAT_PTR qdst, QUAT_PTR qsrc){
	(qdst)->w = (qsrc)->w; (qdst)->x = (qsrc)->x;
	(qdst)->y = (qsrc)->y; (qsrc)->z = (qsrc)->z;
}
inline void QUAT_COPY(QUAT_PTR qdst, QUAT_PTR qsrc){
	(qdst)->x = (qsrc)->x; (qdst)->y = (qsrc)->y;
	(qdst)->z = (qsrc)->z; (qdst)->w = (qsrc)->w;
}
//定点数宏
#define FIXP16_WP(fp) ((fp)>>FIXP16_SHIFT)
#define FIXP16_DP(fp) ((fp)&FIXP16_DP_MASK)
#define INT_TO _FIXP16(i) ((i)<<FIXP16_SHIFT)
#define FLOAT_TO_FIXP16(f) (((float)(f))*(float)FIXP16_MAG + 0.5)
#define FIXP16_TO_FLOAT(fp) (((float)fp)/FIX16_MAG)

//函数定义
//三角函数
float Fast_Sin(float theta);
float Fast_Cos(float theta);
//距离函数
int Fast_Distance_2D(int x, int y);
float Fast_Distance_3D(float x, float y, float z);
//极坐标，柱面坐标和球面坐标函数
void POLAR2D_To_POINT2D(POLAR2D_PTR polar, POINT2D_PTR rect);
void POLAR2D_To_RectXY(POLAR2D_PTR polar, float *x, float *y);
void POINT2D_To_POLAR2D(POINT2D_PTR rect, POLAR2D_PTR polar);
void POINT2D_To_PolarRTh(POINT2D_PTR rect, float *r, float *theta);
void CYLINDRICAL3D_To_POINT3D(CYLINDRICAL3D_PTR cyl, POINT3D_PTR rect);
void CYLINDRICAL3D_To_RectXYZ(CYLINDRICAL3D_PTR cyl, float *x, float *y, float *z);
void POINT3D_To_CYLINDRICAL3D(POINT3D_PTR rect, CYLINDRICAL3D_PTR cyl);
void POINT3D_To_CYLINDRICAL3D(POINT3D_PTR rect, float *r, float *theta, float *z);

void SPHERICAL3D_To_POINT3D(SPHERICAL3D_PTR sph, POINT3D_PTR rect);
void SPHERICAL3D_To_RectXYZ(SPHERICAL3D_PTR sph, float *x, float *y, float *z);
void POINT3D_To_SPHERICAL3D(POINT3D_PTR rect, SPHERICAL3D_PTR sph);
void POINT3D_To_SPHERICALPThPh(POINT3D_PTR rect, float *p, float *theta, float *phi);

//2D向量函数
void VECTOR2D_Add(VECTOR2D_PTR va, VECTOR2D_PTR vb, VECTOR2D_PTR vsum);
VECTOR2D VECTOR2D_Add(VECTOR2D_PTR va, VECTOR2D_PTR vb);
void VECTOR2D_Sub(VECTOR2D_PTR va, VECTOR2D_PTR vb, VECTOR2D_PTR vidff);
VECTOR2D VECTOR2D_Sub(VECTOR2D_PTR va, VECTOR2D_PTR vb);
void VECTOR2D_Scale(float k, VECTOR2D_PTR va);
void VECTOR2D_Scale(float k, VECTOR2D_PTR va, VECTOR2D_PTR vscaled);
float VECTOR2D_Dot(VECTOR3D_PTR va, VECTOR2D_PTR vb);
float VECTOR2D_Length(VECTOR2D_PTR va);
float VECTOR2D_Length_Fast(VECTOR2D_PTR va);
void VECTOR2D_Normalize(VECTOR2D_PTR va);
void VECTOR2D_Normalize(VECTOR2D_PTR va, VECTOR2D_PTR vn);
void VECTOR2D_Build(VECTOR2D_PTR init, VECTOR2D_PTR term, VECTOR2D_PTR result);
float VECTOR2D_CosTh(VECTOR2D_PTR va, VECTOR2D_PTR vb);
void VECTOR2D_Print(VECTOR2D_PTR va, char *name);
//3D向量函数
void VECTOR3D_Add(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vsum);
VECTOR3D VECTOR3D_Add(VECTOR3D_PTR va, VECTOR3D_PTR vb);
void VECTOR3D_Sub(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vsum);
VECTOR3D VECTOR3D_Sub(VECTOR3D_PTR va, VECTOR3D_PTR vb);
void VECTOR3D_Scale(float k, VECTOR3D_PTR va);
void VECTOR3D_Scale(float k, VECTOR3D_PTR va, VECTOR3D_PTR vscaled);
float VECTOR3D_Dot(VECTOR3D_PTR va, VECTOR3D_PTR vb);
void VECTOR3D_Cross(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vn);
VECTOR3D VECTOR3D_Cross(VECTOR3D_PTR va, VECTOR3D_PTR vb);
float VECTOR3D_Length(VECTOR3D_PTR va);
float VECTOR3D_Length_Fast(VECTOR3D_PTR va);
VECTOR3D VECTOR3D_Normalize(VECTOR3D_PTR va);
void VECTOR3D_Normalize(VECTOR3D_PTR va, VECTOR3D_PTR vn);
void VECTOR3D_Build(VECTOR3D_PTR init, VECTOR3D_PTR term, VECTOR3D_PTR result);
float VECTRO3D_CosTh(VECTOR3D_PTR va, VECTOR3D_PTR vb);
void VECTOR3D_Print(VECTOR3D_PTR va, char *name);
//4D向量函数
void VECTOR4D_Add(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vsum);
VECTOR4D VECTOR4D_Add(VECTOR4D_PTR va, VECTOR4D_PTR vb);
void VECTOR4D_Sub(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vdiff);
VECTOR4D VECTOR4D_Sub(VECTOR4D_PTR va, VECTOR4D_PTR vb);
void VECTOR4D_Scale(float k, VECTOR4D_PTR va);
void VECTOR4D_Scale(float k, VECTOR4D_PTR va, VECTOR4D_PTR vscaled);
float VECTOR4D_Dot(VECTOR4D_PTR va, VECTOR4D_PTR vb);
void VECTOR4D_Cross(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vn);
VECTOR4D VECTOR4D_Cross(VECTOR4D_PTR va, VECTOR4D_PTR vb);
float VECTOR4D_Length(VECTOR4D_PTR va);
float VECTOR4D_Length_Fast(VECTOR4D_PTR va);
void VECTOR4D_Normalize(VECTOR4D_PTR va);
void VECTOR4D_Normalize(VECTOR4D_PTR va, VECTOR4D_PTR vn);
void VECTOR4D_Build(VECTOR4D_PTR init, VECTOR4D_PTR term, VECTOR4D_PTR result);
float VECTOR4D_CosTh(VECTOR4D_PTR va, VECTOR4D_PTR vb);
void VECTOR4D_Print(VECTOR4D_PTR va, char *name);

//2X2矩阵函数
void Mat_Init_2X2(MATRIX2X2_PTR ma, float m00, float m01, float m10, float m11);
void Print_Mat_2X2(MATRIX2X2_PTR ma, char *name);
float Mat_Det_2X2(MATRIX2X2_PTR m);
void Mat_Add_2X2(MATRIX2X2_PTR ma, MATRIX2X2_PTR mb, MATRIX2X2_PTR msum);
void Mat_Mul_2X2(MATRIX2X2_PTR ma, MATRIX2X2_PTR mb, MATRIX2X2_PTR mprod);
int Mat_Inverse_2X2(MATRIX2X2_PTR m, MATRIX2X2_PTR mi);
int Solve_2X2_System(MATRIX2X2_PTR A, MATRIX1X2_PTR X, MATRIX1X2_PTR B);
//3X3矩阵函数
int Mat_Mul_1X2_2X2(MATRIX1X2_PTR ma, MATRIX2X2_PTR mb, MATRIX1X2_PTR mprod);
int Mat_Mul_1X3_3X3(MATRIX1X3_PTR ma, MATRIX3X3_PTR mb, MATRIX1X3_PTR mprod);
int Mat_Mul_3X3(MATRIX3X3_PTR ma, MATRIX3X3_PTR mb, MATRIX3X3_PTR mprod);

inline int Mat_Init_3X2(MATRIX3X2_PTR ma, 
	float m00, float m01,
	float m10, float m11,
	float m20, float m21);
void Mat_Add_3X3(MATRIX3X3_PTR ma, MATRIX3X3_PTR mb, MATRIX3X3_PTR mprod);
void Mat_Mul_VECTOR3D_3X3(VECTOR3D_PTR va, MATRIX3X3_PTR mb, VECTOR3D_PTR vprod);
int Mat_Invarse_3X3(MATRIX3X3_PTR m, MATRIX3X3_PTR mi);
void Mat_Init_3X3(MATRIX3X3_PTR ma,
	float m00, float m01, float m02,
	float m10, float m11, float m12,
	float m20, float m21, float m22);
void Print_Mat_3X3(MATRIX3X3_PTR ma, char *name);
float Mat_Det_3X3(MATRIX3X3_PTR m);
int Solve_3X3_System(MATRIX3X3_PTR A, MATRIX1X3_PTR X, MATRIX1X3_PTR B);

//4X4矩阵函数
void Mat_Add_4X4(MATRIX4X4_PTR ma, MATRIX4X4_PTR mb, MATRIX4X4_PTR msum);
void Mat_Mul_4X4(MATRIX4X4_PTR ma, MATRIX4X4_PTR mb, MATRIX4X4_PTR mprod);
void Mat_Mul_1X4_4X4(MATRIX1X4_PTR ma, MATRIX4X4_PTR mb, MATRIX1X4_PTR mrpod);
void Mat_Mul_VECTOR3D_4X4(VECTOR3D_PTR va, MATRIX4X4_PTR mb, VECTOR3D_PTR vprod);
void Mat_Mul_VECTOR3D_4X3(VECTOR3D_PTR va, MATRIX4X3_PTR mb, VECTOR3D_PTR vprod);
void Mat_Mul_VECTOR4D_4X4(VECTOR4D_PTR va, MATRIX4X4_PTR mb, VECTOR4D_PTR vprod);
void Mat_Mul_Vector4d_4X3(VECTOR4D_PTR va, MATRIX4X3_PTR mb, VECTOR4D_PTR vprod);
int Mat_Inverse_4X4(MATRIX4X4_PTR m, MATRIX4X4_PTR mi);
void Mat_Init_4X4(MATRIX4X4_PTR ma,
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33
	);
void Print_Mat_4X4(MATRIX4X4_PTR ma, char *name);

//四元数函数
void QUAT_Add(QUAT_PTR q1, QUAT_PTR q2, QUAT_PTR qaum);
void QUAT_Sub(QUAT_PTR q1, QUAT_PTR q2, QUAT_PTR qdiff);
void QUAT_Conjugate(QUAT_PTR q, QUAT_PTR qconj);
void QUAT_Scale(QUAT_PTR q, float scale, QUAT_PTR qs);
void QUAT_Scale(QUAT_PTR q, float scale);
float QUAT_Norm(QUAT_PTR q);
float QUAT_Norm2(QUAT_PTR q); 
void QUAT_Normalize(QUAT_PTR q, QUAT_PTR qn);
void QUAT_Normalize(QUAT_PTR q);
void QUAT_Unit_Inverse(QUAT_PTR q, QUAT_PTR qi);
void QUAT_Unit_Inverse(QUAT_PTR q);
void QUAT_Nul(QUAT_PTR q1, QUAT_PTR q2, QUAT_PTR qprod);
void QUAT_Triple_Product(QUAT_PTR q1, QUAT_PTR q2, QUAT_PTR q3, QUAT_PTR qprod);

void VECTOR3D_Theta_To_QUAT(QUAT_PTR q, VECTOR3D_PTR v, float theta);
void VECTOR4D_Theta_To_QUAT(QUAT_PTR q, VECTOR4D_PTR v, float theta);
void EulerZYX_To_QUAT(QUAT_PTR q, float theta_z, float theta_y, float theta_x);
void QUAT_To_VECTOR3D_Theta(QUAT_PTR q, VECTOR3D_PTR v, float  *theta);
void QUAT_Print(QUAT_PTR q, char *name);
//2D参数化直线函数
void Init_ParmLine2D(POINT2D_PTR p_init, POINT2D_PTR p_term, PARMLINE2D_PTR p);
void Compute_Parm_Line2D(PARMLINE2D_PTR p, float t, POINT2D_PTR pt);
int Intersect_Parm_Lines2D(PARMLINE2D_PTR p1, PARMLINE2D_PTR p2, float *t1, float *t2);
int Intersect_Parm_Lines2D(PARMLINE2D_PTR p1, PARMLINE2D_PTR p2, POINT2D_PTR pt);
//3D参数化直线函数
void Init_Parm_Line3D(POINT3D_PTR p_init, POINT3D_PTR p_term);
void Compute_Parm_Line3D(PARMLINE3D_PTR p, float t, POINT3D_PTR pt);
//3D平面函数
void PLANE3D_Init(PLANE3D_PTR plane, POINT3D_PTR p0,
	VECTOR3D_PTR normal, int normalize);
float Compute_Point_In_Plane3D(POINT3D_PTR pt, PLANE3D_PTR plane);
int Intersect_Parm_Line3D_Plane3D(PARMLINE3D_PTR pline, PLANE3D_PTR plane, float *t, POINT3D_PTR pt);
//顶点数函数
FIXP16 FIXP16_NUL(FIXP16 fp1, FIXP16 fp2);
FIXP16 FIXP16_DIV(FIXP16 fp1, FIXP16 fp2);
void FIXP16_Print(FIXP16 fp);




//辅助定义

void Build_Sin_Cos_Tables();


//------------------
#endif