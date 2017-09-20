#ifndef _MODEL_H_
#define _MODEL_H_

//用于简化测试的位掩码
#define PLX_RGB_MASK				0x8000
#define PLX_SHADE_MODE_MASK			0x8000
#define PLX_2SIDED_MASK				0x1000
#define PLX_COLOR_MASK				0x0fff
//用户判断那颜色模式的标记
#define PLX_COLOR_MODE_RGB_FLAG		0x8000
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000
//双面标记
#define PLX_2SIDED_FLAG				0x1000
#define PLX_1SIDED_FLAG				0x0000
//着色模式标记
#define PLX_SHADE_MODE_PURE_FLAG		0x0000	//多边形使用固定颜色
#define PLX_SHADE_MODE_FLAT_FLAG		0x2000	//恒定着色模式
#define PLX_SHADE_MODE_GOURAUD_FLAG		0x4000	//gouraud着色
#define PLX_SHADE_MODE_PHONG_FLAG		0x6000	//phong着色




char *Get_Line_PLG(char *buffer, int maxlength, FILE *fp);
float Compute_OBJECT4DV1_Radius(OBJECT4DV1_PTR obj);
int Load_OBJECT4DV1_PLG(OBJECT4DV1_PTR obj, char *filename,
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot);

#endif