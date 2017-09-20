#include "math_engine.h"

//辅助定义
float cos_look[361];
float sin_look[361];

void Build_Sin_Cos_Tables(){
	int i;
	for (i = 0; i <= 360; i++)
	{
		cos_look[i] = (float)cos(DEG_TO_RAD(i));
		sin_look[i] = (float)sin(DEG_TO_RAD(i));
	}
}

//三角函数
float Fast_Sin(float theta){
	//使用查表sin_look[]
	theta = fmodf(theta, 360);
	if (theta < 0)
		theta += 360.0;
	int theta_int = (int)theta;
	float theta_frac = theta - theta_int;
	return (sin_look[theta_int] + theta_frac*(sin_look[theta_int + 1] - sin_look[theta_int]));
}
float Fast_Cos(float theta){
	theta = fmodf(theta, 360);
	if (theta < 0)
		theta += 360.0;
	int theta_int = (int)theta;
	float theta_frac = theta - theta_int;
	return (cos_look[theta_int] + theta_frac *(cos_look[theta_int + 1] - sin_look[theta_int]));
}
//距离函数
int Fast_Distance_2D(int x, int y){
	x = abs(x);
	y = abs(y);
	int mm = MIN(x, y);
	return (x + y - (mm >> 1) - (mm >> 2) + (mm >> 4) - 1);
	//return (int)(sqrt((double)(x*x + y*y)));
}
float Fast_Distance_3D(float fx, float fy, float fz){
	int temp;
	int x, y, z;
	x = (int)(fabs(fx) * 1024);
	y = (int)(fabs(fy) * 1024);
	z = (int)(fabs(fz) * 1024);
	if (y < x) SWAP(x, y, temp);
	if (z < y) SWAP(y, z, temp);
	if (y < x) SWAP(x, y, temp);
	int dist = (z + 11 * (y >> 5) + (x >> 2));
	return ((float)(dist >> 10));
	//return (float)sqrt((double)(x*x + y*y + z*z));
}
//极坐标，柱面坐标和球面坐标函数
void POLAR2D_To_POINT2D(POLAR2D_PTR polar, POINT2D_PTR rect){
	rect->x = polar->r * cos(polar->theta);
	rect->y = polar->r * sin(polar->theta);
}
void POLAR2D_To_RectXY(POLAR2D_PTR polar, float *x, float *y){
	*x = polar->r * cos(polar->theta);
	*y = polar->r * sin(polar->theta);
}
void POINT2D_To_POLAR2D(POINT2D_PTR rect, POLAR2D_PTR polar){
	polar->r = (float)sqrt(rect->x * rect->x + rect->y * rect->y);
	if (rect->x == 0)
	{
		if (rect->y > 0)
			polar->theta = PI_DIV_2;
		else if (rect->y < 0)
			polar->theta = -PI_DIV_2;
		else
			polar->theta = 0;
	}
	else if (rect->y == 0)
	{
		if (rect->x > 0)
			polar->theta = 0;
		else if (rect->x < 0)
			polar->theta = PI;
		else
			polar->theta = 0;
	}
	else
	{
		polar->theta = (float)atan((double)(rect->y * 1.0 / rect->x * 1.0));
	}
}

void POINT2D_To_PolarRTh(POINT2D_PTR rect, float *r, float *theta){
	if (rect == NULL)
		return;
	if (r != NULL)
		*r = (float)sqrt(rect->x * rect->x + rect->y * rect->y);
	if (theta == NULL)
		return;
	if (rect->x == 0)
	{
		if (rect->y > 0)
			*theta = PI_DIV_2;
		else if (rect->y < 0)
			*theta = -PI_DIV_2;
		else
			*theta = 0;
	}
	else if (rect->y == 0)
	{
		if (rect->x > 0)
			*theta = 0;
		else if (rect->x < 0)
			*theta = PI;
		else
			*theta = 0;
	}
	else
	{
		*theta = (float)atan((double)(rect->y * 1.0 / rect->x * 1.0));
	}
}

