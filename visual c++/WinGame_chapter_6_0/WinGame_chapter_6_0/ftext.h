#ifndef _FTEXT_H_
#define _FTEXT_H_
#include "femy.h"
void SetFont(HDC hdc, LPCWSTR text){
	HFONT hFont = CreateFont(45, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, text);
	SelectObject(hdc, hFont);
}

#endif