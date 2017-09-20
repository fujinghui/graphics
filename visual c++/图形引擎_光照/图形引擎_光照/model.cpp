#define _CRT_SECURE_NO_DEPRECATE
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "graph_engine.h"
#include "model.h"
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
	int r, g, b;
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
			ReplaceChars(parser.buffer, parser.buffer, ":\rgba", ' ');
			if (parser.Pattern_Match(parser.buffer, "[i] [i] [i]"))
			{
				r = parser.pints[0];
				g = parser.pints[1];
				b = parser.pints[2];
				SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_RGB16);		//������ɫλ��״̬
				obj->plist[poly].color = RGB(r, g, b);
			}
			else
			{
				SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_RGB16);
				obj->plist[poly].color = RGB(0, 0, 0);
			}
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


