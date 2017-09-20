#ifndef _DRAW_H_
#define _DRAW_H_
#include <windows.h>
//�������޹�ͼ����ʾ����
void Draw_Triangle_2D(float x1, float y1, float x2, float y2, float x3, float y3, int color, HDC hdc, int mempitch);
void Draw_Line_2D(float x1, float y1, float x2, float y2, int color, HDC hdc, int lpitch);



//Gouraud��ɫ�Ļ���������
//��Gouraud��ɫ�������Լ�����GDI�Ļ�ͼģʽ���иĽ��ģ�ʹ�õ�����������ɫ������ԭ�������ؼ���ɫ
inline void SetPixel(float x, float y, int color, HDC hdc, int lpitch);			//Ϊ�ӿ��ٶȣ�����Ϊ��������
void Draw_Gouraud_Trangle(float x0, float y0, float i0, float x1, float y1, float i1, float x2, float y2, float i2, HDC hdc, int pitch);
void Draw_Gouraud_Line_2D(float x1, float y1, float x2, float y2, int color, HDC hdc, int lpitch);
#endif