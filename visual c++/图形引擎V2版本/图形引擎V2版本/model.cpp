#define _CRT_SECURE_NO_DEPRECATE
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "graph_engine.h"
#include "model.h"
extern MATV1 materials[MAX_MATERIALS];
extern int num_materials;

void StringLtrim(char *buffer){
	int sindex = 0;
	int eindex = 0;
	while (isspace(buffer[sindex]))
	{
		sindex++;
	}
	if (sindex == 0 || sindex >= PARSER_BUFFER_SIZE)
		return;
	while (buffer[sindex] != '\0')
	{
		buffer[eindex] = buffer[sindex];
		eindex++;
		sindex++;
	}
	buffer[eindex] = '\0';
}
void StringRtrim(char *buffer){
	int slength = strlen(buffer);
	if (slength == 0)
		return;
	slength--;
	while (isspace(buffer[slength]) && slength >= 0)
	{
		slength--;
	}
	buffer[slength + 1] = '\0';
}

char *Get_Line_PLG(char *buffer, int maxlength, FILE *fp){
	int index = 0;
	int length = 0;
	while (1)
	{
		if (!fgets(buffer, maxlength, fp))
		{
			return NULL;
		}
		for (length = strlen(buffer), index = 0; isspace(buffer[index]) && index < length; index++);
		if (index >= length || buffer[index] == '#')
			continue;
		return &buffer[index];
	}
}

float Compute_OBJECT4DV1_Radius(OBJECT4DV1_PTR obj){
	obj->avg_radius = 0;
	obj->max_radius = 0;
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		float dist_to_vertex = (float)sqrt(
			obj->vlist_local[vertex].x * obj->vlist_local[vertex].x +
			obj->vlist_local[vertex].y * obj->vlist_local[vertex].y +
			obj->vlist_local[vertex].z * obj->vlist_local[vertex].z
			);
		obj->avg_radius += dist_to_vertex;
		if (dist_to_vertex > obj->max_radius)
			obj->max_radius = dist_to_vertex;
	}
	obj->avg_radius /= obj->num_vertices;
	return (obj->max_radius);
}
int Load_OBJECT4DV1_PLG(OBJECT4DV1_PTR obj, char *filename,
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot){
	FILE *fp;
	char buffer[256];
	char *token_string;
	memset(obj, 0, sizeof(OBJECT4DV1));
	obj->state = OBJECT4DV1_STATE_ACTIVE | OBJECT4DV1_STATE_VISIBLE;
	obj->world_pos.x = pos->x;
	obj->world_pos.y = pos->y;
	obj->world_pos.z = pos->z;
	obj->world_pos.w = pos->w;
	if (!(fp = fopen(filename, "r")))
	{
		return 0;
	}
	if (!(token_string = Get_Line_PLG(buffer, 255, fp)))
	{
		return 0;
	} 
	//��һ�У���ȡ������ĵĶ��������ͱߵ�����
	sscanf(token_string, "%s %d %d", obj->name, &obj->num_vertices, &obj->num_polys);
	//���ض����б�
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		if (!(token_string = Get_Line_PLG(buffer, 255, fp)))
		{
			fclose(fp);
			return 0;
		}
		sscanf(token_string, "%f %f %f", &obj->vlist_local[vertex].x, &obj->vlist_local[vertex].y, &obj->vlist_local[vertex].z);
		obj->vlist_local[vertex].w = 1;
		//scale vertices
		obj->vlist_local[vertex].x *= scale->x;
		obj->vlist_local[vertex].y *= scale->y;
		obj->vlist_local[vertex].z *= scale->z;
	}

	Compute_OBJECT4DV1_Radius(obj);
	//�������б�
	int poly_surface_desc = 0;
	int poly_num_verts = 0;
	char tmp_string[8];
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		if (!(token_string = Get_Line_PLG(buffer, 255, fp)))
		{
			fclose(fp);
			return 0;
		}
		sscanf(token_string, "%s %d %d %d %d", tmp_string, &poly_num_verts, &obj->plist[poly].vert[0], &obj->plist[poly].vert[1], &obj->plist[poly].vert[2]);
		if (tmp_string[0] == '0' && toupper(tmp_string[1]) == 'X')
			sscanf(tmp_string, "%x", &poly_surface_desc);
		else
			poly_surface_desc = atoi(tmp_string);
		//���ö�������ָ��֮ǰ��ȡ�Ķ�������
		obj->plist[poly].state = POLY4DV1_STATE_ACTIVE;
		obj->plist[poly].attr = 0;
		obj->plist[poly].vlist = obj->vlist_local;
		
		if ((poly_surface_desc & PLX_2SIDED_FLAG))
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_2SIDED);
		}
		else
		{
			//����
		}
		if ((poly_surface_desc & PLX_COLOR_MODE_RGB_FLAG))
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_RGB16);
			
			int red = ((poly_surface_desc & 0x0f00) >> 8);
			int green = ((poly_surface_desc & 0x00f0) >> 4);
			int blue = (poly_surface_desc & 0x000f);
			
			obj->plist[poly].color = RGB(red * 32, green * 32, blue * 32);
			obj->plist[poly].color_light_trans = obj->plist[poly].color;
		}
		else
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_8BITCOLOR);
			obj->plist[poly].color = (poly_surface_desc & 0x00ff);
		}

		int shade_mode = (poly_surface_desc & PLX_SHADE_MODE_MASK);
		switch (shade_mode)
		{
		case PLX_SHADE_MODE_PURE_FLAG:
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PURE);
			break;
		case PLX_SHADE_MODE_FLAT_FLAG:
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_FLAT);
			break;
		case PLX_SHADE_MODE_GOURAUD_FLAG:
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_GOURAUD);
			break;
		case PLX_SHADE_MODE_PHONG_FLAG:
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PHONG);
			break;
		}
		//SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_FLAT);
	}
	fclose(fp);
	return 0;
}

