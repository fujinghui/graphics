#include "draw.h"

void Draw_Triangle_2D(int x1, int y1, int x2, int y2, int x3, int y3, int color, HDC hdc, int mempitch){
	int temp_x, temp_y, new_x;
	if ((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3))
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
	if (y3 < y2)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x2;
		y3 = y2;
		x2 = temp_x;
		y2 = temp_y;
	}
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
		for (int y = y1; y <= y3; y++)
		{
			Draw_Line_2D((int)xs, y, (int)xe, y, color, hdc, mempitch);
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
		for (int y = y1; y <= y2; y++)
		{
			Draw_Line_2D((int)xs, y, (int)xe, y, color, hdc, mempitch);
			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else
	{
		float dxy_left = (float)(x2 - x1) / (float)(y2 - y1);
		float dxy_right = (float)(x3 - x1) / (float)(y3 - y1);
		float xs = x1, xe = x1;
		int y;
		for (y = y1; y <= y2; y++)
		{
			Draw_Line_2D((int)xs, y, (int)xe, y, color, hdc, mempitch);
			xs += dxy_left;
			xe += dxy_right;
		}
		dxy_left = (float)(x3 - x2) / (float)(y3 - y2);
		xs = x2;
		for (; y <= y3; y++)
		{
			Draw_Line_2D((int)xs, y, (int)xe, y, color, hdc, mempitch);
			xs += dxy_left;
			xe += dxy_right;
		}
	}
}
void Draw_Line_2D(int x1, int y1, int x2, int y2, int color, HDC hdc, int lpitch){
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);

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



