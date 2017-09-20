#include <stdio.h>
#include <winsock2.h>
#include <mstcpip.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

#pragma comment(lib, "ws2_32.lib")		//����Socket��̬�� 

#define OK 0
#define ERR -1
#define HOST_NAME_LEN 256
#define MY_PORT 8000
#define BUFF_LEN 65536
typedef struct tagIP_HEADER //����IP�ײ�
{
	unsigned char h_lenver; //4λ�ײ�����+4λIP�汾��
	unsigned char tos; //8λ��������TOS
	unsigned short total_len; //16λ�ܳ��ȣ��ֽڣ�
	unsigned short ident; //16λ��ʶ
	unsigned short frag_and_flags; //3λ��־λ
	unsigned char ttl; //8λ����ʱ�� TTL
	unsigned char proto; //8λЭ�� (TCP, UDP ������)
	unsigned short checksum; //16λIP�ײ�У���
	unsigned int sourceIP; //32λԴIP��ַ
	unsigned int destIP; //32λĿ��IP��ַ
}IP_HEADER;
void PrintData(char szBuf[], int iLen){
	//��ӡ16��������
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
	WSAData stWsa;			//���ڴ򿪶�̬���ӿ� 
	SOCKADDR_IN stAddr = { 0 };
	DWORD dwInBuff = 1;
	DWORD dwOutBuff;
	DWORD dwBytesRet;
	int iRet;
	char szBuf[BUFF_LEN];
	//0x0202 �汾�� 
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
	stAddr.sin_port = htons(MY_PORT);			//�ҵĶ˿�

	//����ԭʼ�׽���
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

	//����iFd�ذ�����
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
		//����ֵΪʵ�ʷ��ص����ݳ���
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