void CYLINDRICAL3D_To_POINT3D(CYLINDRICAL3D_PTR cyl, POINT3D_PTR rect){
	if (cyl == NULL || rect == NULL)
		return;
	rect->x = cyl->r * (float)cos((double)cyl->theta);
	rect->y = cyl->r * (float)sin((double)cyl->theta);
	rect->z = cyl->z;
}
void CYLINDRICAL3D_To_RectXYZ(CYLINDRICAL3D_PTR cyl, float *x, float *y, float *z){
	if (cyl == NULL)
		return;
	if (x != NULL)
		*x = cyl->r * (float)cos((double)cyl->theta);
	if (y != NULL)
		*y = cyl->r * (float)sin((double)cyl->theta);
	if (z != NULL)
		*z = cyl->z;
}
void POINT3D_To_CYLINDRICAL3D(POINT3D_PTR rect, CYLINDRICAL3D_PTR cyl){
	if (rect == NULL || cyl == NULL)
		return;
	cyl->r = (float)sqrt((double)(rect->x * rect->x + rect->y + rect->y));
	cyl->z = cyl->z;

	if (rect->x == 0)
	{
		if (rect->y > 0)
			cyl->theta = PI_DIV_2;
		else if (rect->y < 0)
			cyl->theta = -PI_DIV_2;
		else
			cyl->theta = 0;
	}
	else if (rect->y == 0)
	{
		if (rect->x > 0)
			cyl->theta = 0;
		else if (rect->x < 0)
			cyl->theta = PI;
		else
			cyl->theta = 0;
	}
	else
	{
		cyl->theta = (float)atan((double)(rect->y * 1.0 / rect->x * 1.0));
	}
}
//3D直角坐标系转换成圆柱坐标系
void POINT3D_To_CYLINDRICAL3D(POINT3D_PTR rect, float *r, float *theta, float *z){
	if (rect == NULL)
		return;
	if (r != NULL)
		*r = (float)sqrt((double)(rect->x * rect->x + rect->y + rect->y));
	if (z != NULL)
		*z = rect->z;
	if (theta == NULL)
		return;

	if (rect->x == 0)
	{
		if (rect->y > 0)
			*theta = PI_DIV_2;
		else if (rect->y < 0)
			*theta = -PI_DIV_2;
		else
			*theta = 0;
	}
	else if (rect->y == 0)
	{
		if (rect->x > 0)
			*theta = 0;
		else if (rect->x < 0)
			*theta = PI;
		else
			*theta = 0;
	}
	else
	{
		*theta = (float)atan((double)(rect->y * 1.0 / rect->x * 1.0));
	}
}
//将球体坐标系转换成笛卡尔坐标系
void SPHERICAL3D_To_POINT3D(SPHERICAL3D_PTR sph, POINT3D_PTR rect){
	if (sph == NULL || rect == NULL)
		return;
	rect->x = sph->p * (float)cos((double)sph->phi) * (float)cos((double)sph->theta);
	rect->y = sph->p * (float)cos((double)sph->phi) * (float)sin((double)sph->theta);
	rect->z = sph->p * (float)sin((double)sph->phi);
}
//上同，只不过输出每一个分量，而不是作为一个结构体进行输出
void SPHERICAL3D_To_RectXYZ(SPHERICAL3D_PTR sph, float *x, float *y, float *z){
	if (sph == NULL)
		return;
	if (x != NULL)
		*x = sph->p * (float)cos((double)sph->phi) * (float)cos((double)sph->theta);
	if (y != NULL)
		*y = sph->p * (float)cos((double)sph->phi) * (float)sin((double)sph->theta);
	if (z != NULL)
		*z = sph->p * (float)sin((double)sph->phi);
}
//将笛卡尔坐标系下的坐标转化成球体坐标系下的坐标
void POINT3D_To_SPHERICAL3D(POINT3D_PTR rect, SPHERICAL3D_PTR sph){
	if (rect == NULL || sph == NULL)
		return;
	sph->p = (float)sqrt((double)(rect->x * rect->x + rect->y * rect->y + rect->z * rect->z));
	sph->phi = (float)asin((double)(rect->z / sph->p));
	sph->theta = (float)atan((double)(rect->y / rect->x));
}
//上同
void POINT3D_To_SPHERICALPThPh(POINT3D_PTR rect, float *p, float *theta, float *phi){
	if (rect == NULL)
		return;
	float result_p = (float)sqrt((double)(rect->x * rect->x + rect->y * rect->y + rect->z * rect->z));
	if (p != NULL)
		*p = result_p;
	if (theta != NULL)
		*theta = (float)atan((double)(rect->y / rect->x));
	if (phi != NULL)
		*phi = (float)asin((double(rect->z / result_p)));
}

