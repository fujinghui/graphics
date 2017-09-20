
/*
词法分析器模块，主要由于词法的分析
编写人：femy
编写时间：2016.7.14
*/
#ifndef _LEX_H_
#define _LEX_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "data_structure.h"
#define TEMP_STRING_LEN 100
#define TEMP_BUFFER_LEN 1024		//定义临时缓冲区大小

char *keyword[32] = {

	"main", "if", "then", "while", "do", "static", "default", "enum",
	"struct", "break", "else", "long", "switch", "case", "typedef",
	"char", "return", "const", "float", "short", "continue", "for",
	"int", "void", "sizeof", "auto", "double", "extern", "goto", 
	"signed", "unsigned", "volatile"
};
int keyword_id[32] = {
	MAIN, IF, THEN, WHILE, DO, STATIC, DEFAULT, ENUM,
	STRUCT, BREAK, ELSE, LONG, SWITCH, CASE, TYPEDEF,
	CHAR, RETURN, CONST, FLOAT, SHORT, CONTINUE, FOR,
	INT, VOID, SIZEOF, AUTO, DOUBLE, EXTERN, GOTO, 
	SIGNED, UNSIGNED, VOLATILE
};
char symbol[17] = {
	'+', '-', '*', '/',
	'=', ';', '(', ')',
	'<', '>', '{', '}',
	'[', ']', '&', '|',
	'#'
};
int symbol_id[17] = {
	ADD, SUB, MUL, DIV,
	ASSIGN, SEMI, LPAREN, RPAREN,
	LT, GT, LBRACE, RBRACE, 
	LBRACK, RBRACK, BIT_AND, BIT_OR,
	JING
};

int IsKeyword();
int IsSymbol();
void ReadNextChar();			//读入下一个字符串
void Concat();					//将当前读入的字符与临时字符串连接在一起
void ResetInputBuffer();
int IsLetter();
int IsDigit();
int IsBlank();
int IsSemicolon();
int IsUnderline();
void FlushInputBuffer();				//完成一个缓存区，将读入的标识符串加入表中
void DestroyToken();
void ShowToken();
void InitLexer();
Token* Lexer(char path[]);

//Token块链表
Token *token = NULL;
Token *token_current = NULL;
//由于保存临时的一个语句块
char temp_input_string[TEMP_STRING_LEN];
int temp_input_index = 0;
//读取的字符
int read_char_index = 0;
char read_char = '\0';
char pre_char = '\0';
//临时从文件读入的缓冲区
char temp_buffer[TEMP_BUFFER_LEN];
int temp_buffer_len = 0;
int temp_buffer_index = 0;
int is_end = 0;
FILE *file = NULL;
Flag flag_current = ID;
Flag kind_current = EMPTY;
int line = 1;

int IsKeyword(){
	int i;
	int index = 0;
	for (index = 0; index < 32; index++)
	{
		for (i = 0; i < temp_input_index && keyword[index][i] != '\0'; i++)
		{
			if (temp_input_string[i] != keyword[index][i])
			{
				break;
			}
		}
		if (i == temp_input_index)
			break;
	}

	if (index == 32)
		return -1;
	return keyword_id[index];
}
int IsSymbol(){
	int index;
	if (temp_input_index > 1)
		return EMPTY;
	for (index = 0; index < 17; index++)
	{
		if (temp_input_string[0] == symbol[index])
		{
			return symbol_id[index];
		}
	}
	return EMPTY;
}
void InitLexer(){
	token = NULL;
	token_current = NULL;
	temp_input_index = 0;
	read_char_index = 0;
	read_char = '\0';
	pre_char = '\0';
	temp_buffer_len = 0;
	temp_buffer_index = 0;
	is_end = 0;
	file = NULL;
	line = 1;
}
void ShowToken(){
	Token *t = token;
	while (t != NULL)
	{
		if (t->flag != EMPTY)
		{
			printf("%s", t->value);
		//	printf("%d", t->flag);
			printf("\n");
		}
		t = t->next;
	}
}
void DestroyToken(){
	Token *t = token;
	Token *t_delete = NULL;
	while (t->next != NULL)
	{
		t_delete = t;
		t = t->next;
		free(t_delete);
	}
	token = NULL;
	token_current = NULL;
	free(t);
}

