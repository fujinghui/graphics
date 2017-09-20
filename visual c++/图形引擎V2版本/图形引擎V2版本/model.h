#ifndef _MODEL_H_
#define _MODEL_H_
#include "assist.h"

//用于简化测试的位掩码
#define PLX_RGB_MASK				0x8000
#define PLX_SHADE_MODE_MASK			0x6000
#define PLX_2SIDED_MASK				0x1000
#define PLX_COLOR_MASK				0x0fff
//用户判断那颜色模式的标记
#define PLX_COLOR_MODE_RGB_FLAG		0x8000
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000
//双面标记
#define PLX_2SIDED_FLAG				0x1000
#define PLX_1SIDED_FLAG				0x0000
//着色模式标记
#define PLX_SHADE_MODE_PURE_FLAG		0x0000	//多边形使用固定颜色
#define PLX_SHADE_MODE_FLAT_FLAG		0x2000	//恒定着色模式
#define PLX_SHADE_MODE_GOURAUD_FLAG		0x4000	//gouraud着色
#define PLX_SHADE_MODE_PHONG_FLAG		0x6000	//phong着色
//文件模型读取类有关常量
#define PARSER_STRIP_EMPTY_LINES	1	//删除空行
#define PARSER_LEAVE_EMPTY_LINES	2	//保留空行
#define PARSER_STRIP_WS_ENDS		4	//删除行尾的空格
#define PARSER_LEAVE_WS_ENDS		8	//保留行尾的空格
#define PARSER_STRIP_COMMENTS		16	//删除注释
#define PARSER_LEAVE_COMMENTS		32	//保留注释


#define PATTERN_STATE_INIT       0
#define PATTERN_STATE_RESTART    1
#define PATTERN_STATE_FLOAT      2
#define PATTERN_STATE_INT        3 
#define PATTERN_STATE_LITERAL    4
#define PATTERN_STATE_STRING     5
#define PATTERN_STATE_NEXT       6
#define PATTERN_STATE_MATCH      7
#define PATTERN_STATE_END        8
#define PATTERN_MAX_ARGS         16
#define PATTERN_BUFFER_SIZE      80
//类型标志
#define PATTERN_TOKEN_FLOAT   'f'
#define PATTERN_TOKEN_INT     'i'
#define PATTERN_TOKEN_STRING  's'
#define PATTERN_TOKEN_LITERAL '\''


char *Get_Line_PLG(char *buffer, int maxlength, FILE *fp);
float Compute_OBJECT4DV1_Radius(OBJECT4DV1_PTR obj);
int Load_OBJECT4DV1_PLG(OBJECT4DV1_PTR obj, char *filename,							//加载PLG模型文件
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot);
int Load_OBJECT4DV1_3DSASC(OBJECT4DV1_PTR obj, char *filename,
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags);		//加载ASC模型文件


void StringLtrim(char *buffer);
void StringRtrim(char *buffer);


//---------------------------V2版本函数-----------------------------------
int Init_OBJECT4DV2(OBJECT4DV2_PTR obj, int _num_vertices, int _num_polys, int _num_frames, int destroy = 0);
int Destroy_OBJECT4DV2(OBJECT4DV2_PTR obj);
float Compute_OBJECT4DV2_Radius(OBJECT4DV2_PTR obj);
int Load_OBJECT4DV2_PLG(OBJECT4DV2_PTR obj, char *filename,							//加载PLG模型文件
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags);
int Load_OBJECT4DV2_3DASC(OBJECT4DV2_PTR obj, char *filename,						//加载ASC模型文件
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags);	
int Load_OBJECT4DV2_COB(OBJECT4DV2_PTR obj, char *filename,							//加载COB模型文件
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags
	);
int Compute_OBJECT4DV2_Poly_Normals(OBJECT4DV2_PTR obj);
int Compute_OBJECT4DV2_Vertex_Normals(OBJECT4DV2_PTR obj);



class CPARSERV1{
public:
	CPARSERV1();
	~CPARSERV1();
	//重置文件系统
	int Reset();
	//打开文件
	int Open(char *filename);
	//关闭文件
	int Close();
	//读取一行
	char *GetLine(int mode);
	//设置注释字符串
	int SetComment(char *string);
	//查找模式
	int Pattern_Match(char *string, char *pattern, ...);
	//变量声明
#define PARSER_BUFFER_SIZE 128
#define PARSER_MAX_COMMENT 256
//#define PATTERN_MAX_ARGS 128
//#define PATTERN_BUFFER_SIZE 128
public:
	FILE *fstream;						//文件指针
	char buffer[PARSER_BUFFER_SIZE];	//缓存区
	int length;							//当前行的长度
	int num_lines;						//处理了多少行
	char comment[PARSER_MAX_COMMENT];

	//模式匹配参数存储空间
	char pstrings[PATTERN_MAX_ARGS][PATTERN_BUFFER_SIZE];		//任何字符串
	int num_pstrings;
	float pfloats[PATTERN_MAX_ARGS];							//任何浮点数
	int num_pfloats;

	int pints[PATTERN_MAX_ARGS];								//任何整数
	int num_pints;
};
typedef CPARSERV1 *CPARSERV1_PTR;
#endif