//2D向量函数
void VECTRO2D_Add(VECTOR2D_PTR va, VECTOR2D_PTR vb, VECTOR2D_PTR vsum){
	if (va == NULL || vb == NULL || vsum == NULL)
		return;
	vsum->x = va->x + vb->x;
	vsum->y = va->y + vb->y;
}
VECTOR2D VECTOR2D_Add(VECTOR2D_PTR va, VECTOR2D_PTR vb){
	VECTOR2D result = { 0, 0 };
	if (va == NULL || vb == NULL)
		return result;
	result.x = va->x + vb->x;
	result.y = va->y + vb->y;
	return result;
}
void VECTOR2D_Sub(VECTOR2D_PTR va, VECTOR2D_PTR vb, VECTOR2D_PTR vidff){
	if (va == NULL || vb == NULL || vidff == NULL)
		return;
	vidff->x = va->x - vb->x;
	vidff->y = va->y - vb->y;
}
VECTOR2D VECTOR2D_Sub(VECTOR2D_PTR va, VECTOR2D_PTR vb){
	VECTOR2D result = { 0, 0 };
	if (va == NULL || vb == NULL)
		return result;
	result.x = va->x - vb->x;
	result.y = va->y - vb->y;
	return result;
}
void VECTOR2D_Scale(float k, VECTOR2D_PTR va){
	if (va == NULL)
		return;
	va->x *= k;
	va->y *= k;
}
void VECTOR2D_Scale(float k, VECTOR2D_PTR va, VECTOR2D_PTR vscaled){
	vscaled->x = va->x * k;
	vscaled->y = va->y * k;
}
float VECTOR2D_Dot(VECTOR3D_PTR va, VECTOR2D_PTR vb){
	if (va == NULL || vb == NULL)
		return 0.0f;
	return va->x * vb->x + va->y * vb->y;
}
float VECTOR2D_Length(VECTOR2D_PTR va){
	if (va == NULL)
		return 0.0f;
	return (float)sqrt((double)(va->x * va->x + va->y * va->y));
}
float VECTOR2D_Length_Fast(VECTOR2D_PTR va){
	if (va == NULL)
		return 0.0f;

	return((float)Fast_Distance_2D((int)va->x, (int)va->y));
	//return (float)sqrt((double)(va->x * va->x + va->y * va->y));
}
void VECTOR2D_Normalize(VECTOR2D_PTR va, VECTOR2D_PTR vn){
	if (va == NULL || vn == NULL)
		return;
	float length = (float)sqrt((double)(va->x * va->x + va->y * va->y));
	vn->x = va->x / length;
	vn->y = va->y / length;
}
void VECTOR2D_Normalize(VECTOR2D_PTR va){
	if (va == NULL)
		return;
	float length = (float)sqrt((double)(va->x * va->x + va->y * va->y));
	va->x = va->x / length;
	va->y = va->y / length;
}
void VECTOR2D_Build(VECTOR2D_PTR init, VECTOR2D_PTR term, VECTOR2D_PTR result){
	if (init == NULL || term == NULL || result == NULL)
		return;
	result->x = term->x - init->x;
	result->y = term->y - init->y;
}
float VECTOR2D_CosTh(VECTOR2D_PTR va, VECTOR2D_PTR vb){
	float result = 0.0f;
	if (va == NULL || vb == NULL)
		return result;
	result = (va->x * vb->x + va->y * vb->y) / (VECTOR2D_Length_Fast(va) * VECTOR2D_Length_Fast(vb));
	return result;
}
void VECTOR2D_Print(VECTOR2D_PTR va, char *name){

}
//3D向量函数
void VECTOR3D_Add(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vsum){
	if (va == NULL || vb == NULL || vsum == NULL)
		return;
	vsum->x = va->x - vb->x;
	vsum->y = va->y - vb->y;
	vsum->z = va->z - vb->z;
}
VECTOR3D VECTOR3D_Add(VECTOR3D_PTR va, VECTOR3D_PTR vb){
	VECTOR3D result;
	result.x = va->x + vb->x;
	result.y = va->y + vb->y;
	result.z = va->z + vb->z;
	return result;
}
void VECTOR3D_Sub(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vsum){
	if (va == NULL || vb == NULL || vsum == NULL)
		return;
	vsum->x = va->x - vb->x;
	vsum->y = va->y - vb->y;
	vsum->z = va->z - vb->z;
}
VECTOR3D VECTOR3D_Sub(VECTOR3D_PTR va, VECTOR3D_PTR vb){
	VECTOR3D result = { 0, 0, 0 };
	if (va == NULL || vb == NULL)
		return result;
	result.x = va->x - vb->x;
	result.y = va->y - vb->y;
	result.z = va->z - vb->z;
	return result;
}
void VECTOR3D_Scale(float k, VECTOR3D_PTR va){
	if (va == NULL)
		return;
	va->x *= k;
	va->y *= k;
	va->z *= k;
}
void VECTOR3D_Scale(float k, VECTOR3D_PTR va, VECTOR3D_PTR vscaled){
	if (va == NULL || vscaled == NULL)
		return;
	vscaled->x = va->x * k;
	vscaled->y = va->y * k;
	vscaled->z = va->z * k;
}
float VECTOR3D_Dot(VECTOR3D_PTR va, VECTOR3D_PTR vb){
	if (va == NULL || vb == NULL)
		return 0.0f;
	return va->x * va->x + vb->y * vb->y + va->z * vb->z;
}
void VECTOR3D_Cross(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vn){
	if (va == NULL || vb == NULL || vn == NULL)
		return;
	vn->x = va->x - vb->x;
	vn->y = va->y - vb->y;
	vn->z = va->z - vb->z;
}
VECTOR3D VECTOR3D_Cross(VECTOR3D_PTR va, VECTOR3D_PTR vb){
	VECTOR3D result = { 0, 0, 0 };
	if (va == NULL || vb == NULL)
		return result;
	/*
	float l1 = (float)sqrt((double)(va->x * va->x + va->y * va->y + va->z * va->z));
	float l2 = (float)sqrt((double)(vb->x * vb->x + vb->y * vb->y + vb->z * vb->z));
	float angle = acos((va->x*vb->x + va->y *vb->y + va->z*vb->z) / (l1 * l2));
	float distance = l1*l2*sin(angle);
	//(x, y, z)
	//x * va.x + y * va.y + z * va.z = 0
	//x * vb.x + y * vb.y + z * vb.z = 0
	//x * (va->x - vb->x * (va->y / vb->y)) + va->z - vb->z*(va->y / vb->y) = 0;
	result.x = (vb->z*(va->y / vb->y) - va->z) / (va->x - vb->x * (va->y / vb->y));
	result.y = -(result.x * va->x + va->z) / va->y;
	result.z = 1;
	VECTOR3D_Normalize(&result);
	result.x = distance * result.x; result.y = distance * result.y; result.z = distance * result.z;
	*/
	//叉积公式
	//a2b3-a3b2, a3b1-a1b3, a1b2-a2b1
	result.x = va->y*vb->z - va->z*vb->y;
	result.y = va->z*vb->x - va->x*vb->z;
	result.z = va->x*vb->y - va->y*vb->x;
	return result;
}
float VECTOR3D_Length(VECTOR3D_PTR va){
	float result = 0.0f;
	if (va == NULL)
		return result;
	//result = (float)sqrt((double)(va->x * va->x + va->y * va->y + va->z * va->z));
	return Fast_Distance_3D(va->x, va->y, va->z);
}
float VECTOR3D_Length_Fast(VECTOR3D_PTR va){			//此处应该采用泰勒级数，而当前使用的方法为暂时的（。。。。主要是自己太菜，数学功底不好。。。）
	float result = 0.0f;
	if (va == NULL)
		return result;
	result = (float)sqrt((double)(va->x * va->x + va->y * va->y + va->z * va->z));
	return result;
}
VECTOR3D VECTOR3D_Normalize(VECTOR3D_PTR va){
	VECTOR3D result = { 0, 0, 0 };
	float length = 0;
	if (va == NULL)
		return result;
	length = (float)sqrt((double)(va->x * va->x + va->y * va->y + va->z * va->z));
	result.x = va->x / length; result.y = va->y / length; result.z = va->z / length;
	return result;
}
void VECTOR3D_Normalize(VECTOR3D_PTR va, VECTOR3D_PTR vn){
	float length;
	if (va == NULL || vn == NULL)
		return;
	length = (float)sqrt((double)(va->x * va->x + va->y * va->y + va->z * va->z));
	vn->x = va->x / length; vn->y = va->y / length; vn->z = va->z / length;
}
void VECTOR3D_Build(VECTOR3D_PTR init, VECTOR3D_PTR term, VECTOR3D_PTR result){
	if (init == NULL || term == NULL || result == NULL)
		return;
	result->x = term->x - init->x;
	result->y = term->y - init->y;
	result->z = term->z - init->z;
}
float VECTRO3D_CosTh(VECTOR3D_PTR va, VECTOR3D_PTR vb){
	if (va == NULL || vb == NULL)
		return 0.0f;
	float l1 = (float)sqrt((double)(va->x * va->x + va->y * va->y + va->z * va->z));
	float l2 = (float)sqrt((double)(vb->x * vb->x + vb->y * vb->y + vb->z * vb->z));
	float angle = (va->x * vb->x + va->y * vb->y + va->z * vb->z) / (l1 * l2);
	return angle;
}
void VECTOR3D_Print(VECTOR3D_PTR va, char *name){

}

