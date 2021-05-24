#include "ClientOperation.h"

ClientOperation::ClientOperation()
{
	
}

ClientOperation::ClientOperation(ClientInfo * info)
{
	memcpy(&m_info, info, sizeof(ClientInfo));
	//´´½¨¹²ÏíÄÚ´æ
	m_shm = new KeyShm(m_info.shmKey, m_info.maxNode);
}

ClientOperation::~ClientOperation()
{
}

int ClientOperation::keyAgree()
{
	//×¼±¸·¢ËÍÊý¾Ý
	RequestMsg ReqMsg;
	memset(&ReqMsg, 0x00, sizeof(RequestMsg));
	ReqMsg.cmdType = RequestCodec::NewOrUpdate;
	strcpy(ReqMsg.clientId, m_info.clientID);
	strcpy(ReqMsg.serverId, m_info.severID);
	char buf[64];
	getRandString(ReqMsg.r1, sizeof(ReqMsg.r1));
	//Ê¹ÓÃHMACÉú³É¹þÏ£Öµ----ÏûÏ¢ÈÏÖ¤Âë
	char key[64];
	memset(key, 0x00, sizeof(key));
	unsigned char md[20];
	unsigned int len;
	char *outData = NULL;
	int dataLen;
	sprintf(key, "%s%s", ReqMsg.clientId, ReqMsg.serverId);
	HMAC(EVP_sha1(),key,strlen(key),(unsigned char *)ReqMsg.r1
		,strlen(ReqMsg.r1),md,&len);

	for (int i = 0; i < 20; i++)
	{
		sprintf(&ReqMsg.authCode[i * 2], "%02x", md[i]);
	}
	//±àÂë
	CodecFactory *factory = new RequestFactory(&ReqMsg);
	Codec *pCode=factory->createCodec();
	pCode->msgEncode(&outData, dataLen);
	delete factory;
	delete pCode;

	//·¢ËÍÊý¾Ý
	cout<<"开始连接服务器"<<endl;
	m_socket.connectToHost(m_info.severIP, m_info.severPort);
	//·¢ËÍÇëÇóÊý¾Ý
	m_socket.sendMsg(outData, dataLen);
	//µÈ´ýÓ¦´ð
	char *inData;
	m_socket.recvMsg(&inData, dataLen);
	//½âÂë
	factory = new RespondFactory();
	pCode = factory->createCodec();
	RespondMsg *pMsg = (RespondMsg *)pCode->msgDecode(inData, dataLen);
	//delete factory;
	//ÅÐ¶ÏÊÇ·ñ³É¹¦
	if (pMsg->rv == -1)
	{
		cout << "faailed" << endl;
	}
	else
	{
		cout << "decode sucessed" << endl;
	}
	//·þÎñ¶Ër2¿Í»§¶Ër1Æ´½ÓÉú³ÉÃÜÔ¿
	char buf1[128];
	char mdSha1[SHA_DIGEST_LENGTH * 2 + 1] = {0};
	memset(buf1, 0x00, sizeof(buf1));
	sprintf((char *)buf1, "%s%s", ReqMsg.r1, pMsg->r2);
	//SHA1(buf,strlen(buf),md)
	SHA1((const unsigned char *)buf1, strlen(buf1), md);
	
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&mdSha1[i * 2], "%02x", md[i]);
	}
	//¸³Öµ¸øÃÜÔ¿½á¹¹Ìå
	cout<<"客户端密钥生成的："<<mdSha1<<endl;
	NodeShmInfo node;
	memset(&node, 0x00, sizeof(NodeShmInfo));
	strcpy(node.clientID, m_info.clientID);
	strcpy(node.seckey, mdSha1);
	strcpy(node.severID, m_info.severID);
	node.seckeyID = pMsg->seckeyid;
	node.status = 0;
	
	//½«ÃÜÔ¿ÏûÏ¢Ð´Èë¹²ÏíÄÚ´æ
	m_shm->KeyShmWR(&node);
	m_socket.disConnect();

	//generateHmac(ReqMsg.authCode);
	delete factory;
	delete pCode;
	return 0;
}

int ClientOperation::keyCheck()
{
	return 0;
}

int ClientOperation::keyRevock()
{
	return 0;
}

int ClientOperation::keyView()
{
	return 0;
}

void ClientOperation::getRandString(char * prandBuf,int len)
{
	int flag;
	srand(time(NULL));
	for (int i = 0; i < len - 1; i++)
	{
		flag = rand() % 4;
		char buf[] = "~!#$%^&*()+-|\?><>";
		switch (flag)
		{
		case 0:
			//Êý×Ö
			prandBuf[i] = rand() % 10 + '0';
			break;
		case 1:
			//Ð¡Ð´×ÖÄ¸
			prandBuf[i] = rand() % 26 + 'A';
			break;
		case 2:
			prandBuf[i] = rand() % 26 + 'a';
			break;
		case 3:
			prandBuf[i] = buf[rand() % strlen(buf)];
			break;
		default:
			break;
		}
	}
}

void ClientOperation::generateHmac(char * Hmac)
{
	char key[] = "123456";
	unsigned int len;
	unsigned int SHA_DIGEST_LENTH = 20;
	char mdBuf[SHA_DIGEST_LENTH * 2 + 1];
	unsigned char data[] = "hello word";
	unsigned char md[SHA_DIGEST_LENTH];
	HMAC_CTX *ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key, strlen(key), EVP_sha1(), NULL);
	HMAC_Update(ctx, data, strlen((char *)data));
	HMAC_Final(ctx, md, &len);

	for (int i = 0; i < SHA_DIGEST_LENTH; i++)
	{
		sprintf(&mdBuf[i * 2], "%02x", md[i]);
	}
	Hmac = mdBuf;
	cout << "ÃÜÔ¿: " << mdBuf << endl;
}
