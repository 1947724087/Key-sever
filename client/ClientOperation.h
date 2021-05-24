#pragma once
#include<iostream>
//#include<stralign.h>
#include<stdlib.h>
#include"TcpSocket.h"
#include"KeyShm.h"
#include"ShareMemory.h"
#include"TcpSocket.h"
#include"KeyShm.h"
#include"RequestCodec.h"
#include"CodecFactory.h"
#include"RequestFactory.h"
#include"RespondFactory.h"
#include<openssl/hmac.h>
#include<openssl/evp.h>
#include<openssl/sha.h>

using namespace std;

class ClientInfo
{
public:
	char clientID[12];
	char severID[12];
	char severIP[32];
	unsigned short severPort;
	int maxNode;
	int shmKey;
};

class ClientOperation
{
public:
	ClientOperation();
	ClientOperation(ClientInfo *info);
	~ClientOperation();
	
	//密钥协商
	int keyAgree();
	//密钥校验
	int keyCheck();
	//密钥注销
	int keyRevock();
	//密钥查看
	int keyView();

private:
	void getRandString(char *randBuf,int len);
	void generateHmac(char *Hmac);
private:
	ClientInfo m_info;
	TcpSocket m_socket;
	KeyShm *m_shm;
};