//4D向量函数
void VECTOR4D_Add(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vsum){
	vsum->x = va->x + vb->x;
	vsum->y = va->y + vb->y;
	vsum->z = va->z + vb->z;
	vsum->w = 1;
}
VECTOR4D VECTOR4D_Add(VECTOR4D_PTR va, VECTOR4D_PTR vb){
	VECTOR4D vsum;
	vsum.x = va->x + vb->x;
	vsum.y = va->y + vb->y;
	vsum.z = va->z + vb->z;
	vsum.w = 1;
	return(vsum);
}
void VECTOR4D_Sub(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vdiff){
	vdiff->x = va->x - vb->x;
	vdiff->y = va->y - vb->y;
	vdiff->z = va->z - vb->z;
	vdiff->w = 1;
}
VECTOR4D VECTOR4D_Sub(VECTOR4D_PTR va, VECTOR4D_PTR vb){
	VECTOR4D vdiff;
	vdiff.x = va->x - vb->x;
	vdiff.y = va->y - vb->y;
	vdiff.z = va->z - vb->z;
	vdiff.w = 1;
	return(vdiff);
}
void VECTOR4D_Scale(float k, VECTOR4D_PTR va){
	va->x *= k;
	va->y *= k;
	va->z *= k;
	va->w = 1;
}
void VECTOR4D_Scale(float k, VECTOR4D_PTR va, VECTOR4D_PTR vscaled){
	vscaled->x = k*va->x;
	vscaled->y = k*va->y;
	vscaled->z = k*va->z;
	vscaled->w = 1;
}
float VECTOR4D_Dot(VECTOR4D_PTR va, VECTOR4D_PTR vb){
	return((va->x * vb->x) + (va->y * vb->y) + (va->z * vb->z));
}
void VECTOR4D_Cross(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vn){
	vn->x = ((va->y * vb->z) - (va->z * vb->y));
	vn->y = -((va->x * vb->z) - (va->z * vb->x));
	vn->z = ((va->x * vb->y) - (va->y * vb->x));
	vn->w = 1;
}
VECTOR4D VECTOR4D_Cross(VECTOR4D_PTR va, VECTOR4D_PTR vb){
	VECTOR4D vn;
	vn.x = ((va->y * vb->z) - (va->z * vb->y));
	vn.y = -((va->x * vb->z) - (va->z * vb->x));
	vn.z = ((va->x * vb->y) - (va->y * vb->x));
	vn.w = 1;
	return(vn);
}
float VECTOR4D_Length(VECTOR4D_PTR va){
	return(sqrtf(va->x*va->x + va->y*va->y + va->z*va->z));
}
float VECTOR4D_Length_Fast(VECTOR4D_PTR va){
	return(Fast_Distance_3D(va->x, va->y, va->z));
}
void VECTOR4D_Normalize(VECTOR4D_PTR va){
	float length = sqrtf(va->x*va->x + va->y*va->y + va->z*va->z);
	if (length < EPSILON_E5)
		return;
	float length_inv = 1.0f / length;
	va->x *= length_inv;
	va->y *= length_inv;
	va->z *= length_inv;
	va->w = 1;
}
void VECTOR4D_Normalize(VECTOR4D_PTR va, VECTOR4D_PTR vn){
	VECTOR4D_ZERO(vn);
	float length = sqrt(va->x*va->x + va->y*va->y + va->z*va->z);
	if (length < EPSILON_E5)
		return;
	float length_inv = 1.0f / length;
	vn->x = va->x*length_inv;
	vn->y = va->y*length_inv;
	vn->z = va->z*length_inv;
	vn->w = 1;
}
void VECTOR4D_Build(VECTOR4D_PTR init, VECTOR4D_PTR term, VECTOR4D_PTR result){
	result->x = term->x - init->x;
	result->y = term->y - init->y;
	result->z = term->z - init->z;
	result->w = 1;
}
float VECTOR4D_CosTh(VECTOR4D_PTR va, VECTOR4D_PTR vb){
	return(VECTOR4D_Dot(va, vb) / (VECTOR4D_Length(va)*VECTOR4D_Length(vb)));
}
void VECTOR4D_Print(VECTOR4D_PTR va, char *name){

}
//2维矩阵函数
void Mat_Init_2X2(MATRIX2X2_PTR ma, float m00, float m01, float m10, float m11){
	if (ma == NULL)
		return;
	ma->M00 = m00; ma->M01 = m01; ma->M10 = m10; ma->M11 = m11;
}
void Print_Mat_2X2(MATRIX2X2_PTR ma, char *name){

}
float Mat_Det_2X2(MATRIX2X2_PTR m){
	float result = 0.0f;
	if (m == NULL)
		return 0.0f;
	result = m->M00 * m->M11 - m->M01 * m->M10;
	return result;
}
void Mat_Add_2X2(MATRIX2X2_PTR ma, MATRIX2X2_PTR mb, MATRIX2X2_PTR msum){
	if (ma == NULL || mb == NULL || msum == NULL)
		return;
	msum->M00 = ma->M00 + mb->M00;
	msum->M01 = ma->M01 + mb->M01;
	msum->M10 = ma->M10 + mb->M10;
	msum->M11 = ma->M11 + mb->M11;
}
void Mat_Mul_2X2(MATRIX2X2_PTR ma, MATRIX2X2_PTR mb, MATRIX2X2_PTR mprod){
	if (ma == NULL || mb == NULL || mprod == NULL)
		return;
	mprod->M00 = ma->M00 * mb->M00 + ma->M01 * mb->M10;
	mprod->M01 = ma->M00 * mb->M01 + ma->M01 * mb->M11;
	mprod->M10 = ma->M10 * mb->M00 + ma->M11 * mb->M10;
	mprod->M11 = ma->M10 * mb->M01 + ma->M11 * mb->M11;
}
int Mat_Inverse_2X2(MATRIX2X2_PTR m, MATRIX2X2_PTR mi){
	if (m == NULL)
		return 0;
	if (mi == NULL)
		return 1;
	float value = (m->M00 * m->M11 - m->M01 * m->M10);
	if (value == 0.0)
		return 0;
	value = 1 / value;
	mi->M00 = m->M11; mi->M11 = m->M00; mi->M01 = -m->M01; mi->M10 = -m->M10;
	mi->M00 *= value; mi->M01 *= value; mi->M10 *= value; mi->M11 *= value;
	return 1;
}
int Solve_2X2_System(MATRIX2X2_PTR A, MATRIX1X2_PTR X, MATRIX1X2_PTR B){
	return 0;
}
int Mat_Mul_1X2_2X2(MATRIX1X2_PTR ma, MATRIX2X2_PTR mb, MATRIX1X2_PTR mprod){
	if (ma == NULL || mb == NULL || mprod == NULL)
		return 0;
	int i, j;
	for (i = 0; i < 2; i++)
	{
		mprod->M[i] = 0;
		for (j = 0; j < 2; j++)
		{
			mprod->M[i] += ma->M[j] * mb->M[j][i];
		}
	}
	return 1;
}
int Mat_Mul_1X3_3X3(MATRIX1X3_PTR ma, MATRIX3X3_PTR mb, MATRIX1X3_PTR mprod){
	if (ma == NULL || mb == NULL || mprod == NULL)
		return 0;
	int i, j;
	for (i = 0; i < 3; i++)
	{
		mprod->M[i] = 0;
		for (j = 0; j < 3; j++)
		{
			mprod->M[i] += ma->M[i] * mb->M[j][i];
		}
	}
	return 1;
}
int Mat_Mul_3X3(MATRIX3X3_PTR ma, MATRIX3X3_PTR mb, MATRIX3X3_PTR mprod){
	int i, j, k;
	if (ma == NULL || mb == NULL || mprod == NULL)
		return 0;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			mprod->M[i][j] = 0;
			for (k = 0; k < 3; k++)
			{
				mprod->M[i][j] += ma->M[i][k] * mb->M[k][j];
			}
		}
	}
	return 1;
}

