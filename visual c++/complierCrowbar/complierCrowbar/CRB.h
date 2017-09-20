#ifndef _CRB_H_
#define _CRB_H_
struct CRB_Interpreter_tag{
	MEM_Storage interpreter_storage;
	MEM_Storage execute_storage;
	Variable *variable;
	FunctionDefinition *function_list;
	StatementList *statment_list;
	int current_line_number;
};
	
#endif