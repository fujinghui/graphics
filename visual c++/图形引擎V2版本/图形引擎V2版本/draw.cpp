#include "draw.h"
#include <stdio.h>

void Draw_Triangle_2D(float x1, float y1, float x2, float y2, float x3, float y3, int color, HDC hdc, int mempitch){
	float temp_x, temp_y, new_x;
	int count = 0;
	x1 = (float)(int)x1;
	y1 = (float)(int)y1;
	x2 = (float)(int)x2;
	y2 = (float)(int)y2;
	x3 = (float)(int)x3;
	y3 = (float)(int)y3;
	
//	MoveToEx(hdc, (int)x3, (int)y3, NULL);
//	LineTo(hdc, (int)x1, (int)y1);
//	LineTo(hdc, (int)x2, (int)y2);
//	LineTo(hdc, (int)x3, (int)y3);


	if ((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3))
		return;
	if ((x1 == x2 && y1 == y2) || (x1 == x3 && y1 == y3) || (x2 == x3 && y2 == y3))
		return;
	HPEN hPen = CreatePen(PS_SOLID, mempitch, color);
	HPEN old_hpen = (HPEN)SelectObject(hdc, hPen);
	DeleteObject(old_hpen);
//	Draw_Line_2D(x1, y1, x2, y2, color, hdc, 1);
//	Draw_Line_2D(x1, y1, x3, y3, color, hdc, 1);
// 	Draw_Line_2D(x2, y2, x3, y3, color, hdc, 1);
	if (y2 < y1)
	{
		temp_x = x2;
		temp_y = y2;
		y2 = y1;
		x2 = x1;
		x1 = temp_x;
		y1 = temp_y;
	}
	if (y3 < y1)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x1;
		y3 = y1;
		x1 = temp_x;
		y1 = temp_y;
	}