inline int Mat_Init_3X2(MATRIX3X2_PTR ma,
	float m00, float m01,
	float m10, float m11,
	float m20, float m21){
	if (ma == NULL)
		return 0;
	ma->M00 = m00; ma->M01 = m01;
	ma->M10 = m10; ma->M11 = m11;
	ma->M20 = m20; ma->M21 = m21;
	return 1;
}
void Mat_Add_3X3(MATRIX3X3_PTR ma, MATRIX3X3_PTR mb, MATRIX3X3_PTR mprod){
	mprod->M00 = ma->M00 + mb->M00; mprod->M01 = ma->M01 + mb->M00; mprod->M02 = ma->M02 + mb->M02;
	mprod->M10 = ma->M10 + mb->M10; mprod->M11 = ma->M11 + mb->M11; mprod->M12 = ma->M12 + mb->M12;
	mprod->M20 = ma->M20 + mb->M20; mprod->M21 = ma->M21 + mb->M21; mprod->M22 = ma->M22 + mb->M22;
}
void Mat_Mul_VECTOR3D_3X3(VECTOR3D_PTR va, MATRIX3X3_PTR mb, VECTOR3D_PTR vprod){
	if (va == NULL || mb == NULL || vprod == NULL)
		return;
	int i, j;
	for (i = 0; i < 3; i++)
	{
		vprod->M[i] = 0;
		for (j = 0; j < 3; j++)
		{
			vprod->M[i] += va->M[j] * mb->M[j][i];
		}
	}
}
int Mat_Invarse_3X3(MATRIX3X3_PTR m, MATRIX3X3_PTR mi){
	if (m == NULL || mi == NULL)
		return 0;
	//00 01 02
	//10 11 12
	//20 21 22
	//先求代数余子式
	mi->M00 = m->M11 * m->M22 - m->M10 * m->M01; mi->M01 = -(m->M10 * m->M22 - m->M12 * m->M20); mi->M02 = m->M10 * m->M21 - m->M11 * m->M20;
	mi->M10 = -(m->M01 * m->M22 - m->M21 * m->M02);  mi->M11 = m->M00 * m->M22 - m->M20 * m->M02; mi->M12 = -(m->M00 * m->M21 - m->M02 * m->M20);
	mi->M20 = m->M00 * m->M11 - m->M10 * m->M02; mi->M21 = -(m->M00 * m->M12 - m->M10 * m->M02); mi->M22 = m->M00 * m->M11 - m->M01 * m->M10;
	//求行列式
	float value = mi->M00 * m->M00 + mi->M01 * m->M01 * -1 + mi->M02 * m->M02;
	if (value <= 0.000001)
		return 0;
	//求伴随矩阵
	float temp;
	temp = mi->M01; mi->M01 = mi->M10; mi->M10 = temp;
	temp = mi->M02; mi->M02 = mi->M20; mi->M20 = temp;
	temp = mi->M12; mi->M12 = mi->M21; mi->M21 = temp;
	//得到逆矩阵
	mi->M00 /= value; mi->M01 /= value; mi->M02 /= value;
	mi->M10 /= value; mi->M11 /= value; mi->M12 /= value;
	mi->M20 /= value; mi->M21 /= value; mi->M22 /= value;
	return 1;
}
void Mat_Init_3X3(MATRIX3X3_PTR ma,
	float m00, float m01, float m02,
	float m10, float m11, float m12,
	float m20, float m21, float m22){
	ma->M00 = m00; ma->M01 = m01; ma->M02 = m02;
	ma->M10 = m10; ma->M11 = m11; ma->M12 = m12;
	ma->M20 = m20; ma->M21 = m21; ma->M22 = m22;
}
void Print_Mat_3X3(MATRIX3X3_PTR ma, char *name){

}
float Mat_Det_3X3(MATRIX3X3_PTR m){
	MATRIX3X3 mi;
	if (m == NULL)
		return 0.0f;
	//求第一行的代数余子式
	mi.M00 = m->M11 * m->M22 - m->M10 * m->M01;
	mi.M01 = m->M10 * m->M22 - m->M12 * m->M20;
	mi.M02 = m->M10 * m->M21 - m->M11 * m->M20;
	//求行列式
	float value = mi.M00 * m->M00 + mi.M01 * m->M01 + mi.M02 * m->M02;
	return value;
}
int Solve_3X3_System(MATRIX3X3_PTR A, MATRIX1X3_PTR X, MATRIX1X3_PTR B){
	return 0;
}

