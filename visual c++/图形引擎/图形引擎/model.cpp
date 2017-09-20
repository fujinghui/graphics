#define _CRT_SECURE_NO_DEPRECATE
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "graph_engine.h"
#include "model.h"
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
	//第一行，读取到物体的的顶点数量和边的数量
	sscanf(token_string, "%s %d %d", obj->name, &obj->num_vertices, &obj->num_polys);
	//加载顶点列表
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
	//加载面列表
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
		//将该顶点数组指向之前获取的顶点数组
		obj->plist[poly].state = POLY4DV1_STATE_ACTIVE;
		obj->plist[poly].attr = 0;
		obj->plist[poly].vlist = obj->vlist_local;
		
		if ((poly_surface_desc & PLX_2SIDED_FLAG))
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_2SIDED);
		}
		else
		{
			//错误
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
	}
	fclose(fp);
	return 0;
}


