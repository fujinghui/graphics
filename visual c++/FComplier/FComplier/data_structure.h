#ifndef _DATA_STRUCTURE_H_
#define _DATA_STRUCTURE_H_
	//��ǰ��Ŀ���е����ݵĽṹ�������ڴ� �ṹ�� ö�� ������
typedef int INT32;
typedef unsigned int UINT32;
typedef short INT16;
typedef unsigned short UINT16;
typedef char INT8;
typedef unsigned UINT8;
typedef float DECIMALS32;
typedef double DECIMALS64;

enum Flag{
	EMPTY, ID, NUMBER, JING,											//��ʾ
	STRING, POINT,
	MAIN, IF, THEN, WHILE, DO, STATIC, DEFAULT, ENUM,					//�ؼ���
	STRUCT, BREAK, ELSE, LONG, SWITCH, CASE, TYPEDEF,
	CHAR, RETURN, CONST, FLOAT, SHORT, CONTINUE, FOR,
	INT, VOID, SIZEOF, AUTO, DOUBLE, EXTERN, GOTO,
	SIGNED, UNSIGNED, VOLATILE,
	ADD, SUB, MUL, DIV,												//�Ӽ��˳���������
	EQ, LT, GT, LPAREN, RPAREN, SEMI, ASSIGN, LBRACE, RBRACE, LBRACK, RBRACK,
	LTE, GTE, BIT_AND, BIT_OR, AND, OR, //���ڵ��ڣ�С�ڵ���
	//������һЩ��ʾ���͹ؼ��ֵĶ��壬������һЩ���
	KEYWORD, BORDER_SYMBOL, OPERATOR_SYMBOL
};

typedef struct tagToken{
#define TOKEN_LEN 1024
	Flag flag;				//���
	Flag kind;				//��ǵ�ǰtoken���������ͣ�Ŀǰ���������ͣ��ؼ��֣��߽�����������������ΪEMPYT)
	char value[TOKEN_LEN];
	int length;				//�ַ�������
	int line;				//��ǰ���token�ڶ�����
	struct tagToken *next;	//ֻ������
}Token;


enum AST_Flag{
	AST_STMT = 1,
	AST_WHILE, AST_DO, AST_IF, AST_SWITCH, AST_FOR,		//����
	AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_ASSIGNMENT, //�����
	AST_GT, AST_ST, AST_EQUAL 	

};
enum VALUE_Kind{
	VALUE_KIND_INT8,
	VALUE_KIND_UINT8,
	VALUE_KIND_INT16,
	VALUE_KIND_UINT16,
	VALUE_KIND_INT32,
	VALUE_KIND_UINT32,
	VALUE_KIND_FLOAT,
	VALUE_KIND_DOUBLE
};
//�����﷨������
typedef struct tagFValue{
	VALUE_Kind kind;
	union{
		INT8 int8;
		UINT8 uint8;
	}value;
}FValue;
typedef struct tagAST{
	AST_Flag kind;								//��ǰ����ڵ���ִ�еĲ�������
	FValue value;
	int child_node_count = 0;					//��ǰ�˽ڵ��ж��ٸ�ִ���ӽڵ�
	struct tagAST *brother_or_sister;			//�ֵܽڵ㣨���ýڵ㣿)
	struct tagAST *child;						//�ӽڵ�
}AST;

#endif