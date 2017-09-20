#ifndef _FGAME_H_
#define _FGAME_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#define SAFE_RELEASE(p) {						\
if (p)				\
							{					\
							(p)->Release();		\
							}					\
}
#define SAFE_DELETE(p){							\
if (p)											\
{												\
	delete (p); (p) = NULL;						\
}												\
}

#define HR(x) if(FAILED(x))						\
			  {									\
			  return x;							\
}
typedef struct tagFVertex{
	FLOAT x, y, z;
	DWORD color;
}FVertex;
typedef struct CUSTOMVERTEX{
	FLOAT _x, _y, _z;	//顶点坐标
	FLOAT _u, _v;		//纹理坐标
	CUSTOMVERTEX(FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v)
		:_x(x), _y(y), _z(z), _u(u), _v(v){}
}CUSTOMVERTEX;

#define D3DFVF_CUSTOMVERTEX {D3DFVF_XYZ | D3DFVF_DIFFUSE}
#define D3DFVF_CUSTOMVERTEX_NORMAL {D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE}
#define D3DFVF_CUSTOMVERTEX_TEX (D3DFVF_XYZ | D3DFVF_TEX1)
//#define D3DFVF_CUSTOMVERTEX {D3DFVF_XYZRHW | D3DFVF_DIFFUSE}
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#endif