int Load_OBJECT4DV1_3DSASC(OBJECT4DV1_PTR obj, char *filename,
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags){
	CPARSERV1 parser;
	char seps[16];
	char token_buffer[256];
	char *token;
	int r = 0, g = 0, b = 0;
	memset(obj, 0, sizeof(OBJECT4DV1));
	obj->state = OBJECT4DV1_STATE_ACTIVE | OBJECT4DV1_STATE_VISIBLE;		//���õ�ǰ����״̬Ϊ���Ѽ���and�ɼ�
	//step 1:set position of object is caller requested position
	if (pos)
	{
		obj->world_pos.x = pos->x;
		obj->world_pos.y = pos->y;
		obj->world_pos.z = pos->z;
		obj->world_pos.w = pos->w;
	}
	else
	{
		obj->world_pos.x = 0; obj->world_pos.y = 0; obj->world_pos.z = 0; obj->world_pos.w = 1;
	}
	//Step 2:open the file for reading using the parser
	if (!parser.Open(filename))
	{
		return 0;
	}
	//step 1: ��ȡ���������
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			//parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['Named'] ['object:']"))
		{
			strcpy(token_buffer, parser.buffer);			//���ƶ�ȡ����һ����Ϣ
			strcpy(seps, "\"");
			strtok(token_buffer, seps);

			token = strtok(NULL, seps);
			strcpy(obj->name, token);
			break;
		}
	}		//end while
	//step 2:��ȡ����Ķ���ͱߵ�����

	while (1)
	{
		if(!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['Tri-mesh,']['Vertices:'][i]['Faces:'][i]"))
		{
			obj->num_vertices = parser.pints[0];			//��ȡ��������
			obj->num_polys = parser.pints[1];				//��ȡ�ߵ�����
			break;
		}	//end if
	}	//end while
	//step 3:Vertex list
	while (1)
	{
		//��ȡ��һ��
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			//��ȡ����
			return 0;
		}
		//�������˶����б�(Vertex list)��ʼ������ʼ��ʽ������������
		if (parser.Pattern_Match(parser.buffer, "['Vertex']['list:']"))
		{
			break;
		}
	}
	//step 4:��ʼ��������
	//�����ʽ(ԭ��ʽ):Vertex: d X:d.d Y:d.d Z:d.d
	//�����ʽ(����ע��):Vertex:������ X:x��ֲ����� Y:y��ֲ����� Z:z��ֲ�����
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		while (1)
		{
			if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				parser.Close();
				return 0;
			}
			StripChars(parser.buffer, parser.buffer, ":XYZ");		//����':0',"X","Y","Z"���ĸ��ַ�
			if (parser.Pattern_Match(parser.buffer, "['Vertex'][i][f][f][f]"))
			{
				obj->vlist_local[vertex].x = parser.pfloats[0];
				obj->vlist_local[vertex].y = parser.pfloats[1];
				obj->vlist_local[vertex].z = parser.pfloats[2];
				
				//sprintf_s(parser.buffer, "%f %f %f\0", parser.pfloats[0], parser.pfloats[1], parser.pfloats[2]);
				//MessageBox(NULL, parser.buffer, "title", MB_OK);
				
				obj->vlist_local[vertex].w = 1;
				float temp_f;
				if (vertex_flags & VERTEX_FLAGS_INVERT_X)
				{
					obj->vlist_local[vertex].x = -obj->vlist_local[vertex].x;
				}
				if (vertex_flags & VERTEX_FLAGS_INVERT_Y)
				{
					obj->vlist_local[vertex].y = -obj->vlist_local[vertex].y;
				}
				if (vertex_flags & VERTEX_FLAGS_INVERT_Z)
				{
					obj->vlist_local[vertex].z = -obj->vlist_local[vertex].z;
				}
				//swap
				if (vertex_flags & VERTEX_FLAGS_SWAP_YZ)
				{
					SWAP(obj->vlist_local[vertex].y, obj->vlist_local[vertex].z, temp_f);
				}
				if (vertex_flags & VERTEX_FLAGS_SWAP_XZ)
				{
					SWAP(obj->vlist_local[vertex].x, obj->vlist_local[vertex].z, temp_f);
				}
				if (vertex_flags & VERTEX_FLAGS_SWAP_XY)
				{
					SWAP(obj->vlist_local[vertex].x, obj->vlist_local[vertex].y, temp_f);
				}
				if (scale)
				{
					obj->vlist_local[vertex].x *= scale->x;
					obj->vlist_local[vertex].y *= scale->y;
					obj->vlist_local[vertex].z *= scale->z;
				}
				break;

			}	//end if
		}	//end while
	}	//end for
	//step 4:������
	//�Ƚ�������Face list��
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		//ֱ��ƥ�䵽Face List:���Ƴ���ѭ��������һֱ����һ�У�ֱ�����������߶��뵽�ļ�ĩβ
		if (parser.Pattern_Match(parser.buffer, "['Face']['list:']"))
		{
			break;
		}
	}
	//step 5:��ʼ������
	//��ʽ��
	//��һ�У�Face 0: A:2 B:3 C:1 AB:1 BC:1 CA:1
	//�ڶ��У�Material:"r255g255b255"
	//�����У�Smoothing: 1
	int poly_surafce_desc = 0;
	int poly_num_verts = 0;
	char tmp_string[8];
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		//��һ��
		while (1)
		{
			if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				parser.Close();
				return 0;
			}
			StripChars(parser.buffer, parser.buffer, ":ABC");
			//������
			if (parser.Pattern_Match(parser.buffer, "['Face'][i][i][i][i]"))
			{
				if (vertex_flags & VERTEX_FLAGS_INVERT_WINDING_ORDER)
				{
					poly_num_verts = 3;
					obj->plist[poly].vert[0] = parser.pints[3];
					obj->plist[poly].vert[1] = parser.pints[2];
					obj->plist[poly].vert[2] = parser.pints[1];
				}
				else
				{
					poly_num_verts = 3;
					obj->plist[poly].vert[0] = parser.pints[1];
					obj->plist[poly].vert[1] = parser.pints[2];
					obj->plist[poly].vert[2] = parser.pints[3];
				}

				obj->plist[poly].vlist = obj->vlist_local;
				break;
			}	// end if
		}	//end while
		//�ڶ���
		while (1)
		{
			if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				parser.Close();
				return 0;
			}
			//MessageBox(NULL, parser.buffer, "title", MB_OK);
			ReplaceChars(parser.buffer, parser.buffer, ":\"rgba", ' ');
			//MessageBox(NULL, parser.buffer, "title", MB_OK);
			//return 0;
			if (parser.Pattern_Match(parser.buffer, "[i] [i] [i]"))
			{
				r = parser.pints[0];
				g = parser.pints[1];
				b = parser.pints[2];
				SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_RGB16);					//������ɫλ��״̬
			}
			else
			{
				SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_RGB16);
			}
			obj->plist[poly].color = obj->plist[poly].color_light_trans = RGB(r, g, b);

			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_FLAT);
			obj->plist[poly].state = POLY4DV1_STATE_ACTIVE;
			break;
		}	//end if
	}	//end for
	Compute_OBJECT4DV1_Radius(obj);
	return 1;
}


//CPARSERV1���ʵ�ִ���
CPARSERV1::CPARSERV1(){
	fstream = NULL;
	Reset();
}

CPARSERV1::~CPARSERV1(){
	Reset();
}

int CPARSERV1::Reset(){
	if (fstream)
		fclose(fstream);
	fstream = NULL;
	memset(buffer, 0, sizeof(buffer));
	length = 0;
	num_lines = 0;
	//strcpy(comment);
	return 1;
}

int CPARSERV1::Open(char *filename){
	Reset();
	if ((fstream = fopen(filename, "r")) != NULL)
	{
		return 1;
	}
	return 0;
}

int CPARSERV1::Close(){
	return (Reset());
}

