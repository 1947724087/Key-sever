#pragma once
#include <sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

const char RandX = 'x';

/*�����ڴ������*/
class ShareMemory
{
public:
	/*���캯��*/
	ShareMemory();
	ShareMemory(int key);
	ShareMemory(char *pathName);
	ShareMemory(int key, int shmSize);
	ShareMemory(char* pathName, int shmSize);

	void* mapShm();
	int unmapShm();
	int delshm();
private:
	int getShmID(key_t key, int shmSize, int flag);
	int m_shmID;
	void *m_shmAddr=NULL;
};

