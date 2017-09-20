#ifndef _MODEL_H_
#define _MODEL_H_
#include "assist.h"

//���ڼ򻯲��Ե�λ����
#define PLX_RGB_MASK				0x8000
#define PLX_SHADE_MODE_MASK			0x6000
#define PLX_2SIDED_MASK				0x1000
#define PLX_COLOR_MASK				0x0fff
//�û��ж�����ɫģʽ�ı��
#define PLX_COLOR_MODE_RGB_FLAG		0x8000
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000
//˫����
#define PLX_2SIDED_FLAG				0x1000
#define PLX_1SIDED_FLAG				0x0000
//��ɫģʽ���
#define PLX_SHADE_MODE_PURE_FLAG		0x0000	//�����ʹ�ù̶���ɫ
#define PLX_SHADE_MODE_FLAT_FLAG		0x2000	//�㶨��ɫģʽ
#define PLX_SHADE_MODE_GOURAUD_FLAG		0x4000	//gouraud��ɫ
#define PLX_SHADE_MODE_PHONG_FLAG		0x6000	//phong��ɫ
//�ļ�ģ�Ͷ�ȡ���йس���
#define PARSER_STRIP_EMPTY_LINES	1	//ɾ������
#define PARSER_LEAVE_EMPTY_LINES	2	//��������
#define PARSER_STRIP_WS_ENDS		4	//ɾ����β�Ŀո�
#define PARSER_LEAVE_WS_ENDS		8	//������β�Ŀո�
#define PARSER_STRIP_COMMENTS		16	//ɾ��ע��
#define PARSER_LEAVE_COMMENTS		32	//����ע��


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
//���ͱ�־
#define PATTERN_TOKEN_FLOAT   'f'
#define PATTERN_TOKEN_INT     'i'
#define PATTERN_TOKEN_STRING  's'
#define PATTERN_TOKEN_LITERAL '\''


char *Get_Line_PLG(char *buffer, int maxlength, FILE *fp);
float Compute_OBJECT4DV1_Radius(OBJECT4DV1_PTR obj);
int Load_OBJECT4DV1_PLG(OBJECT4DV1_PTR obj, char *filename,							//����PLGģ���ļ�
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot);
int Load_OBJECT4DV1_3DSASC(OBJECT4DV1_PTR obj, char *filename,
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags);		//����ASCģ���ļ�


void StringLtrim(char *buffer);
void StringRtrim(char *buffer);


//---------------------------V2�汾����-----------------------------------
int Init_OBJECT4DV2(OBJECT4DV2_PTR obj, int _num_vertices, int _num_polys, int _num_frames, int destroy = 0);
int Destroy_OBJECT4DV2(OBJECT4DV2_PTR obj);
float Compute_OBJECT4DV2_Radius(OBJECT4DV2_PTR obj);
int Load_OBJECT4DV2_PLG(OBJECT4DV2_PTR obj, char *filename,							//����PLGģ���ļ�
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags);
int Load_OBJECT4DV2_3DASC(OBJECT4DV2_PTR obj, char *filename,						//����ASCģ���ļ�
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags);	
int Load_OBJECT4DV2_COB(OBJECT4DV2_PTR obj, char *filename,							//����COBģ���ļ�
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags
	);
int Compute_OBJECT4DV2_Poly_Normals(OBJECT4DV2_PTR obj);
int Compute_OBJECT4DV2_Vertex_Normals(OBJECT4DV2_PTR obj);



class CPARSERV1{
public:
	CPARSERV1();
	~CPARSERV1();
	//�����ļ�ϵͳ
	int Reset();
	//���ļ�
	int Open(char *filename);
	//�ر��ļ�
	int Close();
	//��ȡһ��
	char *GetLine(int mode);
	//����ע���ַ���
	int SetComment(char *string);
	//����ģʽ
	int Pattern_Match(char *string, char *pattern, ...);
	//��������
#define PARSER_BUFFER_SIZE 128
#define PARSER_MAX_COMMENT 256
//#define PATTERN_MAX_ARGS 128
//#define PATTERN_BUFFER_SIZE 128
public:
	FILE *fstream;						//�ļ�ָ��
	char buffer[PARSER_BUFFER_SIZE];	//������
	int length;							//��ǰ�еĳ���
	int num_lines;						//�����˶�����
	char comment[PARSER_MAX_COMMENT];

	//ģʽƥ������洢�ռ�
	char pstrings[PATTERN_MAX_ARGS][PATTERN_BUFFER_SIZE];		//�κ��ַ���
	int num_pstrings;
	float pfloats[PATTERN_MAX_ARGS];							//�κθ�����
	int num_pfloats;

	int pints[PATTERN_MAX_ARGS];								//�κ�����
	int num_pints;
};
typedef CPARSERV1 *CPARSERV1_PTR;
#endif