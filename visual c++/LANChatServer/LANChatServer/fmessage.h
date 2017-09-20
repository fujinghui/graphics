#ifndef _F_MESSAGE_H_
#define _F_MESSAGE_H_
/*
	消息规则：
	FMessageHeader		消息头部
	FMessageBody		消息体
*/
#include <string>
using std::string;
#define MESSAGE_TYPE_LOGIN 0
#define MESSAGE_TYPE_REGISTER 1
#define MESSAGE_TYPE_LOGOUT 2
#define MESSAGE_TYPE_NORMAL 3
#define MESSAGE_TYPE_ADD_HOUSE 4
#define MESSAGE_TYPE_EXIT_HOUSE 5
#define MESSAGE_TYPE_CREATE_HOUSE 6
#define MESSAGE_TYPE_GET_HOUSE 7

typedef struct tagFMessageHeader{
	int type;		//消息的id
	int length;		//整个消息的长度
}FMessageHeader;

typedef struct tagFMessageBody{

}FMessageBody;

class FUser{
public:
	int user_id;
	string user_name;
	string user_password;
	bool is_login;
	//设置用户登录状态为未登录
	FUser(){
		is_login = false;
	}
};

class FMessage{
public:
	int user_id;		//用户id
	int group_id;		//房间id
	string message;		//用户发送的消息
}; 

//用户自定义函数
void fstrcpy(void*des, void*res, int len){
	unsigned char *d = (unsigned char *)des;
	unsigned char *r = (unsigned char *)res;
	for (int i = 0; i < len; i++)
	{
		*(d + i) = *(r + i);
	}
}
#endif