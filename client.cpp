#define WIN32_LEAN_AND_MEAN

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#define BUF_SIZE 128
using namespace std;

// 枚举值最好以 1 开始 如果不定义  初始化枚举值的时候就会有一个默认值 0 这样会带来潜在问题
enum CMD {
	CMD_LOGIN = 1,
	CMD_LOGIN_RESULT = 2,
	CMD_LOGOUT = 3,
	CMD_LOGOUT_RESULT = 4,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
struct DataHeader {
	short dataLength; // 数据长度
	short cmd = NULL; // 命令
};

typedef struct Login : public DataHeader {
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
}Login;

struct LoginRes : public DataHeader {
	LoginRes() {
		dataLength = sizeof(LoginRes);
		cmd = CMD_LOGIN_RESULT;
	}
	int code;
	char msg[128];
};

struct LogOut :public DataHeader {
	LogOut() {
		dataLength = sizeof(LogOut);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

typedef struct LogOutMsg :public DataHeader {
	LogOutMsg() {
		dataLength = sizeof(LogOutMsg);
		cmd = CMD_LOGOUT_RESULT;
	}
	int code;
	char msg[128];
}LogOutMsg;

struct CmdError :public DataHeader {
	CmdError() {
		dataLength = sizeof(CmdError);
	}
	char msg[128];
};

struct NewUserJoin :public DataHeader {
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sockId = 0;
	}
	int sockId;
};

int processor(SOCKET _cSock) {
	// 作为缓冲区
	char szRecv[1024] = {};
	int reqLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader *header = (DataHeader *)szRecv;

	if (reqLen <= SOCKET_ERROR) {
		printf("与服务器断开连接 任务结束 \n");
		return -1;
	}
	// 判断粘包 少包
	/*if (reqLen >= sizeof(DataHeader)) {
		return 0;
	}*/

	switch (header->cmd) {
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginRes *res = (LoginRes *)szRecv;
			cout << "服务器消息: " << res->msg << endl;
		}break;

		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogOutMsg *res = (LogOutMsg *)szRecv;
			cout << "服务器消息: " << res->msg << endl;

		}break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin *res = (NewUserJoin *)szRecv;
			cout << "服务器消息: 新用户" << res->sockId << "加入房间" << endl;

		}break;
	}
}


int main() {
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	// 1. 新建socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	// 定义socket连接地址结构体
	sockaddr_in _sin;
	_sin.sin_family = AF_INET;
	const char* ipAddr = "127.0.0.1";
	_sin.sin_port = htons(8999);
	inet_pton(AF_INET, ipAddr, &_sin.sin_addr);
	// 2. 连接服务器
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	// assert(ret != SOCKET_ERROR);
	if (ret == SOCKET_ERROR) {
		printf("连接socket 失败...\n");
	} else {
		printf("连接socket 成功...\n");
	};

	while (true) {
		// 伯克利 socket
		fd_set fdRead = {};
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);

		timeval t = { 1,0 };

		int ret = select(_sock + 1, &fdRead, 0, 0, &t);

		if (ret < 0) {
			printf("select 任务结束 \n");
			break;
		}

		if (FD_ISSET(_sock, &fdRead)) {
			FD_CLR(_sock, &fdRead);

			if (-1 == processor(_sock)) {
				printf("select 任务结束 \n");
				break;
			}
		}
		printf("空闲时间处理其它业务 \n");
		Login login;
		strcpy_s(login.userName, "byd");
		strcpy_s(login.passWord, "123456");
		send(_sock, (char *)&login, sizeof(Login), 0);
		Sleep(1000);
	}

	// 4. 关闭客户端套接字
	closesocket(_sock);
	// 清除window socket环境
	WSACleanup();
	return 0;
}