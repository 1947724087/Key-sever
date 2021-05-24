#include "RequestCodec.h"
#include <iostream>
using namespace std;

//解码的时候使用
RequestCodec::RequestCodec() : Codec()
{
}

//编码的时候调用
RequestCodec::RequestCodec(RequestMsg * msg)
{
	// 赋值操作，需要编码的数据
	memcpy(&m_msg, msg, sizeof(RequestMsg));
}

RequestCodec::~RequestCodec()
{
}

/*
	struct RequestMsg
	{
		//1 密钥协商  	//2 密钥校验; 	// 3 密钥注销
		int		cmdType;		// 报文类型
		char	clientId[12];	// 客户端编号
		char	authCode[65];	// 认证码
		char	serverId[12];	// 服务器端编号 
		char	r1[64];			// 客户端随机数
	};
*/
int RequestCodec::msgEncode(char ** outData, int & len)
{
	//报文编码类型
	writeHeadNode(m_msg.cmdType);
	//客户端号编码
	writeNextNode(m_msg.clientId, strlen(m_msg.clientId)+1);
	//认证码编码
	writeNextNode(m_msg.authCode, strlen(m_msg.authCode) + 1);
	//服务端编号
	writeNextNode(m_msg.serverId, strlen(m_msg.serverId) + 1);
	//客户端随机数
	writeNextNode(m_msg.r1, strlen(m_msg.r1) + 1);
	//序列化
	packSequence(outData, len);

	return 0;
}

void * RequestCodec::msgDecode(char * inData, int inLen)
{
	//反序列化
	unpackSequence(inData, inLen);

	readHeadNode(m_msg.cmdType);
	readNextNode(m_msg.clientId);
	readNextNode(m_msg.authCode);
	readNextNode(m_msg.serverId);
	readNextNode(m_msg.r1);

	return &m_msg;
}