char *CPARSERV1::GetLine(int mode){
	char *string = NULL;
	if (fstream)
	{
		if (mode & PARSER_STRIP_EMPTY_LINES)			//ɾ������
		{
			while (1)
			{
				if ((string = fgets(buffer, PARSER_BUFFER_SIZE, fstream)) == NULL)	//ÿ�ζ�ȡһ��
					break;
				int slength = strlen(string);
				int sindex = 0;
				while (isspace(string[sindex]) && sindex < slength)
				{
					sindex++;
				}
				//ȥ���ո��
				if ((slength - sindex) > 0)
				{
					memmove((void*)buffer, (void*)&string[sindex], (slength - sindex) + 1);
					string = buffer;									//��sringָ���ȥ�ո�Ķ������ݻ�����
					slength = strlen(string);							//������ȥ�ո���ַ����ĳ���
					if (mode & PARSER_STRIP_COMMENTS)					//ɾ��ע��
					{
						char *comment_string = strstr(string, comment);	//�ж�comment�Ƿ���string���Ӵ�(commentΪע��)
						if (comment_string == NULL)						//��������
							break;
						int cindex = (int)(comment_string - string);	
						if (cindex == 0)
							continue;									//û��ע�ͣ�����ɾ��
						else
						{
							comment_string[0] = '\0';
							break;
						}
						string[cindex] = '\0';
						break;
					}
					break;
				}
			}
		}
		else
		{
			string = fgets(buffer, PARSER_BUFFER_SIZE, fstream);
		}
		if (string != NULL)
		{
			num_lines++;
			if (mode & PARSER_STRIP_WS_ENDS)
			{
				StringLtrim(buffer);
				StringRtrim(buffer);
			}
			length = strlen(buffer);
			return (string);
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
	return string;
}	//end GetLine

//��string�ַ�������ȡģʽpattern��������Ϣ
int CPARSERV1::Pattern_Match(char *string, char *pattern, ...){
	char token_type[PATTERN_MAX_ARGS];							//������������� f, i, s, l
	char token_string[PATTERN_MAX_ARGS][PATTERN_BUFFER_SIZE];	//for literal string this holds
	char token_operator[PATTERN_MAX_ARGS];
	int token_numeric[PATTERN_MAX_ARGS];

	char buffer[PARSER_BUFFER_SIZE];

	//ƥ���ַ����޷�����
	if ((!string || strlen(string) == 0) || (!pattern || strlen(pattern) == 0))
	{
		return 0;
	}
	//���ƶ�ȡ����һ��
	strcpy(buffer, string);

	int tok_start = 0,
		tok_end = 0,
		tok_restart = 0,
		tok_first_pass = 0,
		num_tokens = 0;

	int length_pattern, length_string;
	length_string = strlen(string);
	length_pattern = strlen(pattern);
	//�ڸ�ѭ���У���Ҫ����pattern�ַ������������ģʽ
	while (1)
	{
		//ɾ���ո�
		while (isspace(pattern[tok_start]) && tok_start < length_pattern)
		{
			tok_start++;
		}
		if (tok_start >= strlen(pattern))
		{
			break;
		}
		if (pattern[tok_start] == '[')
		{
			switch (pattern[tok_start + 1])
			{
			case PATTERN_TOKEN_FLOAT:							//'f'
				if (pattern[tok_start + 2] != ']')				//����������ֱ���˳�����
				{
					return 0;
				}
				tok_start += 3;
				//insert a float into pattern
				token_type[num_tokens] = PATTERN_TOKEN_FLOAT;
				strcpy(token_string[num_tokens], "");
				token_operator[num_tokens] = 0;
				token_numeric[num_tokens] = 0;
				num_tokens++;
				break;
			case PATTERN_TOKEN_INT:							//'i'
				if (pattern[tok_start + 2] != ']')			//�����׵ģ���������ͬ
					return 0;
				tok_start += 3;
				//insert a int into pattern
				token_type[num_tokens] = PATTERN_TOKEN_INT;
				strcpy(token_string[num_tokens], "");
				token_operator[num_tokens] = 0;
				token_numeric[num_tokens] = 0;
				num_tokens++;
				break;
			case PATTERN_TOKEN_LITERAL:										//'\''	example:['text']
				tok_start += 2;												//tok_start֮ǰָ��'['
				tok_end = tok_start;
				while (pattern[tok_end] != PATTERN_TOKEN_LITERAL)			//����''�м���ַ���
					tok_end++;
				if (pattern[tok_end + 1] != ']')							//you know. up same 
					return 0;
				//insert a string into pattern
				memcpy(token_string[num_tokens], &pattern[tok_start], (tok_end - tok_start));
				token_string[num_tokens][(tok_end - tok_start)] = '\0';

				token_type[num_tokens] = PATTERN_TOKEN_LITERAL;				//type is literal
				token_operator[num_tokens] = 0;
				token_numeric[num_tokens] = 0;

				//���һ��ɨ����
				tok_start = tok_end + 2;									//������һ��ɨ���ָ��λ��
				num_tokens++;												//token������һ
				break;
			case PATTERN_TOKEN_STRING:										//[s] s�ɱ�ʾһ������
				//tok_start == [     tok_start + 1 ��һ������	tok_start+2 ��һ��������			//example:[a>0] 
				if (pattern[tok_start + 2] == '=' || pattern[tok_start + 2] == '>' || pattern[tok_start + 2] == '<')
				{
					tok_end = tok_start + 3;								//
					while (isdigit(pattern[tok_end]) && pattern[tok_end] != '\0')
						tok_end++;
					if (pattern[tok_end] != ']')
						return 0;
					memcpy(buffer, &pattern[tok_start + 3], (tok_end - tok_start - 3));
					buffer[tok_end - tok_start - 3] = 0;
					//insert a string into pattern
					token_type[num_tokens] = PATTERN_TOKEN_STRING;
					strcpy(token_string[num_tokens], "");
					token_operator[num_tokens] = pattern[tok_start + 2];	//���ò���
					token_numeric[num_tokens] = atoi(buffer);
					
					//return pattern[tok_start+3]-'0';// token_numeric[num_tokens];

					num_tokens++;	
					tok_start = tok_end + 1;
				}
				else
					break;
				break;
			default:
				break;
			}		//end swtich
		}		//end if '['
		if (tok_start >= strlen(pattern))
			break;
	}		//end while
	int pattern_state = PATTERN_STATE_INIT;							//���õ�ǰ״̬����ʼ��
	int curr_tok = 0;
	char token[PATTERN_BUFFER_SIZE];
	int i;
	float f;
	//memcpy(buffer, string, strlen(string) * sizeof(char));
	strcpy(buffer, string);
	//a new 
	while (1)
	{
		switch (pattern_state)
		{
		case PATTERN_STATE_INIT:
			tok_start = 0;
			tok_end = 0;
			tok_restart = 0;
			tok_first_pass = 1;
			curr_tok = 0;

			num_pints = num_pfloats = num_pstrings = 0;
			pattern_state = PATTERN_STATE_RESTART;					//������һ��״̬������
			break;
		case PATTERN_STATE_RESTART:
			curr_tok = 0;
			tok_first_pass = 1;
			if (tok_end >= strlen(buffer))		//��������
			{
				return 0;
			}
			tok_start = tok_end = tok_restart;
			pattern_state = PATTERN_STATE_NEXT;						//������һ��״̬��next
			break;
		case PATTERN_STATE_NEXT:
			if (curr_tok >= num_tokens)			//��������token���������Ѵ��ڵ�tokens��������ֱ�ӽ�����һ��
			{
				pattern_state = PATTERN_STATE_MATCH;
			}
			else
			{
				if (tok_end >= strlen(buffer))
				{
					return 0;
				}
				tok_start = tok_end;
				while (isspace(buffer[tok_start])) tok_start++;			//ɾ���ո�
				tok_end = tok_start;
				while (!isspace(buffer[tok_end]) && tok_end < strlen(buffer)) tok_end++;	//�ҵ������ǿո����������(tok_end - tok_start)
				memcpy(token, &buffer[tok_start], tok_end - tok_start);	//�����ǿո������
				token[tok_end - tok_start] = 0;
				if (strlen(token) == 0)
					return 0;
				if (tok_first_pass)
				{
					tok_first_pass = 0;
					tok_restart = tok_end;								//�����´ν���string(buffer)��λ��
				}	//end if
				switch (token_type[curr_tok])
				{
				case PATTERN_TOKEN_FLOAT:
					pattern_state = PATTERN_STATE_FLOAT;
					break;
				case PATTERN_TOKEN_INT:
					pattern_state = PATTERN_STATE_INT;
					break;
				case PATTERN_TOKEN_STRING:
					pattern_state = PATTERN_STATE_STRING;
					break;
				case PATTERN_TOKEN_LITERAL:
					pattern_state = PATTERN_STATE_LITERAL;
					break;
				default:
					break;
				}//end switch
			}//end if else
			break;
		case PATTERN_STATE_FLOAT:
			f = IsFloat(token);
			if (f != FLT_MIN)
			{
				pfloats[num_pfloats++] = f;
				curr_tok++;
				pattern_state = PATTERN_STATE_NEXT;
			}
			else
			{
				pattern_state = PATTERN_STATE_RESTART;
			}
			break;
		case PATTERN_STATE_INT:
			i = IsInt(token);
			if (i != INT_MIN)
			{
				pints[num_pints++] = i;
				curr_tok++;
				pattern_state = PATTERN_STATE_NEXT;
			}
			else
			{
				pattern_state = PATTERN_STATE_RESTART;
			}
			break; 
		case PATTERN_STATE_LITERAL:
			if (strcmp(token, token_string[curr_tok]) == 0)
			{
				strcpy(pstrings[num_pstrings++], token);
				curr_tok++;
				pattern_state = PATTERN_STATE_NEXT;
			}
			else
			{
				pattern_state = PATTERN_STATE_RESTART;
			}
			break;
		case PATTERN_STATE_STRING:
			switch (token_operator[curr_tok])
			{
			case '=':
				if (strlen(token) == token_numeric[curr_tok])
				{
					strcpy(pstrings[num_pstrings++], token);
					curr_tok++;
					pattern_state = PATTERN_STATE_NEXT;
				}
				else
				{
					pattern_state = PATTERN_STATE_RESTART;
				}
				break;
			case '>':
				if (strlen(token) > token_numeric[curr_tok])
				{
					strcpy(pstrings[num_pstrings++], token);
					curr_tok++;
					pattern_state = PATTERN_STATE_NEXT;
				}
				else
				{
					pattern_state = PATTERN_STATE_RESTART;
				}
				break;
			case '<':
				if (strlen(token) < token_numeric[curr_tok])
				{
					strcpy(pstrings[num_pstrings++], token);
					curr_tok++;
					pattern_state = PATTERN_STATE_NEXT;
				}
				else
				{
					pattern_state = PATTERN_STATE_RESTART;
				}
				break;
			}
			break;
		case PATTERN_STATE_MATCH:
			return(1);
			break;
		case PATTERN_STATE_END:
			return 0; 

			break;
		}//end switch
	}
	
	return 0;
}

int CPARSERV1::SetComment(char *string){
	if (strlen(string) < PARSER_MAX_COMMENT)
	{
		strcpy(comment, string);
		return 1;
	}
	return 0;
}



//V2�汾����

float Compute_OBJECT4DV2_Radius(OBJECT4DV2_PTR obj){

	//���ð뾶
	obj->avg_radius[obj->curr_frame] = 0;
	obj->max_radius[obj->curr_frame] = 0;
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		float dist_to_vertex = sqrt(
			obj->vlist_local[vertex].x * obj->vlist_local[vertex].x + 
			obj->vlist_local[vertex].y * obj->vlist_local[vertex].y +
			obj->vlist_local[vertex].z * obj->vlist_local[vertex].z
			);
		obj->avg_radius[obj->curr_frame] += dist_to_vertex;
		if (dist_to_vertex > obj->max_radius[obj->curr_frame])
		{
			obj->max_radius[obj->curr_frame] = dist_to_vertex;
		}
	}	
	//���������ƽ���뾶
	obj->avg_radius[obj->curr_frame] /= obj->num_vertices;
	return (obj->max_radius[0]);
}

int Init_OBJECT4DV2(OBJECT4DV2_PTR obj, int _num_vertices, int _num_polys, int _num_frames, int destroy){
	if (destroy)
	{
		Destroy_OBJECT4DV2(obj);
	}

	//ת��ǰ�ĵ�
	if (!(obj->vlist_local = (VERTEX4DTV1_PTR)malloc(sizeof(VERTEX4DTV1)* _num_vertices * _num_frames)))
		return 0;
	memset((void*)obj->vlist_local, 0, sizeof(VERTEX4DTV1)* _num_vertices * _num_frames);
	//MessageBox(NULL, "tiandao", "tiandao", MB_OK);
	//ת����ĵ�
	if (!(obj->vlist_trans = (VERTEX4DTV1_PTR)malloc(sizeof(VERTEX4DTV1)* _num_polys * _num_frames)))
		return 0;
	memset((void*)obj->vlist_trans, 0, sizeof(VERTEX4DTV1)*_num_polys*_num_frames);
	//��������ĵ�
	if (!(obj->tlist = (POINT2D_PTR)malloc(sizeof(POINT2D)* _num_polys * 3)))
		return 0;
	memset((void*)obj->tlist, 0, sizeof(POINT2D)* _num_polys * 3);

	//���ÿһ��֡�İ뾶��С��ƽ���뾶��
	if (!(obj->avg_radius = (float*)malloc(sizeof(float)*_num_frames)))
		return 0;
	memset((void*)obj->avg_radius, 0, sizeof(float)*_num_frames);
	//��ͬ�����뾶��
	if (!(obj->max_radius = (float*)malloc(sizeof(float)*_num_frames)))
		return 0;
	memset((void*)obj->max_radius, 0, sizeof(float)*_num_frames);

	if (!(obj->plist = (POLY4DV2_PTR)malloc(sizeof(POLY4DV2)*_num_polys)))
		return 0;
	memset((void*)obj->plist, 0, sizeof(POLY4DV2)*_num_polys);

	//�������������ͷ��ָ��
	obj->head_vlist_local = obj->vlist_local;
	obj->head_vlist_trans = obj->vlist_trans;

	obj->num_frames = _num_frames;							//��ǰ��������ж���֡
	obj->num_polys = _num_polys;							//����ε�������һ֡��
	obj->num_vertices = _num_vertices;						//�����������һ֡��
	obj->total_vertices = _num_vertices * _num_frames;		//���еĶ��㣨����֡��
	return 1;
}
int Destroy_OBJECT4DV2(OBJECT4DV2_PTR obj){
	if (obj->head_vlist_local)					//�ͷ�ת��֮ǰ����
		free(obj->head_vlist_local);
	if (obj->head_vlist_trans)					//�ͷ�ת��֮�󶥵�
		free(obj->head_vlist_trans);
	if (obj->tlist)								//�ͷ��������궥��
		free(obj->tlist);
	if (obj->plist)								//�ͷŶ����
		free(obj->plist); 
	if (obj->avg_radius)
		free(obj->avg_radius);
	if (obj->max_radius)
		free(obj->max_radius);
	memset((void*)obj, 0, sizeof(OBJECT4DV2));
	return 1;
}


int Load_OBJECT4DV2_PLG(OBJECT4DV2_PTR obj, char *filename,							//����PLGģ���ļ�
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags){

	FILE *fp;
	char buffer[256];
	char *token_string;

	//��1���������������ݲ�������г�ʼ��
	memset(obj, 0, sizeof(OBJECT4DV2));
	//�������״̬����Ϊ��Ϳɼ�
	obj->state = OBJECT4DV2_STATE_ACTIVE | OBJECT4DV2_STATE_VISIBLE;
	//���������λ��
	obj->world_pos.x = pos->x;
	obj->world_pos.y = pos->y;
	obj->world_pos.z = pos->z;
	obj->world_pos.w = pos->w;
	//�������������֡��
	obj->num_frames = 1;
	obj->curr_frame = 0;
	obj->attr = OBJECT4DV2_ATTR_SINGLE_FRAME;
	//��2�������ļ��Ա���ж�ȡ 
	if (!(fp = fopen(filename, "r")))
	{
		return 0;
	}
	//��3��:��ȡ��һ���ַ�������������������
	if (!(token_string = Get_Line_PLG(buffer, 255, fp)))
	{
		return 0;
	}
	


	sscanf(token_string, "%s %d %d", obj->name, &obj->num_vertices, &obj->num_polys);
	
	if (!(Init_OBJECT4DV2(obj, obj->num_vertices, obj->num_polys, obj->num_frames)))
	{
		Destroy_OBJECT4DV2(obj);			//��ʼ��������ڴ���Դʧ�ܣ��ͷ�������ɹ����ڴ���Դ
		return 0;
	}
	
	//���ض����б�
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		if (!(token_string = Get_Line_PLG(buffer, 255, fp)))
		{
			fclose(fp);
			return 0;
		}
		sscanf(token_string, "%f %f %f", &obj->vlist_local[vertex].x, &obj->vlist_local[vertex].y, &obj->vlist_local[vertex].z);
		obj->vlist_local[vertex].w = 1;
		//scale vertices
		obj->vlist_local[vertex].x *= scale->x;
		obj->vlist_local[vertex].y *= scale->y;
		obj->vlist_local[vertex].z *= scale->z;
		SET_BIT(obj->vlist_local[vertex].attr, VERTEX4DTV1_ATTR_POINT);			//�������ֵ
	}
	Compute_OBJECT4DV2_Radius(obj);
	
	//�������б�
	int poly_surface_desc = 0;
	int poly_num_verts = 0;
	char tmp_string[8];
	//��ȡ����ʼ��ÿһ����
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		if (!(token_string = Get_Line_PLG(buffer, 255, fp)))
		{
			fclose(fp);
			return 0;
		}
		sscanf(token_string, "%s %d %d %d %d", tmp_string, &poly_num_verts, &obj->plist[poly].vert[0], &obj->plist[poly].vert[1], &obj->plist[poly].vert[2]);
		if (tmp_string[0] == '0' && toupper(tmp_string[1]) == 'X')
			sscanf(tmp_string, "%x", &poly_surface_desc);
		else
			poly_surface_desc = atoi(tmp_string);

		//���ö�������ָ��֮ǰ��ȡ�Ķ�������
		//obj->plist[poly].state = POLY4DV1_STATE_ACTIVE;
		//obj->plist[poly].attr = 0;
		obj->plist[poly].vlist = obj->vlist_local;

		if ((poly_surface_desc & PLX_2SIDED_FLAG))
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_2SIDED);
		}
		else
		{
			//����
		}
		if ((poly_surface_desc & PLX_COLOR_MODE_RGB_FLAG))
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_RGB16);

			int red = ((poly_surface_desc & 0x0f00) >> 8);
			int green = ((poly_surface_desc & 0x00f0) >> 4);
			int blue = (poly_surface_desc & 0x000f);

			obj->plist[poly].color = RGB(red * 32, green * 32, blue * 32);

		}
		else
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_8BITCOLOR);
			obj->plist[poly].color = RGB(255, 255, 255);// (poly_surface_desc & 0x00ff);
		}

		int shade_mode = (poly_surface_desc & PLX_SHADE_MODE_MASK);
		switch (shade_mode)
		{
		case PLX_SHADE_MODE_PURE_FLAG:
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PURE);
			break;
		case PLX_SHADE_MODE_FLAT_FLAG:
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_FLAT);
			break;
		case PLX_SHADE_MODE_GOURAUD_FLAG:											//������ɫ
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_GOURAUD);
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[0]].attr, VERTEX4DTV1_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[1]].attr, VERTEX4DTV1_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[2]].attr, VERTEX4DTV1_ATTR_NORMAL);

			break;
		case PLX_SHADE_MODE_PHONG_FLAG:												//����ɫ
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PHONG);

			SET_BIT(obj->vlist_local[obj->plist[poly].vert[0]].attr, VERTEX4DTV1_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[1]].attr, VERTEX4DTV1_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[2]].attr, VERTEX4DTV1_ATTR_NORMAL);

			break;
		default:
			break;
		}
		SET_BIT(obj->plist[poly].attr, POLY4DV2_ATTR_DISABLE_MATERIAL);
		obj->plist[poly].state = POLY4DV2_STATE_ACTIVE;
		obj->plist[poly].vlist = obj->vlist_local;
		obj->plist[poly].tlist = obj->tlist;


		//SET_BIT(obj->plist[poly].attr, POLY4DV2_ATTR_SHADE_MODE_FLAT);
	}


	//����ÿ����ķ���������(compute the polygon normal lengths)
	Compute_OBJECT4DV2_Poly_Normals(obj);
	//����ÿ������ķ�����Ϊ��֮���gouraud��Ⱦ
	Compute_OBJECT4DV2_Vertex_Normals(obj);

	fclose(fp);


	return 1;
}
int Compute_OBJECT4DV2_Poly_Normals(OBJECT4DV2_PTR obj){
	if (!obj)
		return 0;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		int vindex_0 = obj->plist[poly].vert[0];
		int vindex_1 = obj->plist[poly].vert[1];
		int vindex_2 = obj->plist[poly].vert[2];
		VECTOR4D u, v, n;
		VECTOR4D_Build(&obj->vlist_local[vindex_0].v, &obj->vlist_local[vindex_1].v, &u);
		VECTOR4D_Build(&obj->vlist_local[vindex_0].v, &obj->vlist_local[vindex_2].v, &v);
		VECTOR4D_Cross(&u, &v, &n);							//��ǰƽ��ķ�����

		obj->plist[poly].nlength = VECTOR4D_Length(&n);		//�������ĳ���
	}
	return 1;
}
int Compute_OBJECT4DV2_Vertex_Normals(OBJECT4DV2_PTR obj){
	if (!obj)
		return 0;
	int polys_touch_vertex[OBJECT4DV2_MAX_VERTICES];
	memset((void*)polys_touch_vertex, 0, sizeof(int) * OBJECT4DV2_MAX_VERTICES);

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		if (obj->plist[poly].attr & POLY4DV2_ATTR_SHADE_MODE_GOURAUD)
		{
			int vindex_0 = obj->plist[poly].vert[0];
			int vindex_1 = obj->plist[poly].vert[1];
			int vindex_2 = obj->plist[poly].vert[2];
			VECTOR4D u, v, n;
			VECTOR4D_Build(&obj->vlist_local[vindex_0].v, &obj->vlist_local[vindex_1].v, &u);
			VECTOR4D_Build(&obj->vlist_local[vindex_0].v, &obj->vlist_local[vindex_2].v, &v);

			VECTOR4D_Cross(&u, &v, &n);					//��������淨����
			
			//ͳ��ÿ���㱻�����˶��ٴ�
			polys_touch_vertex[vindex_0] ++;
			polys_touch_vertex[vindex_1] ++;
			polys_touch_vertex[vindex_2] ++;

			//���㷨�߼��ϵ�ǰ��ķ�����(��һ�������ڲ�ͬ�ı������õ�Խ��ʱ�����Ķ��㷨����Խ����
			//��Ȩ�������ÿ�����׵ķ�����)
			VECTOR4D_Add(&obj->vlist_local[vindex_0].n, &n, &obj->vlist_local[vindex_0].n);
			VECTOR4D_Add(&obj->vlist_local[vindex_1].n, &n, &obj->vlist_local[vindex_1].n);
			VECTOR4D_Add(&obj->vlist_local[vindex_2].n, &n, &obj->vlist_local[vindex_2].n);
		}
	}
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		if (polys_touch_vertex[vertex] >= 1)
		{
			//��ǰ�������ķ����������ֱ��ڵ�ǰ������㱻���õĴ���
			obj->vlist_local[vertex].nx /= polys_touch_vertex[vertex];
			obj->vlist_local[vertex].ny /= polys_touch_vertex[vertex];
			obj->vlist_local[vertex].nz /= polys_touch_vertex[vertex];
			//��׼����ת��Ϊ��λ��λ������
			VECTOR4D_Normalize(&obj->vlist_local[vertex].n);
		}
	}
	return 1;

}

