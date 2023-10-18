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


enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};
typedef struct {
	short dataLength;  // ���ݳ���
	CMD cmd;         // ����
} DataHeader;

typedef struct Login : public DataHeader {
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];

}LoginPackage;

typedef struct LoginRes : public DataHeader {
	LoginRes() {
		dataLength = sizeof(LoginRes);
	}
	int code;
	char msg[128];
}LoginResult;

typedef struct LogOut :public DataHeader {
	LogOut() {
		dataLength = sizeof(LogOut);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
}LogOut;

typedef struct LogOutMsg :public DataHeader {
	LogOutMsg() {
		dataLength = sizeof(LogOutMsg);
	}
	char msg[128];
}LogOutMsg;

struct CmdError :public DataHeader {
	CmdError() {
		dataLength = sizeof(CmdError);
	}
	char msg[128];
};

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
		char cmdBuf[BUF_SIZE] = {};
		// ��������
		scanf_s("%s", cmdBuf, sizeof(cmdBuf));

		if (0 == strcmp(cmdBuf, "exit")) {
			printf("�����˳�...");
			break;
		} else if (0 == strcmp(cmdBuf, "login")) {
			// �����˷�����������
			LoginPackage login;  // �ṹ���ʼ������
			strcpy_s(login.userName, "byd");
			strcpy_s(login.passWord, "123456");
			send(_sock, (char*)&login, sizeof(LoginPackage), 0);
			// ���շ��������ص�����
			LoginResult resBody;
			recv(_sock, (char*)&resBody, sizeof(LoginResult), 0);
			cout << "�������Ϣ��" << resBody.code << resBody.msg << endl;
		} else if (0 == strcmp(cmdBuf, "logout")) {
			LogOut logout;
			strcpy_s(logout.userName, "byd");
			send(_sock, (char*)&logout, sizeof(logout), 0);

			// ���ܷ��������ص�����
			LogOutMsg resBody = {};
			recv(_sock, (char*)&resBody, sizeof(resBody), 0);
			printf("�������Ϣ: %s\n", resBody.msg);
		} else {
			printf("��֧�ֵ��������������\n");
		}
	}
	// 4. �رտͻ����׽���
	closesocket(_sock);
	// ���window socket����
	WSACleanup();
	getchar();
	return 0;
}