#include "ServerOperation.h"
#include <iostream>
#include <pthread.h>
#include <string.h>
#include "RequestFactory.h"
#include "RespondFactory.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <signal.h>
#include<time.h>
using namespace std;

bool ServerOperation::m_stop = false;	// 静态变量初始化

ServerOperation::ServerOperation(ServerInfo * info)
{
	memcpy(&m_info, info, sizeof(ServerInfo));
	m_shm = new SecKeyShm(m_info.shmkey, m_info.maxnode);

	 MYSQL *mysql = mysql_init(NULL);
	 if(mysql==NULL)
	 {
	 	printf("mysql_init error!\n");
		return ;
	 }

	m_sqlConn = mysql_real_connect(mysql, info->dbHoust, info->dbUesr, info->dbPasswd, "keyBase", info->dbPort, NULL, 0);
	if(m_sqlConn==NULL)
	{
		printf("m_sql_conn error!\n");
		return ;
	}
}

ServerOperation::~ServerOperation()
{

}

void ServerOperation::startWork()
{
	//socket-setsockopt-bind-listen,服务器监听
	m_server.setListen(m_info.sPort);
	pthread_t threadID;
	while (1)
	{
		//accept新的客户端连接
		double time=360*24*60*60;
		m_client = m_server.acceptConn(time);

		//创建一个子线程
		pthread_create(&threadID, NULL, working, this);
		//设置子线程为分离属性
		pthread_detach(threadID);
		m_listSocket.insert(make_pair(threadID, m_client));
	}
}

int ServerOperation::secKeyAgree(RequestMsg * reqMsg, char ** outData, int & outLen)
{
	//验证消息认证
	printf("开始验证\n");
	char key[64];
	unsigned int len;
	unsigned char md[SHA_DIGEST_LENGTH];
	char authCode[SHA_DIGEST_LENGTH * 2 + 1] = {0};

	memset(key, 0x00, sizeof(key));
	sprintf(key, "%s%s", reqMsg->clientId,reqMsg->serverId);
	HMAC(EVP_sha1(), key, strlen(key), (unsigned char *)reqMsg->r1, strlen(reqMsg->r1), md, &len);
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&authCode[2 * i], "%02x", md[i]);
	}
	//将生成的消息认证码和客户端的r1的消息认证做比对
	if (strcmp(authCode, reqMsg->authCode) != 0)
	{
		cout << "消息认证码错误" << endl;
		return -1;
	}

	printf("验证结束\n");
	//生成随机字符串r2
	RespondMsg rspMsg;
	memset(&rspMsg, 0x00, sizeof(rspMsg));
	getRandString(sizeof(rspMsg.r2), rspMsg.r2);

	//将随机字符串r2和r1进行拼接, 然后生成秘钥 r1+r2
	char buf[128];
        unsigned char md1[SHA_DIGEST_LENGTH];
	char seckey[SHA_DIGEST_LENGTH * 2 + 1];
	memset(buf, 0x00, sizeof(buf));
	memset(seckey, 0x00, sizeof(seckey));
	sprintf(buf, "%s%s", reqMsg->r1, rspMsg.r2);
	SHA1((unsigned char *)buf, strlen((char *)buf), md1);
	for (int i = 0; i<SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&seckey[i * 2], "%02x", md1[i]);
	}
	printf("服务端生成的密钥：%s\n",seckey);
	//获取当前时间
	
	time_t timep;
        struct tm *p;
        time (&timep);
        p=gmtime(&timep);
	  //  printf("%d-",1900+p->tm_year);      /*获取当前年份,从1900开始，所以要加1900*/
	   // printf("%d-",1+p->tm_mon);          /*获取当前月份,范围是0-11,所以要加1*/
	  //  printf("%d ",p->tm_mday);           /*获取当前月份日数,范围是1-31*/
	   // printf("%d:",8+p->tm_hour);         /*获取当前时,这里获取西方的时间,刚好相差八个小时*/
	   // printf("%d:",p->tm_min);            /*获取当前分*/
	   // printf("%d\n",p->tm_sec);           /*获取当前秒*
	
	char date[255]={0};
	char Time[255]={0};
	sprintf(date,"%d-%d-%d",1900+p->tm_year,1+p->tm_mon,p->tm_mday);
	sprintf(Time,"%d:%d:%d",8+p->tm_hour,p->tm_min,p->tm_sec);
	//将数据加入MYSQL数据库
	char sql[255]={0};
	sprintf(sql,"INSERT INTO  UserKey VALUES (%d,%s,'%s','%s','%s','%s')",1,reqMsg->clientId,"127.0.0.1",seckey,date, Time);
	printf(sql);
	int ret=mysql_query(m_sqlConn,sql);
	if(ret!=0)
		{
			printf("mysql query error\n");
		}

	//给应答结构体赋值
	rspMsg.seckeyid = 1;  //获取秘钥ID
	rspMsg.rv = 0;
	strcpy(rspMsg.serverId, m_info.serverID);
	strcpy(rspMsg.clientId, reqMsg->clientId);

	//将要发送给客户端的应答结构体进行编码
	int dataLen;
	char *sendData = NULL;
	CodecFactory *factory = new RespondFactory(&rspMsg);
	Codec *pCodec = factory->createCodec();
	pCodec->msgEncode(&sendData, dataLen);
	delete factory;
	//delete pCodec;

	printf("编码成功\n");
	//发送数据给客户端
	pthread_t thread = pthread_self();
	TcpSocket* socket = m_listSocket[thread];
	socket->sendMsg(sendData, dataLen);
	free(sendData);

	//写秘钥信息到共享内存
	NodeSHMInfo node;
	memset(&node, 0x00, sizeof(NodeSHMInfo));
	node.status = 0;
	strcpy(node.seckey, seckey);
	strcpy(node.clientID, rspMsg.clientId);
	strcpy(node.serverID, m_info.serverID);
	node.seckeyID = rspMsg.seckeyid;

	//将秘钥信息写入共享内存
	m_shm->shmWrite(&node);

	//关闭连接
	socket->disConnect();

	return 0;
}



