/*
	�﷨������
	��д�ߣ�femy

*/
#ifndef _PARSE_H_
#define _PARSE_H_

#include "data_structure.h"
#include <malloc.h>

AST* Parse(Token *token);		//��Token�����з��������ɳ����﷨��


AST* stmt(Token *token);

AST* Parse(Token *token){
	AST *ast_head;
	AST *ast_current;

	ast_head = (AST*)malloc(sizeof(AST));
	if (ast_head == NULL)
		return NULL;
	ast_head->kind = AST_STMT;			//�﷨���������ײ�Ϊһ�����
	ast_head->brother_or_sister = NULL;
	ast_head->child = stmt(token);
}

AST* stmt(Token *token){
	AST *ast_stmt = NULL;
	while (token != NULL)
	{
		switch (token->flag)
		{
		case EMPTY:
			break;
		case MAIN:
			
		}
		token = token->next;
	}
	return ast_stmt;
}
#endif