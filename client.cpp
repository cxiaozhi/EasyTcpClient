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

// ö��ֵ����� 1 ��ʼ ���������  ��ʼ��ö��ֵ��ʱ��ͻ���һ��Ĭ��ֵ 0 ���������Ǳ������
enum CMD {
	CMD_LOGIN = 1,
	CMD_LOGIN_RESULT = 2,
	CMD_LOGOUT = 3,
	CMD_LOGOUT_RESULT = 4,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
struct DataHeader {
	short dataLength; // ���ݳ���
	short cmd = NULL; // ����
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
	// ��Ϊ������
	char szRecv[1024] = {};
	int reqLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader *header = (DataHeader *)szRecv;

	if (reqLen <= SOCKET_ERROR) {
		printf("��������Ͽ����� ������� \n");
		return -1;
	}
	// �ж�ճ�� �ٰ�
	/*if (reqLen >= sizeof(DataHeader)) {
		return 0;
	}*/

	switch (header->cmd) {
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginRes *res = (LoginRes *)szRecv;
			cout << "��������Ϣ: " << res->msg << endl;
		}break;

		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogOutMsg *res = (LogOutMsg *)szRecv;
			cout << "��������Ϣ: " << res->msg << endl;

		}break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin *res = (NewUserJoin *)szRecv;
			cout << "��������Ϣ: ���û�" << res->sockId << "���뷿��" << endl;

		}break;
	}
}


int main() {
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	// 1. �½�socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	// ����socket���ӵ�ַ�ṹ��
	sockaddr_in _sin;
	_sin.sin_family = AF_INET;
	const char* ipAddr = "127.0.0.1";
	_sin.sin_port = htons(8999);
	inet_pton(AF_INET, ipAddr, &_sin.sin_addr);
	// 2. ���ӷ�����
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	// assert(ret != SOCKET_ERROR);
	if (ret == SOCKET_ERROR) {
		printf("����socket ʧ��...\n");
	} else {
		printf("����socket �ɹ�...\n");
	};

	while (true) {
		// ������ socket
		fd_set fdRead = {};
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);

		timeval t = { 1,0 };

		int ret = select(_sock + 1, &fdRead, 0, 0, &t);

		if (ret < 0) {
			printf("select ������� \n");
			break;
		}

		if (FD_ISSET(_sock, &fdRead)) {
			FD_CLR(_sock, &fdRead);

			if (-1 == processor(_sock)) {
				printf("select ������� \n");
				break;
			}
		}
		printf("����ʱ�䴦������ҵ�� \n");
		Login login;
		strcpy_s(login.userName, "byd");
		strcpy_s(login.passWord, "123456");
		send(_sock, (char *)&login, sizeof(Login), 0);
		Sleep(1000);
	}

	// 4. �رտͻ����׽���
	closesocket(_sock);
	// ���window socket����
	WSACleanup();
	return 0;
}