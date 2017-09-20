#ifndef _FGAME_H_
#define _FGAME_H_

#include <d3d9.h>
#include "d3dx9.h"
#define SAFE_RELEASE(p) {						\
if (p)				\
							{					\
							(p)->Release(); \
							}					\
}
typedef struct tagFVertex{
	float x, y, z, rhw;
	DWORD color;
}FVertex;
#define D3DFVF_CUSTOMVERTEX {D3DFVF_XYZRHW | D3DFVF_DIFFUSE}
int main(void)

{
	return 0;
}
#endif