//	if (y3 < y2)
//	{
//		temp_x = x3;
//		temp_y = y3;
//		x3 = x2;
//		y3 = y2;
//		x2 = temp_x;
//		y2 = temp_y;
//	}
	if (x2 > x3)
	{
		temp_x = x2; temp_y = y2;
		x2 = x3; y2 = y3;
		x3 = temp_x; y3 = temp_y;
	}
	float dxy_left = (x2 - x1) / (y2 - y1);
	float dxy_right = (x3 - x1) / (y3 - y1);
	float xs = x1, xe = x1;
	int y;
	int min_y = (int)min(y2, y3);
	for (y = y1; y <= min_y; y++)
	{
		Draw_Line_2D(xs, y, xe, y, color, hdc, mempitch);
		count++;
		xs += dxy_left;
		xe += dxy_right;
	}
	if (y2 > y3)
	{
		dxy_right = (x2 - x3) / (y2 - y3);
		xs = x1 + ((min_y - y1) * (x2 - x1) / (y2 - y1));
		xe = x3;
	}
	else
	{
		dxy_left = (x3 - x2) / (y3 - y2);
		xs = x2;
		xe = x1 + (min_y - y1) * (x3 - x1) / (y3 - y1);
	}
	int max_y = (int)max(y2, y3);
	for (y = min_y; y <= max_y; y++)
	{
		Draw_Line_2D(xs, y, xe, y, color, hdc, mempitch);
		count++;
		xs += dxy_left;
		xe += dxy_right;
	}

	/*
	char text[100];

	if (count < 5)
	{
		sprintf_s(text, "count:%d\nx0:%d y0:%d\n x1:%d y1:%d\n x2:%d y2:%d\0", count, x1, y1, x2, y2, x3, y3);
		MessageBox(NULL, text, "FAS", MB_OK);
	}
*/

	/*
	//y1 <= y2 <= y3
	if (y1 == y2)
	{
		//	  ********
		//     ******
		//		***
		//		 *
		float dxy_left = (float)(x3 - x1) / (float)(y3 - y1);
		float dxy_right = (float)(x3 - x2) / (float)(y3 - y2);
		float xs = x1;
		float xe = x2;
		for (int y = (int)y1; y <= y3; y++)
		{
			Draw_Line_2D(xs, y, xe, y, color, hdc, mempitch);
			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else if (y2 == y3)
	{
		//		*
		//	   ***
		//	  *****
		//   *******
		float dxy_left = (float)(x2 - x1) / (float)(y2 - y1);
		float dxy_right = (float)(x3 - x1) / (float)(y3 - y1);
		float xs = x1, xe = x1;
		for (int y = (int)y1; y <= y2; y++)
		{
			Draw_Line_2D(xs, y, xe, y, color, hdc, mempitch);
			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else
	{
		float dxy_left = (x2 - x1) / (y2 - y1);
		float dxy_right = (x3 - x1) / (y3 - y1);
		float xs = x1, xe = x1;
		int y;
		for (y = (int)y1; y <= y2; y++)
		{
			Draw_Line_2D(xs, y, xe, y, color, hdc, mempitch);
			xs += dxy_left;
			xe += dxy_right;
		}
		dxy_left = (x3 - x2) / (y3 - y2);
		xs = x2;
		xe = x1 + (y2 - y1) * (x3 - x1) / (y3 - y1);
		//xe = x1 + ((float)(x3 - x1) * (y - y1) / (float)(y3 - y1));
		for (y = (int)y2; y <= y3; y++)
		{
			//Draw_Line_2D(xs, y, xe, y, color, hdc, mempitch);
			xs += dxy_left;
			xe += dxy_right;
		}
	}
	*/
}
void Draw_Line_2D(float x1, float y1, float x2, float y2, int color, HDC hdc, int lpitch){
	MoveToEx(hdc, (int)x1, (int)y1, NULL);
	LineTo(hdc, (int)x2, (int)y2);

	/*
	int x = x1, y = y1;
	int dx = x2 - x1, dy = y2 - y1;
	int p = 2 * dy - dx;
	while (x <= x2)
	{
		//Draw_Pixel(x, y);
		if (p >= 0)
		{
			y = y + 1;
			p = p + 2 * (dy - dx);
		}
		else
			p = p + 2 * dy;
		x++;
	}
	*/
}



