#ifndef _ASSIST_H_
#define _ASSIST_H_
float IsFloat(char *fstring);
int IsInt(char *istring);
int StripChars(char *string_in, char *string_out, char *strip_chars, int case_on = 1);						//��string_inɾ��ָ����strip_chars�ַ���
int ReplaceChars(char *string_in, char *string_out, char *replace_chars, char rep_char, int case_on = 1);	//�滻string_in���replace_chars(�ַ���)Ϊrep_char(�ַ�)

#endif