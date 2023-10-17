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

	// 1. 新建socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	// 定义socket连接地址结构体
	sockaddr_in	_sin;
	_sin.sin_family = AF_INET;
	const char* ipAddr = "127.0.0.1";
	_sin.sin_port = htons(8999);
	inet_pton(AF_INET, ipAddr, &_sin.sin_addr);
	// 2. 连接服务器
	int ret = connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in));
	//assert(ret != SOCKET_ERROR);
	if (ret == SOCKET_ERROR) {
		printf("连接socket 失败...\n");
	}
	else
	{
		printf("连接socket 成功...\n");
	};
	// 3. 接受服务器消息
	/*char recvFirstBuf[BUF_SIZE] = {};
	ret = recv(_sock, recvFirstBuf, BUF_SIZE - 1, 0);
	if (ret > 0) {
		printf("from server: %s len: %d \n", recvFirstBuf, ret);
	}*/


	while (true)
	{

		char cmdBuf[BUF_SIZE] = {};
		// 发送命令
		scanf_s("%s", cmdBuf, BUF_SIZE);
		if (0 == strcmp(cmdBuf, "exit")) {
			printf("已退出");
			break;
		}
		else
		{
			send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);// 这里的len 不能直接填BUF_SIZE 必须要求出它的真实长度 不然会多发送一个空格
		}

		// 3. 接受服务器消息
		char recvBuf[BUF_SIZE] = {};
		ret = recv(_sock, recvBuf, BUF_SIZE - 1, 0);
		if (ret > 0) {
			DataPackage*dp = (DataPackage *)recvBuf;
			printf("收到数据: 姓名=%s 年龄=%d \n", dp->name, dp->age);
		}
	}
	// 4. 关闭服务端套接字
	closesocket(_sock);
	// 清除window socket环境
	WSACleanup();
	getchar();
	return 0;
}