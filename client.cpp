#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <WS2tcpip.h>
#include <assert.h>


#define BUF_SIZE 128

typedef struct {
	int age;
	int  name[32];
}DataPackage;
int main() {
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	// 1. �½�socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	// ����socket���ӵ�ַ�ṹ��
	sockaddr_in	_sin;
	_sin.sin_family = AF_INET;
	const char* ipAddr = "127.0.0.1";
	_sin.sin_port = htons(8999);
	inet_pton(AF_INET, ipAddr, &_sin.sin_addr);
	// 2. ���ӷ�����
	int ret = connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in));
	//assert(ret != SOCKET_ERROR);
	if (ret == SOCKET_ERROR) {
		printf("����socket ʧ��...\n");
	}
	else
	{
		printf("����socket �ɹ�...\n");
	};
	// 3. ���ܷ�������Ϣ
	/*char recvFirstBuf[BUF_SIZE] = {};
	ret = recv(_sock, recvFirstBuf, BUF_SIZE - 1, 0);
	if (ret > 0) {
		printf("from server: %s len: %d \n", recvFirstBuf, ret);
	}*/


	while (true)
	{

		char cmdBuf[BUF_SIZE] = {};
		// ��������
		scanf_s("%s", cmdBuf, BUF_SIZE);
		if (0 == strcmp(cmdBuf, "exit")) {
			printf("���˳�");
			break;
		}
		else
		{
			send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);// �����len ����ֱ����BUF_SIZE ����Ҫ���������ʵ���� ��Ȼ��෢��һ���ո�
		}

		// 3. ���ܷ�������Ϣ
		char recvBuf[BUF_SIZE] = {};
		ret = recv(_sock, recvBuf, BUF_SIZE - 1, 0);
		if (ret > 0) {
			DataPackage*dp = (DataPackage *)recvBuf;
			printf("�յ�����: ����=%s ����=%d \n", dp->name, dp->age);
		}
	}
	// 4. �رշ�����׽���
	closesocket(_sock);
	// ���window socket����
	WSACleanup();
	getchar();
	return 0;
}