int Load_OBJECT4DV2_3DASC(OBJECT4DV2_PTR obj, char *filename,						//����ASCģ���ļ�
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags){
	CPARSERV1 parser;
	char seps[16];
	char token_buffer[256];
	char *token;
	int r, g, b;
	memset(obj, 0, sizeof(OBJECT4DV2));
	obj->state = OBJECT4DV2_STATE_ACTIVE | OBJECT4DV2_STATE_VISIBLE;
	obj->num_frames = 1;													//���õ�ǰ����ṹ���֡������Ϊ1��������ǰ����Ϊһ����̬���壬ֻ��һ������֡
	obj->curr_frame = 0;													//��ǰ����ʾ�Ķ�����֡
	obj->attr = OBJECT4DV2_ATTR_SINGLE_FRAME;								//�������������Ϊ��֡���������

	if (pos)
	{
		obj->world_pos.x = pos->x;
		obj->world_pos.y = pos->y;
		obj->world_pos.z = pos->z;
		obj->world_pos.w = pos->w;
	}
	else
	{
		obj->world_pos.x = obj->world_pos.y = obj->world_pos.z = 0;
		obj->world_pos.w = 1;
	}
	if (!parser.Open(filename))
	{
		return 0;
	}
	//1����ȡ���������
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		//ƥ��ɹ�
		if (parser.Pattern_Match(parser.buffer, "['Named'] ['object:']"))
		{
			strcpy(token_buffer, parser.buffer);
			strcpy(seps, "\"");
			strtok(token_buffer, seps);
			token = strtok(NULL, seps);
			strcpy(obj->name, token);
			break;
		}
	}
	//2����ȡ����Ķ����������������
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['Tri-mesh,'] ['Vertices:'] [i] ['Faces:'] [i]"))
		{
			obj->num_vertices = parser.pints[0];
			obj->num_polys = parser.pints[1];
			break;
		}

	}
