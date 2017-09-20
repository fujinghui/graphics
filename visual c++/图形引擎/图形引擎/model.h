#ifndef _MODEL_H_
#define _MODEL_H_

//���ڼ򻯲��Ե�λ����
#define PLX_RGB_MASK				0x8000
#define PLX_SHADE_MODE_MASK			0x8000
#define PLX_2SIDED_MASK				0x1000
#define PLX_COLOR_MASK				0x0fff
//�û��ж�����ɫģʽ�ı��
#define PLX_COLOR_MODE_RGB_FLAG		0x8000
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000
//˫����
#define PLX_2SIDED_FLAG				0x1000
#define PLX_1SIDED_FLAG				0x0000
//��ɫģʽ���
#define PLX_SHADE_MODE_PURE_FLAG		0x0000	//�����ʹ�ù̶���ɫ
#define PLX_SHADE_MODE_FLAT_FLAG		0x2000	//�㶨��ɫģʽ
#define PLX_SHADE_MODE_GOURAUD_FLAG		0x4000	//gouraud��ɫ
#define PLX_SHADE_MODE_PHONG_FLAG		0x6000	//phong��ɫ




char *Get_Line_PLG(char *buffer, int maxlength, FILE *fp);
float Compute_OBJECT4DV1_Radius(OBJECT4DV1_PTR obj);
int Load_OBJECT4DV1_PLG(OBJECT4DV1_PTR obj, char *filename,
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot);

#endif