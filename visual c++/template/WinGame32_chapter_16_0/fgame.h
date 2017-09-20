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
#define D3DFVF_CUSTOMVERTEX {D3DFVF_XYZ | D3DFVF_DIFFUSE}
#define D3DFVF_CUSTOMVERTEX_NORMAL {D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE}
//#define D3DFVF_CUSTOMVERTEX {D3DFVF_XYZRHW | D3DFVF_DIFFUSE}
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#endif