//4X4矩阵函数
void Mat_Add_4X4(MATRIX4X4_PTR ma, MATRIX4X4_PTR mb, MATRIX4X4_PTR msum){
	if (ma == NULL || mb == NULL || msum == NULL)
		return;
	msum->M00 = ma->M00 + mb->M00; msum->M01 = ma->M01 + mb->M01; msum->M11 = ma->M11 + mb->M11;
	msum->M10 = ma->M10 + mb->M10; msum->M11 = ma->M11 + mb->M11; msum->M12 = ma->M12 + mb->M12;
	msum->M20 = ma->M20 + mb->M20; msum->M21 = ma->M21 + mb->M21; msum->M22 = ma->M22 + mb->M22;
}
void Mat_Mul_4X4(MATRIX4X4_PTR ma, MATRIX4X4_PTR mb, MATRIX4X4_PTR mprod){
	if (ma == NULL || mb == NULL || mprod == NULL)
		return;
	int i, j, k;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			mprod->M[i][j] = 0;
			for (k = 0; k < 4; k++)
			{
				mprod->M[i][j] += ma->M[i][k] * mb->M[k][j];
			}
		}
	}
}
void Mat_Mul_1X4_4X4(MATRIX1X4_PTR ma, MATRIX4X4_PTR mb, MATRIX1X4_PTR mrpod){
	if (ma == NULL || mb == NULL || mrpod == NULL)
		return;
	int i, j;
	for (i = 0; i < 4; i++)
	{
		mrpod->M[i] = 0;
		for (j = 0; j < 4; j++)
		{
			mrpod->M[i] += ma->M[j] * mb->M[j][i];
		}
	}
}
void Mat_Mul_VECTOR3D_4X4(VECTOR3D_PTR va, MATRIX4X4_PTR mb, VECTOR3D_PTR vprod){
	if (va == NULL || mb == NULL || vprod == NULL)
		return;
	int i, j;
	for (i = 0; i < 3; i++)
	{
		vprod->M[i] = 0;
		for (j = 0; j < 3; j++)
		{
			vprod->M[i] += va->M[j] * mb->M[j][i];
		}
		vprod->M[i] += 1 * mb->M[j][i];
	}
}
void Mat_Mul_VECTOR3D_4X3(VECTOR3D_PTR va, MATRIX4X3_PTR mb, VECTOR3D_PTR vprod){
	if (va == NULL || mb == NULL || vprod == NULL)
		return;
	int i, j;
	for (i = 0; i < 3; i++)
	{
		vprod->M[i] = 0;
		for (j = 0; j < 3; j++)
		{
			vprod->M[i] += va->M[j] * mb->M[j][i];
		}
	}
}
void Mat_Mul_VECTOR4D_4X4(VECTOR4D_PTR va, MATRIX4X4_PTR mb, VECTOR4D_PTR vprod){
	if (va == NULL || mb == NULL || vprod == NULL)
		return;
	int i, j;
	for (i = 0; i < 4; i++)
	{
		vprod->M[i] = 0;
		for (j = 0; j < 4; j++)
		{
			vprod->M[i] += va->M[j] * mb->M[j][i];
		}
	}
}
void Mat_Mul_VECTOR4D_4X3(VECTOR4D_PTR va, MATRIX4X3_PTR mb, VECTOR4D_PTR vprod){
	if (va == NULL || mb == NULL || vprod == NULL)
		return;
	int i, j;
	for (i = 0; i < 3; i++)
	{
		vprod->M[i] = 0;
		for (j = 0; j < 4; j++)
		{
			vprod->M[i] += va->M[j] * mb->M[j][i];
		}
	}
}
//暂未实现
int Mat_Inverse_4X4(MATRIX4X4_PTR m, MATRIX4X4_PTR mi){
	if (m == NULL || mi == NULL)
		return 0;
	float det = (
		m->M00 * (m->M11 * m->M22 - m->M12 * m->M21) -
		m->M01 * (m->M10 * m->M22 - m->M12 * m->M20) +
		m->M02 * (m->M10 * m->M21 - m->M11 * m->M20)
		);
	if (fabs(det) < EPSILON_E5)
		return 0;
	float det_inv = 1.0f / det;
	mi->M00 = det_inv * (m->M11 * m->M22 - m->M12 * m->M21);
	mi->M01 = -det_inv * (m->M01 * m->M22 - m->M02 * m->M21);
	mi->M02 = det_inv * (m->M01 * m->M12 - m->M02 * m->M11);
	mi->M03 = 0.0f;

	mi->M10 = -det_inv * (m->M10 * m->M22 - m->M12 * m->M20);
	mi->M11 = det_inv * (m->M00 * m->M22 - m->M02 * m->M20);
	mi->M12 = -det_inv * (m->M00 * m->M12 - m->M02 * m->M10);
	mi->M13 = 0.0f;

	mi->M20 = det_inv * (m->M10 * m->M21 - m->M11 * m->M20);
	mi->M21 = -det_inv * (m->M00 * m->M21 - m->M01 * m->M20);
	mi->M22 = det_inv * (m->M00 * m->M11 - m->M01 * m->M10);
	mi->M23 = 0.0f;

	mi->M30 = -(m->M30 * mi->M00 + m->M31 * mi->M10 + m->M32 * mi->M20);
	mi->M31 = -(m->M30 * mi->M01 + m->M31 * mi->M11 + m->M32 * mi->M21);
	mi->M32 = -(m->M30 * mi->M02 + m->M31 * mi->M12 + m->M32 * mi->M22);
	mi->M33 = 1.0f;
	return 1;
}
void Mat_Init_4X4(MATRIX4X4_PTR ma,
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33
	){
	ma->M00 = m00; ma->M01 = m01; ma->M02 = m02; ma->M03 = m03;
	ma->M10 = m10; ma->M11 = m11; ma->M12 = m12; ma->M13 = m13;
	ma->M20 = m20; ma->M21 = m21; ma->M22 = m22; ma->M23 = m23;
	ma->M30 = m30; ma->M31 = m31; ma->M32 = m32; ma->M33 = m33;
}
void Print_Mat_4X4(MATRIX4X4_PTR ma, char *name){

}
//四元数函数
void QUAT_Add(QUAT_PTR q1, QUAT_PTR q2, QUAT_PTR qsum){
	qsum->x = q1->x + q2->x;
	qsum->y = q1->y + q2->y;
	qsum->z = q1->z + q2->z;
	qsum->w = q1->w + q2->w;
}
void QUAT_Sub(QUAT_PTR q1, QUAT_PTR q2, QUAT_PTR qdiff){
	qdiff->x = q1->x - q2->x;
	qdiff->y = q1->y - q2->y;
	qdiff->z = q1->z - q2->z;
	qdiff->w = q1->w - q2->w;
}
void QUAT_Conjugate(QUAT_PTR q, QUAT_PTR qconj){
	qconj->x = -q->x;
	qconj->y = -q->y;
	qconj->z = -q->z;
	qconj->w = q->w;
}
void QUAT_Scale(QUAT_PTR q, float scale, QUAT_PTR qs){
	qs->x = scale*q->x;
	qs->y = scale*q->y;
	qs->z = scale*q->z;
	qs->w = scale*q->w;
}
void QUAT_Scale(QUAT_PTR q, float scale){
	q->x *= scale;
	q->y *= scale;
	q->z *= scale;
	q->w *= scale;
}
float QUAT_Norm(QUAT_PTR q){
	return(sqrtf(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z));
}
float QUAT_Norm2(QUAT_PTR q){
	return(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z);
}
void QUAT_Normalize(QUAT_PTR q, QUAT_PTR qn){
	float qlength_inv = 1.0 / (sqrtf(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z));
	qn->w = q->w*qlength_inv;
	qn->x = q->x*qlength_inv;
	qn->y = q->y*qlength_inv;
	qn->z = q->z*qlength_inv;
}
void QUAT_Normalize(QUAT_PTR q){
	float qlength_inv = 1.0 / (sqrtf(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z));
	q->w *= qlength_inv;
	q->x *= qlength_inv;
	q->y *= qlength_inv;
	q->z *= qlength_inv;
}
void QUAT_Unit_Inverse(QUAT_PTR q, QUAT_PTR qi){
	qi->w = q->w;
	qi->x = -q->x;
	qi->y = -q->y;
	qi->z = -q->z;
}
void QUAT_Unit_Inverse(QUAT_PTR q){
	q->x = -q->x;
	q->y = -q->y;
	q->z = -q->z;
}
void QUAT_Inverse(QUAT_PTR q, QUAT_PTR qi){
	float norm2_inv = 1.0 / (q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z);
	qi->w = q->w*norm2_inv;
	qi->x = -q->x*norm2_inv;
	qi->y = -q->y*norm2_inv;
	qi->z = -q->z*norm2_inv;
}
void QUAT_Inverse(QUAT_PTR q){
	float norm2_inv = 1.0 / (q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z);
	q->w = q->w*norm2_inv;
	q->x = -q->x*norm2_inv;
	q->y = -q->y*norm2_inv;
	q->z = -q->z*norm2_inv;
}
void QUAT_Mul(QUAT_PTR q1, QUAT_PTR q2, QUAT_PTR qprod){
	float prd_0 = (q1->z - q1->y) * (q2->y - q2->z);
	float prd_1 = (q1->w + q1->x) * (q2->w + q2->x);
	float prd_2 = (q1->w - q1->x) * (q2->y + q2->z);
	float prd_3 = (q1->y + q1->z) * (q2->w - q2->x);
	float prd_4 = (q1->z - q1->x) * (q2->x - q2->y);
	float prd_5 = (q1->z + q1->x) * (q2->x + q2->y);
	float prd_6 = (q1->w + q1->y) * (q2->w - q2->z);
	float prd_7 = (q1->w - q1->y) * (q2->w + q2->z);

	float prd_8 = prd_5 + prd_6 + prd_7;
	float prd_9 = 0.5 * (prd_4 + prd_8);

	// and finally build up the result with the temporary products

	qprod->w = prd_0 + prd_9 - prd_5;
	qprod->x = prd_1 + prd_9 - prd_8;
	qprod->y = prd_2 + prd_9 - prd_7;
	qprod->z = prd_3 + prd_9 - prd_6;
}
void QUAT_Triple_Product(QUAT_PTR q1, QUAT_PTR q2, QUAT_PTR q3, QUAT_PTR qprod){
	QUAT qtmp;
	QUAT_Mul(q1, q2, &qtmp);
	QUAT_Mul(&qtmp, q3, qprod);
}

