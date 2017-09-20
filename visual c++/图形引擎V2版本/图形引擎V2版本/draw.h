#ifndef _DRAW_H_
#define _DRAW_H_
#include <windows.h>
//与引擎无关图像显示函数
void Draw_Triangle_2D(float x1, float y1, float x2, float y2, float x3, float y3, int color, HDC hdc, int mempitch);
void Draw_Line_2D(float x1, float y1, float x2, float y2, int color, HDC hdc, int lpitch);



//Gouraud着色的绘制三角形
//此Gouraud着色函数是自己根据GDI的绘图模式进行改进的，使用的是线条级着色，而非原来的像素级着色
inline void SetPixel(float x, float y, int color, HDC hdc, int lpitch);			//为加快速度，设置为内联函数
void Draw_Gouraud_Trangle(float x0, float y0, float i0, float x1, float y1, float i1, float x2, float y2, float i2, HDC hdc, int pitch);
void Draw_Gouraud_Line_2D(float x1, float y1, float x2, float y2, int color, HDC hdc, int lpitch);
#endif