#ifndef _F_SOCKET_H_
#define _F_SOCKET_H_
/*
	监听器
	*/
#include "fmessage.h"


class WSAInit{
private:
	WORD sockVersion;
	WSADATA wsaData;
public:
	int init();
};

int WSAInit::init(){
	//创建版本号
	sockVersion = MAKEWORD(2, 2);
	return WSAStartup(sockVersion, &wsaData);
}

/* 基础的socket类 */
class FSocketServer{
private :
	SOCKET socket_server;
	SOCKET socket_client;
	WSAInit wsa;
	int port;
	sockaddr_in sin;	
	sockaddr_in addr_client;

public:
	//与当前类相关
	SOCKET getSocket();
	sockaddr_in *getClientAddr();
	//与socket相关
	int init();
	SOCKET fsocket(int af, int type, int protocol);
	int fbind();
	int flisten();
	SOCKET faccept();
	FSocketServer(int port){
		this->port = port;
		//设置socket地址
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
		sin.sin_addr.S_un.S_addr = INADDR_ANY;
	}
};

SOCKET FSocketServer::fsocket(int af, int type, int protocol){
	socket_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return socket_server;
}
SOCKET FSocketServer::faccept(){
	int len = sizeof(addr_client);
	socket_client = accept(socket_server, (LPSOCKADDR)&addr_client, &len);
	return socket_client;
}
int FSocketServer::flisten(){
	return listen(socket_server, 5);
}
int FSocketServer::fbind(){
	return bind(socket_server, (LPSOCKADDR)&sin, sizeof(sin));
}
int FSocketServer::init(){
	return wsa.init();
}


SOCKET FSocketServer::getSocket(){
	return socket_server;
}
sockaddr_in* FSocketServer::getClientAddr(){
	return &addr_client;
}

//监听器
class SocketSelectListener{
public:
	virtual void NewClient(FSocketServer* fsocket, SOCKET socket) = 0;
	virtual void NewMessage(FSocketServer* fsocket, SOCKET socket, const char *buffer, int len) = 0;
	virtual void CloseClient(FSocketServer* fsocket, SOCKET socket) = 0;
	virtual void Error(FSocketServer* fsocket, int code) = 0;
};

/* socket select模型 */
class FSocketSelect :public FSocketServer{
private:
	fd_set fdSocket;
	SOCKET socket_server;


	//回调函数
	SocketSelectListener *listener;
public:
	FSocketSelect(int port) :FSocketServer(port){
	}
	int CreateSocket();
	void Start(SocketSelectListener *listener);		//只处理有新客户端请求的消息
	fd_set* GetFdSocket();							//获取当前已经加入的client socket集合
};
int FSocketSelect::CreateSocket(){
	if (this->init() != 0)
		return -1;
	if (this->fsocket(AF_INET, SOCK_STREAM, IPPROTO_TCP) == INVALID_SOCKET)
		return -1;
	if (this->fbind() == SOCKET_ERROR)
		return -1;
	if (this->flisten() == SOCKET_ERROR)
		return -1;
	return 0;
}
fd_set* FSocketSelect::GetFdSocket(){
	return &fdSocket;
}
void FSocketSelect::Start(SocketSelectListener *listener){
	this->listener = listener;
	char send_text[] = "TCP Server!\n\0";
	FD_ZERO(&fdSocket);										//初始化集合
	socket_server = this->getSocket();
	FD_SET(socket_server, &fdSocket);
	while (true)
	{
		fd_set fdRead = fdSocket;
		//从fdRead中筛选产生了事件的socket
		int nRet = select(0, &fdRead, NULL, NULL, NULL);
		if (nRet > 0)
		{
			for (int i = 0; i < (int)fdSocket.fd_count; i++)
			{
				//产生的socket请求是产生在fdSocket集合里的
				if (FD_ISSET(fdSocket.fd_array[i], &fdRead))
				{
					//这里是有新的客户端请求服务器
					if (fdSocket.fd_array[i] == socket_server)
					{
						sockaddr_in addr_client;
						int n_len = sizeof(addr_client);
						SOCKET s_new = this->faccept();												//得到一个连接
						FD_SET(s_new, &fdSocket);													//加入到SOCKET集合中
						//接收到一个新的socket并且调用监听器将新的socket传递过去
						listener->NewClient(this, s_new);
						/*
						send(s_new, send_text, strlen(send_text), 0);
						printf("接收到一个客户端请求！客户端地址:%s\n", inet_ntoa(this->getClientAddr()->sin_addr));
						printf("当前客户端数量:%d\n", fdSocket.fd_count - 1);
						*/
					}
					else
					//客户端发送来消息（转发给其他的客户端）
					{
						char buffer[256];
						int nRecv = recv(fdSocket.fd_array[i], buffer, 256, 0);
						if (nRecv > 0)
						{
							buffer[nRecv] = 0;
							//调用监听器
							listener->NewMessage(this, fdSocket.fd_array[i], buffer, nRecv);
						}
						else
						{
							closesocket(fdSocket.fd_array[i]);
							/*printf("有一个客户端关闭了连接\n");*/
							listener->CloseClient(this, fdSocket.fd_array[i]);
							FD_CLR(fdSocket.fd_array[i], &fdSocket);
						}
					}
				}
				else
				{
					//error
					//listener->Error(0);
				}
			}
		}
	}

	/*
	FD_SET(socket_server, &fdSocket);		//将刚才创建的socket加入到集合中
	while (true)
	{
		fd_set fdRead = fdSocket;
		//读取到一个Socket
		//判断是否是请求连接，如果是请求连接，那么加入到Socket集合中
		int nRet = select(0, &fdRead, NULL, NULL, NULL);
		if (nRet > 0)
		{
			//判断读获取到的Read消息是否已经存在了
			for (int i = 0; i < (int)fdSocket.fd_count; i++)
			{
				if (FD_ISSET(fdSocket.fd_array[i], &fdRead))
				{
					//有client请求新的资源
					if (fdSocket.fd_array[i] == socket_server)
					{
						sockaddr_in addr_client;
						int n_len = sizeof(addr_client);
						SOCKET s_new = fsocket.faccept();
						FD_SET(s_new, &fdSocket);													//加入到SOCKET集合中
						send(s_new, send_text, strlen(send_text), 0);
						printf("接收到一个客户端请求！客户端地址:%s\n", inet_ntoa(fsocket.getClientAddr()->sin_addr));
						printf("当前客户端数量:%d\n", fdSocket.fd_count - 1);
					}
					//client发送来了新的消息
					else
					{
						char buffer[256];
						int nRecv = recv(fdSocket.fd_array[i], buffer, 256, 0);
						//接收到的消息正确
						if (nRecv > 0)
						{
							buffer[nRecv] = '\0';
							printf("接收到客户端发来的消息:%s\n", buffer);
							//接收到client的消息后给其他的client发送消息
							for (int j = 0; j < fdSocket.fd_count; j++)
							{
								if (fdSocket.fd_array[i] != socket_server)
								{
									send(fdSocket.fd_array[j], buffer, strlen(buffer), 0);
								}
							}
						}
						else
						{
							printf("close connection!");
							closesocket(fdSocket.fd_array[i]);
							FD_CLR(fdSocket.fd_array[i], &fdSocket);
						}
					}
				}
			}
		}
	}
	*/
}

#endif