void VECTOR3D_Theta_To_QUAT(QUAT_PTR q, VECTOR3D_PTR v, float theta){
	float theta_div_2 = (0.5)*theta; // compute theta/2
	float sinf_theta = sinf(theta_div_2);

	q->x = sinf_theta * v->x;
	q->y = sinf_theta * v->y;
	q->z = sinf_theta * v->z;
	q->w = cosf(theta_div_2);
}
void VECTOR4D_Theta_To_QUAT(QUAT_PTR q, VECTOR4D_PTR v, float theta){
	float theta_div_2 = (0.5)*theta; // compute theta/2

	// compute the quaterion, note this is from chapter 4

	// pre-compute to save time
	float sinf_theta = sinf(theta_div_2);

	q->x = sinf_theta * v->x;
	q->y = sinf_theta * v->y;
	q->z = sinf_theta * v->z;
	q->w = cosf(theta_div_2);
}
void EulerZYX_To_QUAT(QUAT_PTR q, float theta_z, float theta_y, float theta_x){
	float cos_z_2 = 0.5f*cosf(theta_z);
	float cos_y_2 = 0.5f*cosf(theta_y);
	float cos_x_2 = 0.5f*cosf(theta_x);

	float sin_z_2 = 0.5f*sinf(theta_z);
	float sin_y_2 = 0.5f*sinf(theta_y);
	float sin_x_2 = 0.5f*sinf(theta_x);

	// and now compute quaternion
	q->w = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
	q->x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
	q->y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
	q->z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;
}
void QUAT_To_VECTOR3D_Theta(QUAT_PTR q, VECTOR3D_PTR v, float  *theta){
	*theta = acosf(q->w);

	float sinf_theta_inv = 1.0 / sinf(*theta);

	v->x = q->x*sinf_theta_inv;
	v->y = q->y*sinf_theta_inv;
	v->z = q->z*sinf_theta_inv;

	*theta *= 2;
}
void QUAT_Print(QUAT_PTR q, char *name){

}