//	TCHAR text[128];
//	sprintf_s(text, "vertices:%d polys:%d\0", obj->num_vertices, obj->num_polys);
//	MessageBox(NULL, text, "TITLE", MB_OK);
	//������Щ�����������������µ�ǰ������ڴ档�������㹻����ڴ�ռ�
	if (!Init_OBJECT4DV2(obj, obj->num_vertices, obj->num_polys, 1, 0))
	{
		parser.Close();
		return 0;
	}
//	MessageBox(NULL, "Init OBJECT Success!", "title", MB_OK);
	//3����ȡƥ�䵽��������
	//Vertex list:
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['Vertex']['list:']"))
		{
			break;
		}
	}
	//4����ȡ��������
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		while (1)
		{
			if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				parser.Close();
				return 0;
			}
			StripChars(parser.buffer, parser.buffer, ":XYZ");
			if (parser.Pattern_Match(parser.buffer, "['Vertex'] [i] [f] [f] [f]"))
			{
				obj->vlist_local[vertex].x = parser.pfloats[0];
				obj->vlist_local[vertex].y = parser.pfloats[1];
				obj->vlist_local[vertex].z = parser.pfloats[2];
				obj->vlist_local[vertex].w = 1;
				float temp_f;
				if (vertex_flags & VERTEX_FLAGS_INVERT_X)
					obj->vlist_local[vertex].x = -obj->vlist_local[vertex].x;
				if (vertex_flags & VERTEX_FLAGS_INVERT_Y)
					obj->vlist_local[vertex].y = -obj->vlist_local[vertex].y;
				if (vertex_flags & VERTEX_FLAGS_INVERT_Z)
					obj->vlist_local[vertex].z = -obj->vlist_local[vertex].z;
				if (vertex_flags & VERTEX_FLAGS_SWAP_YZ)
					SWAP(obj->vlist_local[vertex].y, obj->vlist_local[vertex].z, temp_f);
				if (vertex_flags & VERTEX_FLAGS_SWAP_XZ)
					SWAP(obj->vlist_local[vertex].x, obj->vlist_local[vertex].z, temp_f);
				if (vertex_flags & VERTEX_FLAGS_SWAP_XY)
					SWAP(obj->vlist_local[vertex].x, obj->vlist_local[vertex].y, temp_f);

				if (scale)
				{
					obj->vlist_local[vertex].x *= scale->x;
					obj->vlist_local[vertex].y *= scale->y;
					obj->vlist_local[vertex].z *= scale->z;
				}
				//���ö�������
				SET_BIT(obj->vlist_local[vertex].attr, VERTEX4DTV1_ATTR_POINT);
				break;
			}
		}
	}
	//��������İ뾶
	Compute_OBJECT4DV2_Radius(obj);
	//5����ʼ��������"Face list:" ��ʾ��Ŀ�ʼ
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['Face'] ['list:']"))
		{
			break;
		}
	}
	//6������ÿ����
	int poly_surface_desc = 0;
	int poly_num_verts = 0;
	char temp_string[8];
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		//��ȡ��
		while (1)
		{
			if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				parser.Close();
				return 0;
			}

			StripChars(parser.buffer, parser.buffer, ":ABC");
			if (parser.Pattern_Match(parser.buffer, "['Face'] [i] [i] [i] [i]"))
			{
				//��������
				if (vertex_flags & VERTEX_FLAGS_INVERT_WINDING_ORDER)
				{
					poly_num_verts = 3;
					obj->plist[poly].vert[0] = parser.pints[3];
					obj->plist[poly].vert[1] = parser.pints[2];
					obj->plist[poly].vert[2] = parser.pints[1];
					
				}
				//����˳��
				else
				{
					poly_num_verts = 3;
					obj->plist[poly].vert[0] = parser.pints[1];
					obj->plist[poly].vert[1] = parser.pints[2];
					obj->plist[poly].vert[2] = parser.pints[3];
				}
				//ÿһ���������ָ�붼ָ��ǰ����Ķ�������
				obj->plist[poly].vlist = obj->vlist_local;
				//��������
				obj->plist[poly].tlist = obj->tlist;
				break;
			}
		}
		//��ȡ����
		while (1)
		{
			if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				parser.Close();
				return 0;
			}
			ReplaceChars(parser.buffer, parser.buffer, ":\"rgba", ' ');
			//M te i l 32 43 54
			//Material 32 43 54
			if (parser.Pattern_Match(parser.buffer, "[i] [i] [i]"))
			{
				r = parser.pints[0];
				g = parser.pints[1];
				b = parser.pints[2];
				obj->plist[poly].color = RGB(r * 32, g * 32, b * 32);

				int vertex_overrides = (vertex_flags & VERTEX_FLAGS_OVERRIDE_MASK);
				if (vertex_overrides)
				{
					if (vertex_overrides & VERTEX_FLAGS_OVERRIDE_PURE)
						SET_BIT(obj->plist[poly].attr, POLY4DV2_ATTR_SHADE_MODE_PURE);
					if (vertex_overrides & VERTEX_FLAGS_OVERRIDE_FLAT)								//�򵥵���ɫģʽ��ֻ��ɫ��
						SET_BIT(obj->plist[poly].attr, POLY4DV2_ATTR_SHADE_MODE_FLAT);
					if (vertex_overrides & VERTEX_FLAGS_OVERRIDE_GOURAUD)							//������ɫģʽ
					{
						//���õ�ǰ�����
						SET_BIT(obj->plist[poly].attr, POLY4DV2_ATTR_SHADE_MODE_GOURAUD);
						
						//���õ�ǰ�����Ķ���
						SET_BIT(obj->vlist_local[obj->plist[poly].vert[0]].attr, VERTEX4DTV1_ATTR_NORMAL);
						SET_BIT(obj->vlist_local[obj->plist[poly].vert[1]].attr, VERTEX4DTV1_ATTR_NORMAL);
						SET_BIT(obj->vlist_local[obj->plist[poly].vert[2]].attr, VERTEX4DTV1_ATTR_NORMAL);

					}
					//֧������
					if (vertex_overrides & VERTEX_FLAGS_OVERRIDE_TEXTURE)
					{
						SET_BIT(obj->plist[poly].attr, POLY4DV2_ATTR_SHADE_MODE_TEXTURE);
					}
				}
				else
				{
					//�û�û�����ã���Ĭ����������ɫģʽ
					 SET_BIT(obj->plist[poly].attr, POLY4DV2_ATTR_SHADE_MODE_FLAT);
					//Ĭ�����ó�GOURAUD��ɫģʽ
					//SET_BIT(obj->plist[poly].attr, POLY4DV2_ATTR_SHADE_MODE_GOURAUD);
				}
				SET_BIT(obj->plist[poly].attr, POLY4DV2_ATTR_DISABLE_MATERIAL);
				obj->plist[poly].state = POLY4DV2_STATE_ACTIVE;
				break;
			}	//end if
		}	//end while
	}	//end for(��ȡ��)

	//����ÿ����ķ���������
	Compute_OBJECT4DV2_Poly_Normals(obj);
	//����ÿ������ļ�Ȩ������
	Compute_OBJECT4DV2_Vertex_Normals(obj);
	return 1;
}


