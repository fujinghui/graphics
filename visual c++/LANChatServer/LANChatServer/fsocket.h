#ifndef _F_SOCKET_H_
#define _F_SOCKET_H_
/*
	������
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
	//�����汾��
	sockVersion = MAKEWORD(2, 2);
	return WSAStartup(sockVersion, &wsaData);
}

/* ������socket�� */
class FSocketServer{
private :
	SOCKET socket_server;
	SOCKET socket_client;
	WSAInit wsa;
	int port;
	sockaddr_in sin;	
	sockaddr_in addr_client;

public:
	//�뵱ǰ�����
	SOCKET getSocket();
	sockaddr_in *getClientAddr();
	//��socket���
	int init();
	SOCKET fsocket(int af, int type, int protocol);
	int fbind();
	int flisten();
	SOCKET faccept();
	FSocketServer(int port){
		this->port = port;
		//����socket��ַ
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

//������
class SocketSelectListener{
public:
	virtual void NewClient(FSocketServer* fsocket, SOCKET socket) = 0;
	virtual void NewMessage(FSocketServer* fsocket, SOCKET socket, const char *buffer, int len) = 0;
	virtual void CloseClient(FSocketServer* fsocket, SOCKET socket) = 0;
	virtual void Error(FSocketServer* fsocket, int code) = 0;
};

/* socket selectģ�� */
class FSocketSelect :public FSocketServer{
private:
	fd_set fdSocket;
	SOCKET socket_server;


	//�ص�����
	SocketSelectListener *listener;
public:
	FSocketSelect(int port) :FSocketServer(port){
	}
	int CreateSocket();
	void Start(SocketSelectListener *listener);		//ֻ�������¿ͻ����������Ϣ
	fd_set* GetFdSocket();							//��ȡ��ǰ�Ѿ������client socket����
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
	FD_ZERO(&fdSocket);										//��ʼ������
	socket_server = this->getSocket();
	FD_SET(socket_server, &fdSocket);
	while (true)
	{
		fd_set fdRead = fdSocket;
		//��fdRead��ɸѡ�������¼���socket
		int nRet = select(0, &fdRead, NULL, NULL, NULL);
		if (nRet > 0)
		{
			for (int i = 0; i < (int)fdSocket.fd_count; i++)
			{
				//������socket�����ǲ�����fdSocket�������
				if (FD_ISSET(fdSocket.fd_array[i], &fdRead))
				{
					//���������µĿͻ������������
					if (fdSocket.fd_array[i] == socket_server)
					{
						sockaddr_in addr_client;
						int n_len = sizeof(addr_client);
						SOCKET s_new = this->faccept();												//�õ�һ������
						FD_SET(s_new, &fdSocket);													//���뵽SOCKET������
						//���յ�һ���µ�socket���ҵ��ü��������µ�socket���ݹ�ȥ
						listener->NewClient(this, s_new);
						/*
						send(s_new, send_text, strlen(send_text), 0);
						printf("���յ�һ���ͻ������󣡿ͻ��˵�ַ:%s\n", inet_ntoa(this->getClientAddr()->sin_addr));
						printf("��ǰ�ͻ�������:%d\n", fdSocket.fd_count - 1);
						*/
					}
					else
					//�ͻ��˷�������Ϣ��ת���������Ŀͻ��ˣ�
					{
						char buffer[256];
						int nRecv = recv(fdSocket.fd_array[i], buffer, 256, 0);
						if (nRecv > 0)
						{
							buffer[nRecv] = 0;
							//���ü�����
							listener->NewMessage(this, fdSocket.fd_array[i], buffer, nRecv);
						}
						else
						{
							closesocket(fdSocket.fd_array[i]);
							/*printf("��һ���ͻ��˹ر�������\n");*/
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
	FD_SET(socket_server, &fdSocket);		//���ղŴ�����socket���뵽������
	while (true)
	{
		fd_set fdRead = fdSocket;
		//��ȡ��һ��Socket
		//�ж��Ƿ����������ӣ�������������ӣ���ô���뵽Socket������
		int nRet = select(0, &fdRead, NULL, NULL, NULL);
		if (nRet > 0)
		{
			//�ж϶���ȡ����Read��Ϣ�Ƿ��Ѿ�������
			for (int i = 0; i < (int)fdSocket.fd_count; i++)
			{
				if (FD_ISSET(fdSocket.fd_array[i], &fdRead))
				{
					//��client�����µ���Դ
					if (fdSocket.fd_array[i] == socket_server)
					{
						sockaddr_in addr_client;
						int n_len = sizeof(addr_client);
						SOCKET s_new = fsocket.faccept();
						FD_SET(s_new, &fdSocket);													//���뵽SOCKET������
						send(s_new, send_text, strlen(send_text), 0);
						printf("���յ�һ���ͻ������󣡿ͻ��˵�ַ:%s\n", inet_ntoa(fsocket.getClientAddr()->sin_addr));
						printf("��ǰ�ͻ�������:%d\n", fdSocket.fd_count - 1);
					}
					//client���������µ���Ϣ
					else
					{
						char buffer[256];
						int nRecv = recv(fdSocket.fd_array[i], buffer, 256, 0);
						//���յ�����Ϣ��ȷ
						if (nRecv > 0)
						{
							buffer[nRecv] = '\0';
							printf("���յ��ͻ��˷�������Ϣ:%s\n", buffer);
							//���յ�client����Ϣ���������client������Ϣ
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