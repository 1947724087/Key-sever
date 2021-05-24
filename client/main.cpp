#include <iostream>
#include"ClientOperation.h"
#include"TcpSocket.h"
#include"KeyShm.h"
#include<string.h>
#include"RequestCodec.h"

using namespace std;
int usage();

int main()
{
	ClientInfo info;
	strcpy(info.clientID, "1111");
	strcpy(info.severID, "0001");
	strcpy(info.severIP, "8.140.7.140");
	info.severPort = 9090;
	info.maxNode = 10;
	info.shmKey = 0x123456;
	ClientOperation op(&info);
	int select;
	while (1)
	{
		int nSl=usage();
		switch (nSl)
		{
		case RequestCodec::NewOrUpdate:
			op.keyAgree();
		case RequestCodec::Check:
			op.keyCheck();
		case RequestCodec::Revoke:
			op.keyRevock();
		case RequestCodec::View:
			op.keyView();
		case 0:
			exit(0);
		default:
			break;
		}
	}
}


int usage()
{
	cout << "------------------密钥协商客户端-------------------- " << endl;
	cout << "------------------  1.密钥协商  -------------------- " << endl;
	cout << "------------------  2.密钥核验  -------------------- " << endl;
	cout << "------------------  3.密钥销毁  -------------------- " << endl;
	cout << "------------------  4.密钥查看  -------------------- " << endl;
	cout << "------------------  0.退   出   -------------------- " << endl;
	int select;
	cout<<"请选择： ";
	cin>>select;
	cout<<endl;
	return select;

}
