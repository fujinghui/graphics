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
			//获取信息头部信息
			fstrcpy((char*)&message_header, (void*)buffer, sizeof(message_header));

			if (message_header.type == MESSAGE_TYPE_LOGIN)			//用户执行登录操作
			{
				fstrcpy(((char*)&login_info) + 4, (void*)(buffer + sizeof(FMessageHeader)), sizeof(FUserInfo));
				printf("user name:%s\n", login_info.user_name);
				printf("user password:%s\n", login_info.user_password);
				//用户登录成功！
				if (user_operator.login(&login_info) == 1)
				{
					char buffer[5];
					buffer[0] = 1;
					fstrcpy(buffer + 1, &login_info.user_id, 4);
					send(socket, buffer, 5, 0);
					//用户登录成功，将当前用户插入到已登录的信息表中
					FUserInfo *user_info = new FUserInfo;						//为新的用户信息声明一段内存
					fstrcpy(user_info, &login_info, sizeof(FUserInfo));			//将用户信息拷贝到user_info中
					user_info->house_id = -1;
					user_info->socket = socket;	
					login_info.house_id = -1;
					login_info.socket = socket;
					//保存客户端的socket
					user_login_table.push_front(login_info);						//将刚才登陆的用户信息添加到用户登陆表中
					printf("用户登录成功！用户id:%d\n", login_info.user_id);
				}
				//用户登录失败！
				else
				{
					char buffer[5];
					buffer[1] = 0;
					send(socket, buffer, 5, 0);
				}
			}
			else if (message_header.type == MESSAGE_TYPE_REGISTER)		//用户执行注册操作
			{
				fstrcpy(((char*)&register_info) + 4, (void*)(buffer + sizeof(FMessageHeader)), sizeof(FUserInfo));
			//	printf("user name:%s\n", register_info.user_name);
			//	printf("user password:%s\n", register_info.user_password);
				int state = user_operator.save(&register_info);							//注册一条信息
				//将返回的信息添加进去
				char buffer[5];
				buffer[0] = state;
				send(socket, buffer, 5, 0);							//发送一条信息
			}
			else if (message_header.type == MESSAGE_TYPE_LOGOUT)		//用户执行注销操作
			{
				//从用户登录表中删除当前用户信息
				fstrcpy(((char*)&logout_info), (void*)(buffer + sizeof(FMessageHeader)), sizeof(FUserInfo));
				list<FUserInfo>::iterator info = user_login_table.begin();
				while (info != user_login_table.end())
				{
					//id相同，删除该信息
					if (info->user_id == logout_info.user_id)
					{
						printf("删除到一个用户!用户id:%d 用户名：%s 密码：%s\n", info->user_id, info->user_name, info->user_password);
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
				buffer[0] = 1;									//表示获取成功
				buffer[1] = len & 0xff; buffer[2] = (len >> 8) & 0xff; buffer[3] = (len >> 16) & 0xff; buffer[4] = (len >> 24) & 0xff;
				
				for (list<FHouse>::iterator i = house_list_table.begin(); i != house_list_table.end(); i ++)
				{
					fstrcpy(&buffer[offset], &(*i), sizeof(FHouse));
					offset += sizeof(FHouse);
				}
				//printf("\n\n\n");
				//printf("用户请求一个message!\n");
				send(socket, buffer, data_len, 0);

				printf("data send completion! len:%d\n", buffer[0]);


				delete buffer;
			}
			else if (message_header.type == MESSAGE_TYPE_EXIT_HOUSE)		//退出房间
			{
				int user_id = -1;
				fstrcpy(&user_id, (char*)(buffer + sizeof(FMessageHeader)), 4);
				for (list<FUserInfo>::iterator i = user_login_table.begin(); i != user_login_table.end(); i++)
				{
					if (i->socket == socket && i->house_id != -1)
					{
						//判断该用户是否是房间主
						for (list<FHouse>::iterator h = house_list_table.begin(); h != house_list_table.end(); h++)
						{
							if (h->user_id == i->user_id)
							{
								//将与该用户在相同房间的用户全部踢出去
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

						i->house_id = -1;									//查找到该用户，退出房间


						char b[5]; b[0] = 1;
						send(socket, b, 5, 0);
						return;
					}
				}
				
				char b[5]; b[0] = 0;
				send(socket, b, 5, 0);
			}
			//用户要加入某个房间
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
					buffer[0] = 0;					//房间号不存在
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
					buffer[0] = 0;					//用户不存在
					send(socket, buffer, 5, 0);
					return;
				}

				printf("start\n\n");
				for (i = user_login_table.begin(); i != user_login_table.end(); i++)
				{
					printf("user_id:%d house_id:%d\n", i->user_id, i->house_id);
				}
				printf("end\n\n");
				//加入房间成功
				char buffer[5];
				buffer[0] = 1;
				send(socket, buffer, 5, 0);
			}
			//创建房间
			else if (message_header.type == MESSAGE_TYPE_CREATE_HOUSE)
			{
				
				FHouse *house = new FHouse;
				fstrcpy(house->house_name, (char*)(buffer+sizeof(FMessageHeader)), 32);
				//创建房间名称！
				printf("create house name:%s\n", house->house_name);
				int user_id = -1;
				//查找到要创建房间的用户的id
				for (list<FUserInfo>::iterator i = user_login_table.begin(); i != user_login_table.end(); i++)
				{
					if (i->socket == socket)
					{
						user_id = i->user_id;
					}
				}
				char buffer[5];
				//创建房间成功
				if (user_id != -1)
				{
					house_index++;
					house->house_id = house_index;
					house->user_id = user_id;
					
					buffer[0] = 1;
					//将创建完的房间加入到socket
					house_list_table.push_back(*house);
					printf("创建房间成功！房间id:%d 房间名：%s", house->house_id, house->house_name);
				}
				else
				{
					buffer[0] = 0;
				}
				send(socket, buffer, 5, 0);
			}
			else if (message_header.type == MESSAGE_TYPE_NORMAL)		//用户发送常规的消息
			{
				printf("send message\n\n\n");
				//开始发送消息
				//验证client发来的消息的长度是否正确
				if (len == sizeof(message_header)+message_header.length)
				{
					//根据SOCKET查找用户id
					int user_id = -1;
					int house_id = -1;
					for (list<FUserInfo>::iterator i = user_login_table.begin(); i != user_login_table.end(); i++)
					{
						printf("find completion!%d %d\n", i->socket, socket);
						//找到该用户的用户id和房间id
						if (i->socket == socket)
						{
							user_id = i->user_id;
							house_id = i->house_id;
						}
					}
					//找到了该用户和该id
					if (user_id != -1 && house_id != -1)
					{
						//查找所有的用户
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
	//获取版本号为2的WSA
	FSocketSelect fsocket(44444);
	if (fsocket.init() != 0)
	{
		printf("创建WSA失败！\n");
		return 0;
	}
	//printf("count:%d %d %d %d\n", sizeof(message), sizeof(message.user_id), sizeof(message.group_id), sizeof(message.message));
	//创建一个TCP Socket
	socket_server = fsocket.fsocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_server == INVALID_SOCKET)
	{
		printf("创建Socket失败！\n");
		return 0;
	}
	
	if (fsocket.fbind() == SOCKET_ERROR)
	{
		printf("Bind Socket出错！\n");
		return 0;
	}

	if (fsocket.flisten() == SOCKET_ERROR) 
	{
		printf("Listen出错！");
		return 0;
	}
	fsocket.Start(&listener);
	//64
	//select
	return 0;
}