void FlushInputBuffer(){
	token_current->next = (Token*)malloc(sizeof(Token));
	if (token_current->next == NULL)
	{
		DestroyToken();
		is_end = 1;
		return;
	}
	token_current = token_current->next;
	token_current->length = temp_input_index;			//设置当前token的value的长度
	token_current->kind = kind_current;					//设置token的标记
	token_current->flag = flag_current;					//标记是哪类符号（关键字， 运算符， 边界符）
	token_current->line = line;
	strcpy(token_current->value, temp_input_string);
	token_current->value[token_current->length] = '\0';
	token_current->next = NULL;
	token_current->flag = flag_current;

	ResetInputBuffer();
}

void ReadNextChar(){
	if (is_end == 1)
		return;
	if (temp_buffer_index < temp_buffer_len)
	{
		pre_char = read_char;
		read_char = temp_buffer[temp_buffer_index++];
		//printf("%c", read_char);
		if (read_char == '\n')
			line++;
	}
	else
	{
		temp_buffer_len = fread(temp_buffer, 1, TEMP_BUFFER_LEN, file);
		if (temp_buffer_len > 0)			//判断是否读取到文件末尾或者读取文件错误,如果没有,则接着执行程序
		{
			temp_buffer_index = 0;
			pre_char = read_char;
			read_char = temp_buffer[temp_buffer_index++];
			if (read_char == '\n')
				line++;
		}
		else								//读取到文件末尾,结束解析
		{
			is_end = 1;
		}
	}
}
void Concat(){
	temp_input_string[temp_input_index++] = read_char;
	if (temp_input_index >= TEMP_STRING_LEN)
	{
		temp_input_index = 0;
	}
}
void ResetInputBuffer(){
	temp_input_index = 0;
}
int IsLetter(){
	if (read_char >= 'a' && read_char <= 'z')
		return 1;
	if (read_char >= 'A' && read_char <= 'Z')
		return 1;
	return 0;
}
int IsDigit(){
	if (read_char >= '0' && read_char <= '9')
		return 1;
	return 0;
}
int IsBlank(){
	if (read_char == ' ')
		return 1;
	return 0;
}
int IsSemicolon(){
	return read_char == ';';
}
int IsUnderline(){
	return read_char == '_';
}
Token* Lexer(char path[]){
	int f_hide = 1;
	InitLexer();
	file = fopen(path, "rb");
	if (file == NULL)
		is_end = 1;			//停止解析
	else
		is_end = 0;			//允许解析
	temp_buffer_len = fread(temp_buffer, 1, TEMP_BUFFER_LEN, file);
	if (temp_buffer_len <= 0)
		is_end = 1;
	token = (Token*)malloc(sizeof(Token));
	if (token == NULL)
	{
		fclose(file);
		return NULL;
	}
	token->flag = EMPTY;
	token->next = NULL;
	token_current = token;
	ReadNextChar();
	while (!is_end)
	{
		kind_current = EMPTY;
		if (IsLetter())
		{
			while (IsLetter() || IsDigit() || IsUnderline())
			{
				Concat();
				ReadNextChar();
			}
			flag_current = (Flag)IsKeyword();
			if (flag_current == -1)
				flag_current = ID;
			else
				kind_current = KEYWORD;
			FlushInputBuffer();
		}
		else if (IsDigit())
		{
			while (IsDigit())
			{
				Concat();
				ReadNextChar();
			}
			flag_current = NUMBER;
			FlushInputBuffer();
		}
		else if (IsBlank())		// 忽略空格
		{
			ReadNextChar();
		}
		else if (read_char == '>' || read_char == '<' || read_char == '=')
		{
			Concat();
			ReadNextChar();
			flag_current = (Flag)IsSymbol();		//获取符号标记
			kind_current = BORDER_SYMBOL;			//标记为边界符
			if (read_char == '=')
			{
				Concat();
				ReadNextChar();
			}
			if (temp_input_index == 2)
			{
				if (read_char == '=')
				{
					if (flag_current == ASSIGN)
					{
						flag_current = EQ;
					}
					else if (flag_current == LT)
					{
						flag_current = LTE;
					}
					else if (flag_current == GT)
					{
						flag_current = GTE;
					}
				}
			}
			FlushInputBuffer();
		}
		else if (read_char == '\"')
		{
			flag_current = STRING;
			ReadNextChar();
			while (read_char != '\"')
			{
				Concat();
				ReadNextChar();
			}
			ReadNextChar();
			FlushInputBuffer();
		}
		else if (read_char == '/')					//消去注释
		{	
			Concat();
			ReadNextChar();
			if (read_char == '/')
			{
				while (read_char != '\n')			//忽略当前这一行
				{
					ReadNextChar();
				}
				ResetInputBuffer();
			}
			else if (read_char == '*')
			{
				ReadNextChar();
				//消去多行注释直到读取到 */ 时才退出此处循环
				do{
					do{
						ReadNextChar();
					} while (read_char != '/' && is_end == 0);
				} while (pre_char != '*' && is_end == 0);
				ReadNextChar();
				//while ((pre_char != '*') && is_end == 0)
				//{
				//	while (read_char != '/' && is_end == 0)
				//	{
				//		ReadNextChar();
				//	}
				//}
			}
			else
			{
				flag_current = (Flag)IsSymbol();
				FlushInputBuffer();
			}
		}
		else if (read_char == '.')
		{
			flag_current = POINT;
			Concat();
			FlushInputBuffer();
			ReadNextChar();
		}
		else if(read_char == '#')
		{
			while (read_char != '\n')
			{
				ReadNextChar();
			}
		}
		else if (read_char == '&')
		{
			flag_current = BIT_AND;
			kind_current = OPERATOR_SYMBOL;
			Concat();
			ReadNextChar();
			if (read_char == '&')
			{
				flag_current = AND;
				Concat();
				ReadNextChar();
			}
			FlushInputBuffer();
		}
		else if (read_char == '|')
		{
			flag_current = BIT_OR;
			Concat();
			ReadNextChar();
			if (read_char == '|')
			{
				flag_current = OR;
				Concat();
				ReadNextChar();
			}
			FlushInputBuffer();
		}
		else{
			Concat();
			flag_current = (Flag)IsSymbol();
			if (flag_current != EMPTY)
			{
				if (flag_current == ADD || flag_current == SUB || flag_current == MUL || flag_current == DIV)
					kind_current = OPERATOR_SYMBOL;								//标记为运算符
				else if (flag_current == LBRACE || flag_current == RBRACE || flag_current == LBRACK || flag_current == RBRACK || flag_current == LPAREN || flag_current == RPAREN)
					kind_current = BORDER_SYMBOL;
				FlushInputBuffer();
			}
			else
			{
				ResetInputBuffer();
			}
			ReadNextChar();
		}
		/*
		else if (IsSemicolon())
		{
			Concat();
			flag_current = SEMI;
			FlushInputBuffer();
			ReadNextChar();
		}
		else if (read_char == '#')
		{
			Concat();
			FlushInputBuffer();
			ReadNextChar();
		}
		else if (read_char == '.')
		{
			Concat();
			FlushInputBuffer();
			ReadNextChar();
		}
		else if (read_char == '(')
		{
			Concat();
			FlushInputBuffer();
			ReadNextChar();
		}
		else if (read_char == ')')
		{
			Concat();
			FlushInputBuffer();
			ReadNextChar();
		}
		else if (read_char == ',')
		{
			Concat();
			FlushInputBuffer();
			ReadNextChar();
			
		}
		else if (read_char == '[')
		{
			Concat();
			FlushInputBuffer();
			ReadNextChar();
		}
		else if (read_char == ']')
		{
			Concat();
			FlushInputBuffer();
			ReadNextChar();
		}
		else if (read_char == '{')
		{
			Concat();
			FlushInputBuffer();
			ReadNextChar();
		}
		else if (read_char == '}')
		{
			Concat();
			FlushInputBuffer();
			ReadNextChar();
		}
		else
			ReadNextChar();
			*/
	}
	fclose(file);
	return token;
}

#endif