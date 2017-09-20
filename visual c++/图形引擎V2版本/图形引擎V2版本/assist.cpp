#include "assist.h"
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>

//kmp算法辅助函数（求解next数组)
void BuildNext(const char *pattern, int *next){
	int length = strlen(pattern);
	next[0] = 0;
	int q, k;
	for (q = 1, k = 0; q < length; q++)
	{
		while (k > 0 && pattern[q] != pattern[k])
			k = next[k - 1];
		if (pattern[q] == pattern[k])
			k++;
		next[q] = k;
	}
}

float IsFloat(char *fstring){
	char *string = fstring;
	while (isspace(*string)) string++;
	//[sign]
	if (*string == '+' || *string == '-') string++;
	while (isdigit(*string)) string++;
	if (*string == '.')
	{
		string++;
		while (isdigit(*string)) string++;
	}
	//记过上述匹配后到达文件末尾，则表明正确
	if (strlen(fstring) == (int)(string - fstring))
		return (atof(fstring));
	return(FLT_MIN);
}
int IsInt(char *istring){
	char *string = istring;
	while (isspace(*string)) string++;
	if (*string == '+' || *string == '-') string++;
	while (isdigit(*string)) string++;
	if (strlen(istring) == (int)(string - istring))
		return atoi(istring);
	return INT_MIN;				//用整数表示的最小的数
}

int StripChars(char *string_in, char *string_out, char *strip_chars, int case_on){
	//从string_in删除指定的strip_chars字符串
	int i, j;
	int next[256];
	int len1 = strlen(string_in);
	int len2 = strlen(strip_chars);
	int ti;
	ti = 0;
	if (case_on)											//区别大小写,不区别大小写
	{
		for (i = 0; i < len1; i++)
		{
			for (j = 0; j < len2; j++)
			{
				if (string_in[i] == strip_chars[j])
				{
					break;
				}
			}
			if (j == len2)
			{
				string_out[ti] = string_in[i];
				ti++;
			}
		}
	}
	else
	{
		for (i = 0; i < len1; i++)
		{
			for (j = 0; j < len2; j++)
			{
				if (toupper(string_in[i]) == toupper(strip_chars[j]))
				{
					break;
				}
			}
			if (j == len2)
			{
				string_out[ti] = string_in[i];
				ti++;
			}
		}
	}
	string_out[ti] = '\0';
	/*
	BuildNext(strip_chars, next);
	for (i = 0, j = 0; i < len1; i++)
	{
		while (j > 0 && string_in[i] != strip_chars[j])
			j = next[j - 1];
		if (string_in[i] == strip_chars[j])
			j++;
		if (j == len2)				//匹配到字符，删除它
		{
			ti = ti - len2 + 1;		//删除之前添加进来的字符(往回移动指针即可)

			j = 0;					//重新再进行一次匹配
		}
		else
		{
			string_out[ti] = string_in[i];
			ti++;
		}
	}
	string_out[ti] = '\0';
	*/
	return 0;
}
int ReplaceChars(char *string_in, char *string_out, char *replace_chars, char rep_char, int case_on){
	//替换string_in里的replace_chars(字符串)为rep_char(字符)
	int i, j;
	int len1 = strlen(string_in);
	int len2 = strlen(replace_chars);
	int next[256];
	//BuildNext(replace_chars, next);
	int ti = 0;
	if (case_on)
	{
		for (i = 0, j = 0; i < len1; i++)
		{
			for (j = 0; j < len2; j++)
			{
				if (string_in[i] == replace_chars[j])
					break;
			}
			if (j == len2)			//不替换
			{
				string_out[ti] = string_in[i];
			}
			else					//替换
			{
				string_out[ti] = rep_char;
			}
			ti++;
		}
	}
	else
	{
		for (i = 0, j = 0; i < len1; i++)
		{
			for (j = 0; j < len2; j++)
			{
				if (toupper(string_in[i]) == toupper(replace_chars[j]))
					break;
			}
			if (j == len2)			//不替换
			{
				string_out[ti] = string_in[i];
			}
			else					//替换
			{
				string_out[ti] = rep_char;
			}
			ti++;
		}
	}
	string_out[ti] = '\0';
	return 0;
}
char *Extract_Filename_From_path(char *filepath, char *filename){
	if (filepath == NULL || strlen(filepath) == 0)
		return NULL;
	int length = strlen(filepath);
	int end_index = length - 1;
	while (end_index > 0 && filepath[end_index] != '/' && filepath[end_index] != '\\')
		end_index--;
	memcpy(filename, filepath + end_index + 1, length - end_index);		//多复制一个'\0'
	return filename;
}

