#ifndef _F_MESSAGE_H_
#define _F_MESSAGE_H_
/*
	��Ϣ����
	FMessageHeader		��Ϣͷ��
	FMessageBody		��Ϣ��
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
	int type;		//��Ϣ��id
	int length;		//������Ϣ�ĳ���
}FMessageHeader;

typedef struct tagFMessageBody{

}FMessageBody;

class FUser{
public:
	int user_id;
	string user_name;
	string user_password;
	bool is_login;
	//�����û���¼״̬Ϊδ��¼
	FUser(){
		is_login = false;
	}
};

class FMessage{
public:
	int user_id;		//�û�id
	int group_id;		//����id
	string message;		//�û����͵���Ϣ
}; 

//�û��Զ��庯��
void fstrcpy(void*des, void*res, int len){
	unsigned char *d = (unsigned char *)des;
	unsigned char *r = (unsigned char *)res;
	for (int i = 0; i < len; i++)
	{
		*(d + i) = *(r + i);
	}
}
#endif