//2D参数化直线函数
void Init_Parm_Line2D(POINT2D_PTR p_init, POINT2D_PTR p_term, PARMLINE2D_PTR p){
	VECTOR2D_INIT(&(p->p0), p_init);

	// end point
	VECTOR2D_INIT(&(p->p1), p_term);

	// now compute direction vector from p0->p1
	VECTOR2D_Build(p_init, p_term, &(p->v));
}
void Compute_Parm_Line2D(PARMLINE2D_PTR p, float t, POINT2D_PTR pt){
	pt->x = p->p0.x + p->v.x*t;
	pt->y = p->p0.y + p->v.y*t;
}
int Intersect_Parm_Lines2D(PARMLINE2D_PTR p1, PARMLINE2D_PTR p2, float *t1, float *t2);
int Intersect_Parm_Lines2D(PARMLINE2D_PTR p1, PARMLINE2D_PTR p2, POINT2D_PTR pt);
//3D参数化直线函数
void Init_Parm_Line3D(POINT3D_PTR p_init, POINT3D_PTR p_term, PARMLINE3D_PTR p){
	VECTOR3D_INIT(&(p->p0), p_init);

	// end point
	VECTOR3D_INIT(&(p->p1), p_term);

	// now compute direction vector from p0->p1
	VECTOR3D_Build(p_init, p_term, &(p->v));
}
void Compute_Parm_Line3D(PARMLINE3D_PTR p, float t, POINT3D_PTR pt){
	pt->x = p->p0.x + p->v.x*t;
	pt->y = p->p0.y + p->v.y*t;
	pt->z = p->p0.z + p->v.z*t;
}
//3D平面函数
void PLANE3D_Init(PLANE3D_PTR plane, POINT3D_PTR p0,
	VECTOR3D_PTR normal, int normalize){
	POINT3D_COPY(&plane->p0, p0);

	// if normalize is 1 then the normal is made into a unit vector
	if (!normalize)
		VECTOR3D_COPY(&plane->n, normal);
	else
	{
		// make normal into unit vector
		VECTOR3D_Normalize(normal, &plane->n);
	} // end else
}
float Compute_Point_In_Plane3D(POINT3D_PTR pt, PLANE3D_PTR plane){
	float hs = plane->n.x*(pt->x - plane->p0.x) +
		plane->n.y*(pt->y - plane->p0.y) +
		plane->n.z*(pt->z - plane->p0.z);
	// return half space value
	return(hs);
}
int Intersect_Parm_Line3D_Plane3D(PARMLINE3D_PTR pline, PLANE3D_PTR plane, float *t, POINT3D_PTR pt){
	float plane_dot_line = VECTOR3D_Dot(&pline->v, &plane->n); if (fabs(plane_dot_line) <= EPSILON_E5)
	{
		// the line and plane are co-planer, does the line lie 
		// in the plane?
		if (fabs(Compute_Point_In_Plane3D(&pline->p0, plane)) <= EPSILON_E5)
			return(PARM_LINE_INTERSECT_EVERYWHERE);
		else
			return(PARM_LINE_NO_INTERSECT);
	} // end if

	*t = -(plane->n.x*pline->p0.x +
		plane->n.y*pline->p0.y +
		plane->n.z*pline->p0.z -
		plane->n.x*plane->p0.x -
		plane->n.y*plane->p0.y -
		plane->n.z*plane->p0.z) / (plane_dot_line);

	// now plug t into the parametric line and solve for x,y,z
	pt->x = pline->p0.x + pline->v.x*(*t);
	pt->y = pline->p0.y + pline->v.y*(*t);
	pt->z = pline->p0.z + pline->v.z*(*t);

	// test interval of t
	if (*t >= 0.0 && *t <= 1.0)
		return(PARM_LINE_INTERSECT_IN_SEGMENT);
	else
		return(PARM_LINE_INTERSECT_OUT_SEGMENT);
}
//顶点数函数
FIXP16 FIXP16_NUL(FIXP16 fp1, FIXP16 fp2);
FIXP16 FIXP16_DIV(FIXP16 fp1, FIXP16 fp2);
void FIXP16_Print(FIXP16 fp);