int Load_OBJECT4DV2_COB(OBJECT4DV2_PTR obj, char *filename,							//����COBģ���ļ�
	VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot, int vertex_flags
	){
	CPARSERV1 parser;
	char seps[16];				//
	char *token;
	int r = 0, g = 0, b = 0;
	VERTEX2DF texture_vertices[OBJECT4DV2_MAX_VERTICES];			//������
	int num_texture_vertices = 0;
	MATRIX4X4 mat_local, mat_world;
	MAT_IDENTITY_4X4(&mat_local);									//��ʼ��Ϊ��λ����
	MAT_IDENTITY_4X4(&mat_world);									//�㶮�ã�����������
	memset(obj, 0, sizeof(OBJECT4DV2));
	//��������״̬λ����Ϳɼ���
	obj->state = OBJECT4DV2_STATE_ACTIVE | OBJECT4DV2_STATE_VISIBLE;
	obj->num_frames = 1;
	obj->curr_frame = 0;
	obj->attr = OBJECT4DV2_ATTR_SINGLE_FRAME;						//���õ�ǰ����ֻ��һ֡
	if (pos)
	{
		obj->world_pos.x = pos->x;
		obj->world_pos.y = pos->y;
		obj->world_pos.z = pos->z;
		obj->world_pos.w = 1;
	}
	else
	{
		obj->world_pos.x = obj->world_pos.y = obj->world_pos.z = 0;
		obj->world_pos.w = 1;
	}
	//���ļ����򿪴�����ֱ���˳�
	if (!parser.Open(filename))
	{
		return 0;
	}

	//������һ������ȡ���������
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['Name'] [s>0]"))
		{
			strcpy(obj->name, parser.pstrings[1]);
			break;
		}
	}
	//�����ڶ�������ȡ����ı任����
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['center'] [f] [f] [f]"))
		{
			//0 0 0 0
			//0 0 0 0
			//0 0 0 0
			//(f0) (f1) (f2) 0
			mat_local.M[3][0] = -parser.pfloats[0];
			mat_local.M[3][1] = -parser.pfloats[1];
			mat_local.M[3][2] = -parser.pfloats[2];
			//"x axis"
			parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.Pattern_Match(parser.buffer, "['x'] ['axis'] [f] [f] [f]");
			
			mat_local.M[0][0] = parser.pfloats[0];		//rxx
			mat_local.M[1][0] = parser.pfloats[1];		//rxy
			mat_local.M[2][0] = parser.pfloats[2];		//rxz
			
			//"y axis"
			parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.Pattern_Match(parser.buffer, "['y'] ['axis']");

			mat_local.M[0][1] = parser.pfloats[0];
			mat_local.M[1][1] = parser.pfloats[1];
			mat_local.M[2][1] = parser.pfloats[2];

			//z axis
			parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.Pattern_Match(parser.buffer, "['z'] ['axis']");

			mat_local.M[0][2] = parser.pfloats[0];
			mat_local.M[1][2] = parser.pfloats[1];
			mat_local.M[2][2] = parser.pfloats[2];
			
			break;
		}
	}
	//��������������ȡ'Transform'
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['Transform']"))
		{
			//"x axis"
			parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.Pattern_Match(parser.buffer, "[f] [f] [f]");

			mat_world.M[0][0] = parser.pfloats[0];
			mat_world.M[1][0] = parser.pfloats[1];
			mat_world.M[2][0] = parser.pfloats[2];

			//"y axis"
			parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.Pattern_Match(parser.buffer, "[f] [f] [f]");
			
			mat_world.M[0][1] = parser.pfloats[0];
			mat_world.M[1][1] = parser.pfloats[1];
			mat_world.M[2][1] = parser.pfloats[2];

			//"z axis"
			parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.Pattern_Match(parser.buffer, "[f] [f] [f]");

			mat_world.M[0][2] = parser.pfloats[0];
			mat_world.M[1][2] = parser.pfloats[1];
			mat_world.M[2][2] = parser.pfloats[2];
			
			break;
		}	//end if
	}	//end while
	//�������Ĳ�����ȡ���嶥�����������
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['World'] ['Vertices'] [i]"))
		{
			obj->num_vertices = parser.pints[0];
		}
	}
	//��ʼ������
	if (!Init_OBJECT4DV2(obj, obj->num_vertices, obj->num_vertices * 3, obj->num_frames))
	{
		parser.Close();
		return 0;
	}
	//�������岽����ȡ����Ķ�������
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		while (1)
		{
			if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				parser.Close();
				return 0;
			}
			if (parser.Pattern_Match(parser.buffer, "[f] [f] [f]"))
			{
				obj->vlist_local[vertex].x = parser.pfloats[0];
				obj->vlist_local[vertex].y = parser.pfloats[1];
				obj->vlist_local[vertex].z = parser.pfloats[2];
				obj->vlist_local[vertex].w = 1;
				
				VECTOR4D temp_vector;
				if (vertex_flags & VERTEX_FLAGS_TRANSFORM_LOCAL)				//����ִ�б�������任�ı�־
				{
					Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex].v, &mat_local, &temp_vector);
					VECTOR4D_COPY(&obj->vlist_local[vertex].v, &temp_vector);
				}
				if (vertex_flags & VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD)
				{
					Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex].v, &mat_world, &temp_vector);
					VECTOR4D_COPY(&obj->vlist_local[vertex].v, &temp_vector);
				}
				float temp_f;		//swapping
				//����������
				if (vertex_flags & VERTEX_FLAGS_INVERT_X)
					obj->vlist_local[vertex].x = -obj->vlist_local[vertex].x;
				if (vertex_flags & VERTEX_FLAGS_INVERT_Y)
					obj->vlist_local[vertex].y = -obj->vlist_local[vertex].y;
				if (vertex_flags & VERTEX_FLAGS_INVERT_Z)
					obj->vlist_local[vertex].z = -obj->vlist_local[vertex].z;
				//����������
				if (vertex_flags & VERTEX_FLAGS_SWAP_YZ)
					SWAP(obj->vlist_local[vertex].y, obj->vlist_local[vertex].z, temp_f);
				if (vertex_flags & VERTEX_FLAGS_SWAP_XZ)
					SWAP(obj->vlist_local[vertex].x, obj->vlist_local[vertex].z, temp_f);
				if (vertex_flags & VERTEX_FLAGS_SWAP_XY)
					SWAP(obj->vlist_local[vertex].x, obj->vlist_local[vertex].y, temp_f);
				
				if (scale)
				{
					obj->vlist_local[vertex].x *= scale->x;
					obj->vlist_local[vertex].y *= scale->y;
					obj->vlist_local[vertex].z *= scale->z;
				}
				SET_BIT(obj->vlist_local[vertex].attr, VERTEX4DTV1_ATTR_POINT);

				break;
			}	//end if
		}	//end while
	}	//end for vertex
	//��������İ뾶
	Compute_OBJECT4DV2_Radius(obj);
	//��������������ȡ����Ķ�����������
	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['Texture'] ['Vertices'] [i]"))
		{
			num_texture_vertices = parser.pints[0];
		}
	}
	//�������߲�����ȡ�����ÿһ������
	for (int tvertex = 0; tvertex < num_texture_vertices; tvertex++)
	{
		while (1)
		{
			if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				parser.Close();
				return 0;
			}
			if (parser.Pattern_Match(parser.buffer, "[f] [f]"))
			{
				obj->tlist[tvertex].x = parser.pfloats[0];
				obj->tlist[tvertex].y = parser.pfloats[1];
				break;
			}
		}
	}
	//�����ڰ˲�����ȡ�������

	while (1)
	{
		if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			parser.Close();
			return 0;
		}
		if (parser.Pattern_Match(parser.buffer, "['Faces'] [i]"))
		{
			obj->num_polys = parser.pints[0];
			break;
		}
	}
	int poly_surface_desc = 0;
	int poly_num_verts = 0;
	int num_materials_object = 0;
	int poly_material[OBJECT4DV2_MAX_POLYS];
	int material_index_referenced[MAX_MATERIALS];
	memset(material_index_referenced, 0, sizeof(material_index_referenced));
	//�����ھŲ�����ȡ�������
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		while (1)
		{
			if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				parser.Close();
				return 0;
			}
			if (parser.Pattern_Match(parser.buffer, "['Faces'] ['verts'] [i] ['flags'] [i] ['mat'] [i]"))
			{
				poly_material[poly] = parser.pints[2];
				if (material_index_referenced[poly_material[poly]] == 0)
				{
					material_index_referenced[poly_material[poly]] = 1;
					num_materials_object++;
				}
				//��������
				if (parser.pints[0] != 3)
				{
					parser.Close();
					return 0;
				}
				if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
				{
					parser.Close();
					return 0;
				}
				ReplaceChars(parser.buffer, parser.buffer, ",<>", ' ');
				//��ȡ��Ķ�������
				parser.Pattern_Match(parser.buffer, "[i] [i] [i] [i] [i] [i]");
				//��ת����
				if (vertex_flags & VERTEX_FLAGS_INVERT_WINDING_ORDER)
				{
					poly_num_verts = 3;
					obj->plist[poly].vert[0] = parser.pints[4];	//5
					obj->plist[poly].vert[1] = parser.pints[2]; //3
					obj->plist[poly].vert[2] = parser.pints[0];

					obj->plist[poly].text[0] = parser.pints[5];	//6
					obj->plist[poly].text[1] = parser.pints[3];	//4
					obj->plist[poly].text[2] = parser.pints[1]; //2
				}
				else
				{
					poly_num_verts = 0;
					//��ȡ��������
					obj->plist[poly].vert[0] = parser.pints[0];
					obj->plist[poly].vert[1] = parser.pints[2];
					obj->plist[poly].vert[2] = parser.pints[4];
					//��ȡ��������
					obj->plist[poly].text[0] = parser.pints[1];
					obj->plist[poly].text[1] = parser.pints[3];
					obj->plist[poly].text[2] = parser.pints[5];
				}
				obj->plist[poly].vlist = obj->vlist_local;
				obj->plist[poly].tlist = obj->tlist;
				obj->plist[poly].state = POLY4DV2_STATE_ACTIVE;
				break;
			}	//end if
		} //end while
	}	//end for
	//��ʮ������ȡ��������
	for (int curr_material = 0; curr_material < num_materials_object; curr_material++)
	{
		while (1)
		{
			if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				parser.Close();
				return 0;
			}
			if (parser.Pattern_Match(parser.buffer, "['mat#'] [i]"))
			{
				int material_index = parser.pints[0];
				//��ȡcolor
				while (1)
				{
					if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						parser.Close();
						return 0;
					}
					ReplaceChars(parser.buffer, parser.buffer, ",", ' ', 1);
					//ԭʼ���ݣ�rgb i,i,i->rgb i i i
					//�����ȡ��Ľ���
					if (parser.Pattern_Match(parser.buffer, "['rgb'] [f] [f] [f]"))
					{
						//��ȡ��������
						//COB�ļ�������ɫ����Ϊ������������ת���ɵ�ǰ�������õĲ�����ɫ���ݸ�ʽ
						materials[material_index + num_materials_object].color.r = (int)(parser.pfloats[0] * 255 + 0.5);
						materials[material_index + num_materials_object].color.g = (int)(parser.pfloats[1] * 255 + 0.5);
						materials[material_index + num_materials_object].color.b = (int)(parser.pfloats[2] * 255 + 0.5);
						//
						break;
					}
				}	//end while
				//��ȡalpha
				//alpha���ʵ�Դ�ļ�����Ϊ����������Χ��0.0-1.0
				while (1)
				{
					if (!parser.GetLine(PARSER_STRIP_WS_ENDS | PARSER_STRIP_EMPTY_LINES))
					{
						parser.Close();
						return 0;
					}
					if (parser.Pattern_Match(parser.buffer, "['alpha'] [f] ['ka'] [f] ['ks'] [f] ['exp'] [f] ['ior'] [f]"))
					{
						//�����ݸ�ʽת��Ϊ��������Χ��0-255��
						materials[material_index + num_materials_object].color.a = (UCHAR)(parser.pfloats[0] * 255 + 0.5);
						materials[material_index + num_materials_object].ka = (UCHAR)(parser.pfloats[1] * 255 + 0.5);
						materials[material_index + num_materials_object].kd = 1;
						materials[material_index + num_materials_object].ks = (UCHAR)(parser.pfloats[2] * 255 + 0.5);
						materials[material_index + num_materials_object].power = parser.pfloats[3];

						for (int rgb_index = 0; rgb_index < 3; rgb_index++)
						{
							//ambient reflectivity
							//�����ⷴ��
							materials[material_index + num_materials_object].ra.rgba_M[rgb_index] = ((UCHAR)(materials[material_index + num_materials].ka * (float)materials[material_index + num_materials].color.rgba_M[rgb_index] + 0.5));
							materials[material_index + num_materials_object].rd.rgba_M[rgb_index] = ((UCHAR)(materials[material_index + num_materials].kd * (float)materials[material_index + num_materials].color.rgba_M[rgb_index] + 0.5));
							materials[material_index + num_materials_object].rs.rgba_M[rgb_index] = ((UCHAR)(materials[material_index + num_materials].ks * (float)materials[material_index + num_materials].color.rgba_M[rgb_index] + 0.5));
						}	//end for rgb_index
						break;
					}	//end if
				}	//end while

				//��ȡshader

				//Shader class color
				while (1)
				{
					if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						parser.Close();
						return 0;
					}
					if (parser.Pattern_Match(parser.buffer, "['Shader'] ['class:'] ['color']"))
					{
						break;
					}
				}
				//Shader name
				while (1)
				{
					if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						parser.Close();
						return 0;
					}
					ReplaceChars(parser.buffer, parser.buffer, "\"", ' ', 1);
					if (parser.Pattern_Match(parser.buffer, "['Shader'] ['name:'] ['plain'] ['color']"))
					{
						break;
					}
					if (parser.Pattern_Match(parser.buffer, "['Shader'] ['name:'] ['texture'] ['map']"))
					{
						SET_BIT(materials[material_index + num_materials].attr, MATV1_ATTR_SHADE_MODE_TEXTURE);
						while (1)
						{
							if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
							{
								parser.Close();
								return 0;
							}
							ReplaceChars(parser.buffer, parser.buffer, "\"", ' ');
							if (parser.Pattern_Match(parser.buffer, "['file'] ['name:'] ['string']"))
							{
								memcpy(materials[material_index + num_materials].texture_file, &parser.buffer[18], strlen(parser.buffer) - 18 + 2);
								if (!obj->texture)
								{
									obj->texture = (BITMAP_IMAGE_PTR)malloc(sizeof(BITMAP_IMAGE));
									char filename[80];
									char path_filename[80];
									char texture_path[80];
									Extract_Filename_From_path(materials[material_index + num_materials].texture_file, filename);
									//��ȡ�������������
									strcpy(path_filename, texture_path);
									strcpy(path_filename, filename);
									//��������ͼƬ
									//......ʡ��
									SET_BIT(obj->attr, OBJECT4DV2_ATTR_TEXTURES);
								}	//end if
								break;
							}
						}	//end while
						break;
					}	//end if
				}	//end while

				while (1)
				{
					if (!parser.GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						parser.Close();
						return 0;
					}
					ReplaceChars(parser.buffer, parser.buffer, "\"", ' ', 1);
					if (parser.Pattern_Match(parser.buffer, "['Shader'] ['name:'] [s>0]"))
					{
						if (strcmp(parser.pstrings[2], "constant") == 0)
						{
							SET_BIT(materials[material_index + num_materials].attr, MATV1_ATTR_SHADE_MODE_CONSTANT);
						}
						else if (strcmp(parser.pstrings[2], "matte") == 0)
						{
							SET_BIT(materials[material_index + num_materials].attr, MATV1_ATTR_SHADE_MODE_FLAT);
						}
						else if (strcmp(parser.pstrings[2], "plastic") == 0)
						{
							SET_BIT(materials[curr_material + num_materials].attr, MATV1_ATTR_SHADE_MODE_GOURAUD);
						}
						else if (strcmp(parser.pstrings[2], "phong") == 0)
						{
							SET_BIT(materials[material_index + num_materials].attr, MATV1_ATTR_SHADE_MODE_FASTPHONG);
						}
						else
						{
							SET_BIT(materials[material_index + num_materials].attr, MATV1_ATTR_SHADE_MODE_FLAT);
						}
						break;
					}
				}	//end while

				for (int curr_poly = 0; curr_poly < obj->num_polys; curr_poly++)
				{
					poly_material[curr_poly] = poly_material[curr_poly] + num_materials;
					SET_BIT(obj->plist[curr_poly].attr, POLY4DV2_ATTR_RGB16);
					//�ж��Ƿ�ʹ��������ɫ
					if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_TEXTURE)
						obj->plist[curr_poly].color = RGB(255, 2555, 255);
					else
						obj->plist[curr_poly].color = RGB(materials[poly_material[curr_poly]].color.r, materials[poly_material[curr_poly]].color.g, materials[poly_material[curr_poly]].color.b);
					//

					if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_CONSTANT)
					{
						SET_BIT(obj->plist[curr_poly].attr, MATV1_ATTR_SHADE_MODE_CONSTANT);
					}
					else if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_FLAT)			//Ҳ��Ĭ����Ⱦģʽ
					{
						SET_BIT(obj->plist[curr_poly].attr, MATV1_ATTR_SHADE_MODE_FLAT);
					}
					else if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_GOURAUD)
					{
						//���õ�ǰ���������
						SET_BIT(obj->plist[curr_poly].attr, MATV1_ATTR_SHADE_MODE_GOURAUD);
						//����ÿһ���������
						SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[0]].attr, VERTEX4DTV1_ATTR_NORMAL);
						SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[1]].attr, VERTEX4DTV1_ATTR_NORMAL);
						SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[2]].attr, VERTEX4DTV1_ATTR_NORMAL);

					}
					else if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_FASTPHONG)
					{
						SET_BIT(obj->plist[curr_poly].attr, MATV1_ATTR_SHADE_MODE_FASTPHONG);

						SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[0]].attr, VERTEX4DTV1_ATTR_NORMAL);
						SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[1]].attr, VERTEX4DTV1_ATTR_NORMAL);
						SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[2]].attr, VERTEX4DTV1_ATTR_NORMAL);
					}
					else
					{
						//û�в���,���ñߵ���ȾģʽΪFLATģʽ
						SET_BIT(obj->plist[curr_poly].attr, POLY4DV2_ATTR_SHADE_MODE_FLAT);
					}

					if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_TEXTURE)
					{
						SET_BIT(obj->plist[curr_poly].attr, POLY4DV2_ATTR_SHADE_MODE_TEXTURE);
						obj->plist[curr_poly].texture = obj->texture;
						SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[0]].attr, VERTEX4DTV1_ATTR_TEXTURE);
						SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[1]].attr, VERTEX4DTV1_ATTR_TEXTURE);
						SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[2]].attr, VERTEX4DTV1_ATTR_TEXTURE);
					}
					SET_BIT(obj->plist[curr_poly].attr, POLY4DV2_ATTR_DISABLE_MATERIAL);

				}	//end for

				num_materials += num_materials_object;
				if (obj->texture)
				{
					for (int tvertex = 0; tvertex < num_texture_vertices; tvertex++)
					{
						int texture_size = obj->texture->width;
						obj->tlist[tvertex].x *= (texture_size - 1);
						obj->tlist[tvertex].y *= (texture_size - 1);
						if (vertex_flags & VERTEX_FLAGS_INVERT_TEXTURE_U)
						{
							obj->tlist[tvertex].x = (texture_size - 1) - obj->tlist[tvertex].x;
						}
						if(vertex_flags & VERTEX_FLAGS_INVERT_TEXTURE_V)
						{
							obj->tlist[tvertex].y = (texture_size - 1) - obj->tlist[tvertex].y;
						}
						if (vertex_flags & VERTEX_FLAGS_INVERT_SWAP_UV)
						{
							float temp;
							SWAP(obj->tlist[tvertex].x, obj->tlist[tvertex].y, temp);
						}
					}	//end for
				}	//end if

			}	//end if
		}

	}//end for
	Compute_OBJECT4DV2_Poly_Normals(obj);
	Compute_OBJECT4DV2_Vertex_Normals(obj);
	return 1;
}

