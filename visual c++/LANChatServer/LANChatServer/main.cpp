#include <stdio.h>
#include <WinSock2.h>
#include "fmessage.h"
#include "fsocket.h"
#include "login.h"
#include "house.h"
#include <list>
#include <numeric>
#include <algorithm>

using namespace std;
#pragma comment(lib, "ws2_32.lib")
list<FHouse> house_list_table;
int house_index = 1;
class FSocketSelectListener :public SocketSelectListener{
private:
	FMessageHeader message_header;
	FMessageBody message_body;
	FUserInfo login_info;
	FUserInfo register_info;
	FUserInfo logout_info;
	FUserOP user_operator;
	list<FUserInfo> user_login_table;
public:
	//@Override
	virtual void NewClient(FSocketServer *fsocket, SOCKET socket){
		printf("new client!\n");
	}
	virtual void NewMessage(FSocketServer *fserver, SOCKET socket, const char *buffer, int len){
		if (len >= sizeof(message_header))
		{
			//��ȡ��Ϣͷ����Ϣ
			fstrcpy((char*)&message_header, (void*)buffer, sizeof(message_header));

			if (message_header.type == MESSAGE_TYPE_LOGIN)			//�û�ִ�е�¼����
			{
				fstrcpy(((char*)&login_info) + 4, (void*)(buffer + sizeof(FMessageHeader)), sizeof(FUserInfo));
				printf("user name:%s\n", login_info.user_name);
				printf("user password:%s\n", login_info.user_password);
				//�û���¼�ɹ���
				if (user_operator.login(&login_info) == 1)
				{
					char buffer[5];
					buffer[0] = 1;
					fstrcpy(buffer + 1, &login_info.user_id, 4);
					send(socket, buffer, 5, 0);
					//�û���¼�ɹ�������ǰ�û����뵽�ѵ�¼����Ϣ����
					FUserInfo *user_info = new FUserInfo;						//Ϊ�µ��û���Ϣ����һ���ڴ�
					fstrcpy(user_info, &login_info, sizeof(FUserInfo));			//���û���Ϣ������user_info��
					user_info->house_id = -1;
					user_info->socket = socket;	
					login_info.house_id = -1;
					login_info.socket = socket;
					//����ͻ��˵�socket
					user_login_table.push_front(login_info);						//���ղŵ�½���û���Ϣ��ӵ��û���½����
					printf("�û���¼�ɹ����û�id:%d\n", login_info.user_id);
				}
				//�û���¼ʧ�ܣ�
				else
				{
					char buffer[5];
					buffer[1] = 0;
					send(socket, buffer, 5, 0);
				}
			}
			else if (message_header.type == MESSAGE_TYPE_REGISTER)		//�û�ִ��ע�����
			{
				fstrcpy(((char*)&register_info) + 4, (void*)(buffer + sizeof(FMessageHeader)), sizeof(FUserInfo));
			//	printf("user name:%s\n", register_info.user_name);
			//	printf("user password:%s\n", register_info.user_password);
				int state = user_operator.save(&register_info);							//ע��һ����Ϣ
				//�����ص���Ϣ��ӽ�ȥ
				char buffer[5];
				buffer[0] = state;
				send(socket, buffer, 5, 0);							//����һ����Ϣ
			}
			else if (message_header.type == MESSAGE_TYPE_LOGOUT)		//�û�ִ��ע������
			{
				//���û���¼����ɾ����ǰ�û���Ϣ
				fstrcpy(((char*)&logout_info), (void*)(buffer + sizeof(FMessageHeader)), sizeof(FUserInfo));
				list<FUserInfo>::iterator info = user_login_table.begin();
				while (info != user_login_table.end())
				{
					//id��ͬ��ɾ������Ϣ
					if (info->user_id == logout_info.user_id)
					{
						printf("ɾ����һ���û�!�û�id:%d �û�����%s ���룺%s\n", info->user_id, info->user_name, info->user_password);
						user_login_table.remove(logout_info);
						break;
					}
					info ++;
				}
			}
			else if (message_header.type == MESSAGE_TYPE_GET_HOUSE)
			{
				int data_len = 5 + sizeof(FHouse)*house_list_table.size();
				char *buffer = new char[5 + sizeof(FHouse)*house_list_table.size()];
				int len = house_list_table.size();
				int offset = 5;
				buffer[0] = 1;									//��ʾ��ȡ�ɹ�
				buffer[1] = len & 0xff; buffer[2] = (len >> 8) & 0xff; buffer[3] = (len >> 16) & 0xff; buffer[4] = (len >> 24) & 0xff;
				
				for (list<FHouse>::iterator i = house_list_table.begin(); i != house_list_table.end(); i ++)
				{
					fstrcpy(&buffer[offset], &(*i), sizeof(FHouse));
					offset += sizeof(FHouse);
				}
				//printf("\n\n\n");
				//printf("�û�����һ��message!\n");
				send(socket, buffer, data_len, 0);

				printf("data send completion! len:%d\n", buffer[0]);


				delete buffer;
			}
			else if (message_header.type == MESSAGE_TYPE_EXIT_HOUSE)		//�˳�����
			{
				int user_id = -1;
				fstrcpy(&user_id, (char*)(buffer + sizeof(FMessageHeader)), 4);
				for (list<FUserInfo>::iterator i = user_login_table.begin(); i != user_login_table.end(); i++)
				{
					if (i->socket == socket && i->house_id != -1)
					{
						//�жϸ��û��Ƿ��Ƿ�����
						for (list<FHouse>::iterator h = house_list_table.begin(); h != house_list_table.end(); h++)
						{
							if (h->user_id == i->user_id)
							{
								//������û�����ͬ������û�ȫ���߳�ȥ
								for (list<FUserInfo>::iterator j = user_login_table.begin(); j != user_login_table.end(); j++)
								{
									if (j->house_id == h->house_id && j->user_id != i->user_id)
									{
										j->house_id = -1;
										char buffer[5];
										buffer[0] = (char)MESSAGE_TYPE_EXIT_HOUSE;
										send(j->socket, buffer, 5, 0);
									}
								}
							}
						}

						i->house_id = -1;									//���ҵ����û����˳�����


						char b[5]; b[0] = 1;
						send(socket, b, 5, 0);
						return;
					}
				}
				
				char b[5]; b[0] = 0;
				send(socket, b, 5, 0);
			}
			//�û�Ҫ����ĳ������
			else if (message_header.type == MESSAGE_TYPE_ADD_HOUSE)
			{
				/*
				head 5
				house_id 4
				user_id 4
				*/
				int house_id, user_id;
				int success1 = 0, success2 = 0;
				fstrcpy(&house_id, (char*)(buffer  + sizeof(FMessageHeader)), 4);
				fstrcpy(&user_id, (char*)(buffer + sizeof(FMessageHeader) + 4), 4);
				list<FUserInfo>::iterator i;
				list<FHouse>::iterator j;

				printf("user_id:%d house_id:%d\n", user_id, house_id);

				for (j = house_list_table.begin(); j != house_list_table.end(); j++)
				{
					if (j->house_id == house_id)
					{
						success2 = 1;
						break;
					}
				}
				if (success2 == 0)
				{
					char buffer[5];
					buffer[0] = 0;					//����Ų�����
					send(socket, buffer, 5, 2);
					return;
				}

				for (i = user_login_table.begin(); i != user_login_table.end(); i++)
				{
					//printf("find:%d  user_id:%d\n", i->user_id, user_id);
					if (i->user_id == user_id)
					{
						i->house_id = house_id;
						success1 = 1;
						break;
					}
				}
				if (success1 == 0)
				{
					char buffer[5];
					buffer[0] = 0;					//�û�������
					send(socket, buffer, 5, 0);
					return;
				}

				printf("start\n\n");
				for (i = user_login_table.begin(); i != user_login_table.end(); i++)
				{
					printf("user_id:%d house_id:%d\n", i->user_id, i->house_id);
				}
				printf("end\n\n");
				//���뷿��ɹ�
				char buffer[5];
				buffer[0] = 1;
				send(socket, buffer, 5, 0);
			}
			//��������
			else if (message_header.type == MESSAGE_TYPE_CREATE_HOUSE)
			{
				
				FHouse *house = new FHouse;
				fstrcpy(house->house_name, (char*)(buffer+sizeof(FMessageHeader)), 32);
				//�����������ƣ�
				printf("create house name:%s\n", house->house_name);
				int user_id = -1;
				//���ҵ�Ҫ����������û���id
				for (list<FUserInfo>::iterator i = user_login_table.begin(); i != user_login_table.end(); i++)
				{
					if (i->socket == socket)
					{
						user_id = i->user_id;
					}
				}
				char buffer[5];
				//��������ɹ�
				if (user_id != -1)
				{
					house_index++;
					house->house_id = house_index;
					house->user_id = user_id;
					
					buffer[0] = 1;
					//��������ķ�����뵽socket
					house_list_table.push_back(*house);
					printf("��������ɹ�������id:%d ��������%s", house->house_id, house->house_name);
				}
				else
				{
					buffer[0] = 0;
				}
				send(socket, buffer, 5, 0);
			}
			else if (message_header.type == MESSAGE_TYPE_NORMAL)		//�û����ͳ������Ϣ
			{
				printf("send message\n\n\n");
				//��ʼ������Ϣ
				//��֤client��������Ϣ�ĳ����Ƿ���ȷ
				if (len == sizeof(message_header)+message_header.length)
				{
					//����SOCKET�����û�id
					int user_id = -1;
					int house_id = -1;
					for (list<FUserInfo>::iterator i = user_login_table.begin(); i != user_login_table.end(); i++)
					{
						printf("find completion!%d %d\n", i->socket, socket);
						//�ҵ����û����û�id�ͷ���id
						if (i->socket == socket)
						{
							user_id = i->user_id;
							house_id = i->house_id;
						}
					}
					//�ҵ��˸��û��͸�id
					if (user_id != -1 && house_id != -1)
					{
						//�������е��û�
						for (list<FUserInfo>::iterator i = user_login_table.begin(); i != user_login_table.end(); i++)
						{
							if (house_id == i->house_id)
							{
							//	printf("send message:%d house_id:%d\n", i->user_id, i->house_id);
								send(i->socket, buffer + sizeof(message_header), len, 0);
							}
						}
					}
					/*
					fd_set *fdSocket = ((FSocketSelect*)fserver)->GetFdSocket();
					for (unsigned int i = 0; i < fdSocket->fd_count; i++)
					{
						if (fdSocket->fd_array[i] != socket)
							;// send(fdSocket->fd_array[i], buffer + sizeof(message_header), len, 0);
					}
					*/
					
				}
			}
		}
	}
	virtual void CloseClient(FSocketServer *fsocket, SOCKET socket){
		printf("close client!\n");
	}
	virtual void Error(FSocketServer *fsocket, int code){
		printf("error!\n");
	}
};
int main(void){
	char s[] = "1234";
	char *p = s;
	printf("%c\n", *p++);
	printf("%c\n", *p);
	FHouse house;
	house.house_id = 1;
	strcpy(house.house_name, "fujinghui");
	house_list_table.push_front(house);
	/*
	for (list<FHouse>::iterator i = house_list_table.begin(); i != house_list_table.end(); i++)
	{

		fstrcpy(&house, &(*i), sizeof(house));
	}
	printf("%d %s\n", house.house_id, house.house_name);
	*/
	SOCKET socket_server;
	FSocketSelectListener listener;
	//��ȡ�汾��Ϊ2��WSA
	FSocketSelect fsocket(44444);
	if (fsocket.init() != 0)
	{
		printf("����WSAʧ�ܣ�\n");
		return 0;
	}
	//printf("count:%d %d %d %d\n", sizeof(message), sizeof(message.user_id), sizeof(message.group_id), sizeof(message.message));
	//����һ��TCP Socket
	socket_server = fsocket.fsocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_server == INVALID_SOCKET)
	{
		printf("����Socketʧ�ܣ�\n");
		return 0;
	}
	
	if (fsocket.fbind() == SOCKET_ERROR)
	{
		printf("Bind Socket����\n");
		return 0;
	}

	if (fsocket.flisten() == SOCKET_ERROR) 
	{
		printf("Listen����");
		return 0;
	}
	fsocket.Start(&listener);
	//64
	//select
	return 0;
}