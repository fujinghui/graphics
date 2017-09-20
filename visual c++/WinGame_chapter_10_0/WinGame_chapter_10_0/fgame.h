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
#endif