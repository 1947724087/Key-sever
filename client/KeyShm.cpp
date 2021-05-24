#include "KeyShm.h"

KeyShm::KeyShm(int key): ShareMemory(key)
{

}

KeyShm::KeyShm(int key, int Nodesize): ShareMemory(key, sizeof(int) + Nodesize * sizeof(NodeShmInfo))
{
	
	m_maxNode = Nodesize;
	//连接共享内存
	void *p = mapShm();
	memset(p, 0x00, sizeof(int));
	//将maxnode 写入共享内存的前四个字节
	memcpy(p, &m_maxNode, sizeof(m_maxNode));
	//断开连接
	unmapShm();
}

KeyShm::KeyShm(char * pathName):ShareMemory(pathName)
{
	
}

KeyShm::KeyShm(char * pathName, int Nodesize):ShareMemory(pathName, sizeof(int) + Nodesize * sizeof(NodeShmInfo))
{
	m_maxNode = Nodesize;

	//连接共享内存
	void *p = mapShm();
	memset(p, 0x00, sizeof(int));
	//将maxnode 写入共享内存的前四个字节
	memcpy(p, &m_maxNode, sizeof(m_maxNode));
	//断开连接
	unmapShm();
}

int KeyShm::KeyShmRD(const char* clientID, const char* serverID, NodeShmInfo* info)
{
	void *p = mapShm();
	//memcpy(&m_maxNode, p, sizeof(int));
	printf("maxnode==[%d]", m_maxNode);
	NodeShmInfo *pNode = (NodeShmInfo *)((char *)p + sizeof(int));
	int i = 0;
	for (i = 0; i < m_maxNode; i++)
	{
		if (strcmp(clientID, pNode[i].clientID) == 0 &&
			strcmp(serverID, pNode[i].severID) == 0)
		{
			memcpy(info, &pNode[i], sizeof(NodeShmInfo));
			break;
		}
	}
	unmapShm();
	if (i == m_maxNode)
	{
		cout << "not found" << endl;
		return -1;
	}
	return 0;
}

int KeyShm::KeyShmWR(NodeShmInfo * info)
{
	//连接共享内存
	void *p = mapShm();
	//写共享内存
	memcpy(&m_maxNode,p,sizeof(int));
	//将共享内存转换为 NodeShmInfo类型,一个共享内存，存放多个结构体信息
	NodeShmInfo *pNode=(NodeShmInfo *)((char *)p+sizeof(int));

	int i = 0;
	for (i = 0; i < m_maxNode; i++)
	{
		if (strcmp(info->clientID, pNode[i].clientID) == 0 &&
			strcmp(info->severID, pNode[i].severID) == 0)
		{
			memcpy(&pNode[i], info, sizeof(NodeShmInfo));
			break;
		}
	}

	//要是没有找到
	NodeShmInfo tmp;
	memset(&tmp, 0x00, sizeof(tmp));
	if (i == m_maxNode)
	{
		for (i = 0; i < m_maxNode; i++)
		{
			cout<<i<<endl;
			if (memcmp(&pNode[i], &tmp, sizeof(NodeShmInfo)) == 0)
			{
				memcpy(&pNode[i],info,sizeof(NodeShmInfo));
				break;
			}
		}
	}
	//断开连接
	unmapShm();
	if (i == m_maxNode)
	{
		printf("no space to use\n");
		return -1;
	}
	return 0;
}

void KeyShm::printShm()
{
	void *p = mapShm();
	memcpy(&m_maxNode, p, sizeof(int));
	cout << "maxNode:[" << m_maxNode << "]" << endl;
	NodeShmInfo *pNode = (NodeShmInfo *)((char *)p + sizeof(int));

	int i = 0;
	for (i = 0; i < m_maxNode; i++)
	{
		cout << "----------" << i << "----------" << endl;
		cout << "status: " << pNode[i].status << endl;
		cout << "secKeyID: " << pNode[i].seckeyID << endl;
		cout << "clientID: " << pNode[i].clientID << endl;
		cout << "severID: " << pNode[i].severID << endl;
		cout << "seckey: " << pNode[i].seckey << endl;

	}
	return;
}
