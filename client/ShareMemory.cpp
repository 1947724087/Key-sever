#include "ShareMemory.h"

ShareMemory::ShareMemory()
{

}

ShareMemory::ShareMemory(int key)
{
	/**/
	int size = 1024;
	int shm_id;
	shm_id=shmget(key,size,0);
	if (shm_id < 0)
	{
		/*调用log类，打印日志*/
		perror("function shmget error");
		return;
	}
	m_shmID = shm_id;
}

ShareMemory::ShareMemory(char * pathName)
{
	key_t key = ftok(pathName, RandX);
	m_shmID = getShmID(key, 0, 0);
	
}

ShareMemory::ShareMemory(int key, int shmSize)
{
	m_shmID = getShmID(key, shmSize, IPC_CREAT | 0755);
	
}

ShareMemory::ShareMemory(char * pathName, int shmSize)
{
	key_t key = ftok(pathName, RandX);
	m_shmID = getShmID(key, shmSize, IPC_CREAT | 0755);
	
}

void * ShareMemory::mapShm()
{
	m_shmAddr = shmat(m_shmID, 0, 0);
	if (m_shmAddr ==(void *)-1)
	{
		//打印日志
		perror("function shmmat() error");
		return NULL;
	}
	return m_shmAddr;
}

int ShareMemory::unmapShm()
{
	int ret;
	ret=shmdt(m_shmAddr);
	if (ret < 0)
	{
		//打印日志,出错
		return -1;
	}
	return 0;
}

int ShareMemory::delshm()
{
	int ret;
	ret=shmctl(m_shmID,IPC_RMID, NULL);
	if (ret < 0)
	{
		//设置出错日志
		printf("删出m_shmID");
		return -1;
	}
	return 0;
}

int ShareMemory::getShmID(key_t key, int shmSize, int flag)
{
	int ret;
	ret = shmget(key, shmSize, flag);
	if (ret < 0)
	{
		/*打印日志*/
		perror("function shmget() error");
		return -1;
	}
	m_shmID = ret;
	return m_shmID;
}
