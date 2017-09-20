#ifndef _ASSIST_H_
#define _ASSIST_H_
float IsFloat(char *fstring);
int IsInt(char *istring);
int StripChars(char *string_in, char *string_out, char *strip_chars, int case_on = 1);						//从string_in删除指定的strip_chars字符串
int ReplaceChars(char *string_in, char *string_out, char *replace_chars, char rep_char, int case_on = 1);	//替换string_in里的replace_chars(字符串)为rep_char(字符)

#endif