void ServerOperation::getRandString(int len, char * randBuf)
{
	int flag = -1;
	// 设置随机种子
	srand(time(NULL));
	// 随机字符串: A-Z, a-z, 0-9, 特殊字符(!@#$%^&*()_+=)
	char chars[] = "!@#$%^&*()_+=";
	for (int i = 0; i < len - 1; ++i)
	{
		flag = rand() % 4;
		switch (flag)
		{
		case 0:
			randBuf[i] = 'Z' - rand() % 26;
			break;
		case 1:
			randBuf[i] = 'z' - rand() % 26;
			break;
		case 3:
			randBuf[i] = rand() % 10 + '0';
			break;
		case 2:
			randBuf[i] = chars[rand() % strlen(chars)];
			break;
		default:
			break;
		}
	}
	randBuf[len - 1] = '\0';
}

// 友元函数, 可以在该友元函数中通过对应的类对象调用期私有成员函数或者私有变量
// 子线程 - 进行业务流程处理
void * working(void * arg)
{
	//接收数据，拿到自己的线程id
	pthread_t thread = pthread_self();
	//线程参数
	ServerOperation *op = (ServerOperation *)arg;
	//取出自己的任务
	TcpSocket* socket = op->m_listSocket[thread];

	char *inData;
	int dataLen=4;
	//接收任务类型
	socket->recvMsg(&inData, dataLen,20);

	//解码
	CodecFactory *factory = new RequestFactory();
	Codec *pCodec = factory->createCodec();
	RequestMsg *pMsg = (RequestMsg *)pCodec->msgDecode(inData, dataLen);
	delete factory;
	//delete pCodec;

	//判断clientID是否合法

	//判断客户端要请求什么服务
	char *outData;
	//执行相对类型的任务
	switch(pMsg->cmdType)
	{
	case RequestCodec::NewOrUpdate:

		op->secKeyAgree(pMsg, &outData, dataLen);
		break;

	case RequestCodec::Check:
		op->secKeyCheck();
		break;
	case RequestCodec::Revoke:
		op->secKeyRevoke();
		break;
	case RequestCodec::View:
		op->secKeyView();
		break;

	default:
		break;
	}
}
