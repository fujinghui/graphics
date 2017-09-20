#ifndef _DRAW_H_
#define _DRAW_H_
#include <windows.h>
//与引擎无关图像显示函数
void Draw_Triangle_2D(int x1, int y1, int x2, int y2, int x3, int y3, int color, HDC hdc, int mempitch);
void Draw_Line_2D(int x1, int y1, int x2, int y2, int color, HDC hdc, int lpitch);

#endif