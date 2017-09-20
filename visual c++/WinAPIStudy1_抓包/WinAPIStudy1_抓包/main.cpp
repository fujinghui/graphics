#include <stdio.h>
#include <winsock2.h>
#include <mstcpip.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

#pragma comment(lib, "ws2_32.lib")		//加载Socket动态库 

#define OK 0
#define ERR -1
#define HOST_NAME_LEN 256
#define MY_PORT 8000
#define BUFF_LEN 65536
typedef struct tagIP_HEADER //定义IP首部
{
	unsigned char h_lenver; //4位首部长度+4位IP版本号
	unsigned char tos; //8位服务类型TOS
	unsigned short total_len; //16位总长度（字节）
	unsigned short ident; //16位标识
	unsigned short frag_and_flags; //3位标志位
	unsigned char ttl; //8位生存时间 TTL
	unsigned char proto; //8位协议 (TCP, UDP 或其他)
	unsigned short checksum; //16位IP首部校验和
	unsigned int sourceIP; //32位源IP地址
	unsigned int destIP; //32位目的IP地址
}IP_HEADER;
void PrintData(char szBuf[], int iLen){
	//打印16进制数据
	int i;
	IP_HEADER ip_header;
	unsigned int ip1, ip2;
	memcpy(&ip_header, szBuf, sizeof(IP_HEADER));
	ip1 = ip_header.sourceIP;
	ip2 = ip_header.destIP;
	printf("sources ip address:%d.%d.%d.%d\n", ip1 & 0xff, ip1 >> 8 & 0xff, ip1 >> 16 & 0xff, ip1 >> 24 & 0xff);
	printf("des ip address:%d.%d.%d.%d\n\n", ip2 & 0xff, ip2 >> 8 & 0xff, ip2 >> 16 & 0xff, ip2 >> 24 & 0xff);
	//for (i = 0; i < iLen; i++)
	//{
	//	printf("%02x ", szBuf[i] & 0xff);
	//}
	printf("\n");
}
int CaptureData(){
	SOCKET iFd;
	char szHostName[HOST_NAME_LEN];
	HOSTENT *pHost;
	WSAData stWsa;			//用于打开动态链接库 
	SOCKADDR_IN stAddr = { 0 };
	DWORD dwInBuff = 1;
	DWORD dwOutBuff;
	DWORD dwBytesRet;
	int iRet;
	char szBuf[BUFF_LEN];
	//0x0202 版本号 
	if (WSAStartup(0x0202, &stWsa) != OK)
	{
		printf("WSA startup failed\n");
		return ERR;
	}

	if (gethostname(szHostName, HOST_NAME_LEN) != OK)
	{
		WSACleanup();
		return ERR;
	}
	printf("host name:%s\n", szHostName);
	pHost = gethostbyname(szHostName);
	printf("ip:%d.%d.%d.%d\n", pHost->h_addr_list[0][0] & 0xff,
		pHost->h_addr_list[0][1] & 0xff,
		pHost->h_addr_list[0][2] & 0xff,
		pHost->h_addr_list[0][3] & 0xff);

	stAddr.sin_addr.s_addr = *(unsigned long*)pHost->h_addr_list[0];
	//stAddr.sin_addr.S_un.S_addr = *(unsigned long*)pHost->h_addr_list[0];
	stAddr.sin_family = AF_INET;
	stAddr.sin_port = htons(MY_PORT);			//我的端口

	//创建原始套接字
	iFd = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (iFd < 0)
	{
		WSACleanup();
		return ERR;
	}
	if (bind(iFd, (struct sockaddr *)&stAddr, sizeof(SOCKADDR)) != OK)
	{
		WSACleanup();
		closesocket(iFd);
		return ERR;
	}

	//设置iFd守包规则
	iRet = WSAIoctl(iFd, SIO_RCVALL, &dwInBuff, sizeof(dwInBuff), &dwOutBuff, sizeof(dwOutBuff), &dwBytesRet, NULL, NULL);
	if (iRet != OK)
	{
		printf("IOCTL faild");
		WSACleanup();
		closesocket(iFd);
		return ERR;
	}
	while (1)
	{
		memset(szBuf, 0, BUFF_LEN);
		//返回值为实际返回的数据长度
		iRet = recv(iFd, szBuf, BUFF_LEN, 0);
		if (iRet < 0)
		{
			break;
		}
		else
		{
			PrintData(szBuf, iRet);
		}
	}



	WSACleanup();
	closesocket(iFd);
	return OK;
}

int main(int argc, char** argv) {
	CaptureData();
	return 0;
}