inline void SetPixel(float x, float y, int color, HDC hdc, int lpitch){
	static RECT rect;
	HBRUSH hBrush =  CreateSolidBrush(color);
	rect.left = x; rect.top = y; rect.right = x + 1; rect.bottom = y + 1;
	FillRect(hdc, &rect, hBrush);
	DeleteObject(hBrush);
}
float xx0, yy0, xx1, yy1, xx2, yy2;
void Draw_Gouraud_Trangle(float x0, float y0, float i0, float x1, float y1, float i1, float x2, float y2, float i2, HDC hdc, int pitch){
	float tx, ty, ti;
	//退化成了直线，直接退出循环
	if ((x0 == x1 && x1 == x2) || (y0 == y1 && y1 == y2))
		return;
	if ((x0 == x1 && y0 == y1) || (x0 == x2 && y0 == y2) || (x1 == x2 && y1 == y2))
		return;


	x1 = (float)(int)x1;
	y1 = (float)(int)y1;
	x2 = (float)(int)x2;
	y2 = (float)(int)y2;
	x0 = (float)(int)x0;
	y0 = (float)(int)y0;

	static int count = 0;
	char text[128];
	
	//sprintf_s(text, "%f %f %f\0", i1, i2, i2_b);
	//MessageBox(NULL, text, "title", MB_OK);
	/*
	if (count <= 1)
	{
		sprintf_s(text, "count:%d\nx0:%f y0:%f\n x1:%f y1:%f\n x2:%f y2:%f\0", count, xx0, yy0, xx1, yy1, xx2, yy2);
		MessageBox(NULL, text, "error", MB_OK);
	}
	


	if (count <= 1)
	{
		sprintf_s(text, "count:%d\nx0:%f y0:%f\n x1:%f y1:%f\n x2:%f y2:%f\0", count, xx0, yy0, xx1, yy1, xx2, yy2);
		MessageBox(NULL, text, "error", MB_OK);
	}
	*/

	//y0<y1<y2
	if (y0 > y1)
	{
		tx = x0; ty = y0; ti = i0;
		x0 = x1; y0 = y1; i0 = i1;
		x1 = tx; y1 = ty; i1 = ti;
	}
	if (y0 > y2)
	{
		tx = x0; ty = y0; ti = i0;
		x0 = x2; y0 = y2; i0 = i2;
		x2 = tx; y2 = ty; i2 = ti;
	}
	if (x2 > x1)
	{
		tx = x2; ty = y2; ti = i2;
		x2 = x1; y2 = y1; i2 = i1;
		x1 = tx; y1 = ty; i1 = ti;
	}
	xx0 = x0; yy0 = y0;
	xx1 = x1; yy1 = y1;
	xx2 = x2; yy2 = y2;


	//HPEN hpen = (HPEN)SelectObject(hdc, GetStockObject(WHITE_PEN));
	//DeleteObject(hpen);
	
	count = 0;
	float xl_edge = x0, xr_edge = x0;			//坐标


	float i0_r = (float)(((int)i0) & 0xff), i0_g = (float)((((int)i0) >> 8) & 0xff), i0_b = (float)((((int)i0) >> 16) & 0xff);
	float i1_r = (float)(((int)i1) & 0xff), i1_g = (float)((((int)i1) >> 8) & 0xff), i1_b = (float)((((int)i1) >> 16) & 0xff);
	float i2_r = (float)(((int)i2) & 0xff), i2_g = (float)((((int)i2) >> 8) & 0xff), i2_b = (float)((((int)i2) >> 16) & 0xff);

	float il_edge_r = i0_r, il_edge_g = i0_g, il_edge_b = i0_b;
	float ir_edge_r = il_edge_r, ir_edge_g = il_edge_g, ir_edge_b = il_edge_b;


	float dxldy = (x2 - x0) / (y2 - y0);
	float dxrdy = (x1 - x0) / (y1 - y0);
	

	float dildy_r, dildy_g, dildy_b;
	float dirdy_r, dirdy_g, dirdy_b;

	dildy_r = (i2_r - i0_r) / (y2 - y0);
	dildy_g = (i2_g - i0_g) / (y2 - y0);
	dildy_b = (i2_b - i0_b) / (y2 - y0);

	dirdy_r = (i1_r - i0_r) / (y1 - y0);
	dirdy_g = (i1_g - i0_g) / (y1 - y0);
	dirdy_b = (i1_b - i0_b) / (y1 - y0);
	
	float i_r, i_g, i_b;
	float dix_r, dix_g, dix_b;


	//开始着色
	//渲染上半部分，平底三角形
	int min_y = min(y1, y2);
	int y;
	float i;
	int x;
	for (y = y0; y <= min_y; y++)
	{
		//设置x坐标和颜色强度的起始值
		//i = il_edge;// (il_edge + ir_edge) / 2;			//当前采用GDI模式显示图像，不支持直接操作像素，因此使用左右两个点的的颜色值的平均值渲染
		
		i_r = il_edge_r; i_g = il_edge_g; i_b = il_edge_b;
		//计算强度的水平梯度
		//原来的Gouraud着色模式方法
		//	dix = (ir_edge - il_edge) / (xr_edge - xl_edge);
		dix_r = (ir_edge_r - il_edge_r) / (xr_edge - xl_edge);
		dix_g = (ir_edge_g - il_edge_g) / (xr_edge - xl_edge);
		dix_b = (ir_edge_b - il_edge_b) / (xr_edge - xl_edge);
		for (x = xl_edge; x <= xr_edge; x++)
		{
			i = (float)(((int)(i_r)) | (((int)(i_g)) << 8) | (((int)(i_b)) << 16));		
			SetPixel((float)x, (float)y, i, hdc, 1);
			//count++;
			i_r += dix_r; i_g += dix_g; i_b += dix_b;
		}
		//Draw_Gouraud_Line_2D(xl_edge, y, xr_edge, y, i, hdc, pitch);
		xl_edge += dxldy;
		xr_edge += dxrdy;

		il_edge_r += dildy_r; il_edge_g += dildy_g; il_edge_b += dildy_b;
		ir_edge_r += dirdy_r; ir_edge_g += dirdy_g; ir_edge_b += dirdy_b;
	}
	if (y2 > y1)
	{

		dxrdy = (x2 - x1) / (y2 - y1);
		xl_edge = x0 + (min_y - y0) * (x2 - x0) / (y2 - y0);
		xr_edge = x1;

		ir_edge_r = i1_r;
		ir_edge_g = i1_g;
		ir_edge_b = i1_b;

		il_edge_r = i0_r + (min_y - y0) * (i2_r - i0_r) / (y2 - y0);
		il_edge_g = i0_g + (min_y - y0) * (i2_g - i0_g) / (y2 - y0);
		il_edge_b = i0_b + (min_y - y0) * (i2_b - i0_b) / (y2 - y0);

		dirdy_r = (i2_r - i1_r) / (y2 - y1);
		dirdy_g = (i2_g - i1_g) / (y2 - y1);
		dirdy_b = (i2_b - i1_b) / (y2 - y1);
	}
	else
	{
		dxldy = (x1 - x2) / (y1 - y2);
		xl_edge = x2;
		xr_edge = x0 + (min_y - y0) * (x1 - x0) / (y1 - y0);

		il_edge_r = i2_r;
		il_edge_g = i2_g;
		il_edge_b = i2_b;

		ir_edge_r = i0_r + (min_y - y0) * (i1_r - i0_r) / (y1 - y0);
		ir_edge_g = i0_g + (min_y - y0) * (i1_g - i0_g) / (y1 - y0);
		ir_edge_b = i0_b + (min_y - y0) * (i1_b - i0_b) / (y1 - y0);

		dildy_r = (i1_r - i2_r) / (y1 - y2);
		dildy_g = (i1_g - i2_g) / (y1 - y2);
		dildy_b = (i1_b - i2_b) / (y1 - y2);
	}
	//渲染下半部分，平顶三角形
	int max_y = max(y1, y2);
	
	for (y = min_y; y <= max_y; y++)
	{
		i_r = il_edge_r; i_g = il_edge_g; i_b = il_edge_b;
		dix_r = (ir_edge_r - il_edge_r) / (xr_edge - xl_edge);
		dix_g = (ir_edge_g - il_edge_g) / (xr_edge - xl_edge);
		dix_b = (ir_edge_b - il_edge_b) / (xr_edge - xl_edge);
		for (x = xl_edge; x <= xr_edge; x++)
		{
			i = (float)(((int)i_r) | (((int)i_g) << 8) | (((int)i_b) << 16));
			SetPixel(hdc, (int)x, (int)y, i);
			//count++;
			i_r += dix_r;
			i_g += dix_g;
			i_b += dix_b;
		}
		//Draw_Gouraud_Line_2D(xl_edge, y, xr_edge, y, i, hdc, pitch);
		xl_edge += dxldy;
		xr_edge += dxrdy;
		
		ir_edge_r += dirdy_r;
		ir_edge_g += dirdy_g;
		ir_edge_b += dirdy_b;

		il_edge_r += dildy_r;
		il_edge_g += dildy_g;
		il_edge_b += dildy_b;
	}
}
void Draw_Gouraud_Line_2D(float x1, float y1, float x2, float y2, int color, HDC hdc, int lpitch){
	HPEN hPen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, lpitch, color));
	DeleteObject(hPen);
	MoveToEx(hdc, (int)x1, (int)y1, NULL);
	LineTo(hdc, (int)x2, (int)y2);
}


