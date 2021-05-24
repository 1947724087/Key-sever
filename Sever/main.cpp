#include <cstdio>
#include "ServerOperation.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
using namespace std;

//守护进程
void defenProcess()
{
	pid_t pid=fork();
	if(pid<0||pid>0)
	{
		exit(0);
	}
	
	//创建会话
	setsid();
	//改变工作目录
	chdir("/root/projects/keySever");
	//修改文件掩码
	umask(0000);
	//关闭重定向文件输出
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	int fd=open("/dev/null",O_RDWR);
	dup2(fd,STDIN_FILENO);
	dup2(fd,STDOUT_FILENO);
	dup2(fd,STDERR_FILENO);
}
int main()
{

	defenProcess();
	printf("子进程开始\n");
	ServerInfo info;
	strcpy(info.serverID, "0001");
	info.maxnode = 10;
	info.shmkey = 0x12340000;
	strcpy(info.dbUesr, "admin");
	strcpy(info.dbPasswd, "123456789qq");
	strcpy(info.dbHoust,"8.140.7.140");
	info.dbPort=3306;
	strcpy(info.dbSID, "");
	info.sPort = 9090;

	ServerOperation server(&info);
	server.startWork();

    return 0;
}
