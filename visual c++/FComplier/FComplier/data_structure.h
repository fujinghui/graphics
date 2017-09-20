#ifndef _DATA_STRUCTURE_H_
#define _DATA_STRUCTURE_H_
	//当前项目所有的数据的结构都定义在此 结构体 枚举 联合体
typedef int INT32;
typedef unsigned int UINT32;
typedef short INT16;
typedef unsigned short UINT16;
typedef char INT8;
typedef unsigned UINT8;
typedef float DECIMALS32;
typedef double DECIMALS64;

enum Flag{
	EMPTY, ID, NUMBER, JING,											//标示
	STRING, POINT,
	MAIN, IF, THEN, WHILE, DO, STATIC, DEFAULT, ENUM,					//关键字
	STRUCT, BREAK, ELSE, LONG, SWITCH, CASE, TYPEDEF,
	CHAR, RETURN, CONST, FLOAT, SHORT, CONTINUE, FOR,
	INT, VOID, SIZEOF, AUTO, DOUBLE, EXTERN, GOTO,
	SIGNED, UNSIGNED, VOLATILE,
	ADD, SUB, MUL, DIV,												//加减乘除基本运算
	EQ, LT, GT, LPAREN, RPAREN, SEMI, ASSIGN, LBRACE, RBRACE, LBRACK, RBRACK,
	LTE, GTE, BIT_AND, BIT_OR, AND, OR, //大于等于，小于等于
	//以上是一些标示符和关键字的定义，以下是一些标记
	KEYWORD, BORDER_SYMBOL, OPERATOR_SYMBOL
};

typedef struct tagToken{
#define TOKEN_LEN 1024
	Flag flag;				//标记
	Flag kind;				//标记当前token是那种类型（目前有三种类型：关键字，边界符，运算符，其他的为EMPYT)
	char value[TOKEN_LEN];
	int length;				//字符串长度
	int line;				//当前这个token在多少行
	struct tagToken *next;	//只用链表
}Token;


enum AST_Flag{
	AST_STMT = 1,
	AST_WHILE, AST_DO, AST_IF, AST_SWITCH, AST_FOR,		//语句符
	AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_ASSIGNMENT, //运算符
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
//抽象语法分析树
typedef struct tagFValue{
	VALUE_Kind kind;
	union{
		INT8 int8;
		UINT8 uint8;
	}value;
}FValue;
typedef struct tagAST{
	AST_Flag kind;								//当前这个节点所执行的操作类型
	FValue value;
	int child_node_count = 0;					//当前此节点有多少个执行子节点
	struct tagAST *brother_or_sister;			//兄弟节点（姐妹节点？)
	struct tagAST *child;						//子节点
}AST;

#endif