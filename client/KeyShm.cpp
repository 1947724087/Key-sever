#include "KeyShm.h"

KeyShm::KeyShm(int key): ShareMemory(key)
{

}

KeyShm::KeyShm(int key, int Nodesize): ShareMemory(key, sizeof(int) + Nodesize * sizeof(NodeShmInfo))
{
	
	m_maxNode = Nodesize;
	//���ӹ����ڴ�
	void *p = mapShm();
	memset(p, 0x00, sizeof(int));
	//��maxnode д�빲���ڴ��ǰ�ĸ��ֽ�
	memcpy(p, &m_maxNode, sizeof(m_maxNode));
	//�Ͽ�����
	unmapShm();
}

KeyShm::KeyShm(char * pathName):ShareMemory(pathName)
{
	
}

KeyShm::KeyShm(char * pathName, int Nodesize):ShareMemory(pathName, sizeof(int) + Nodesize * sizeof(NodeShmInfo))
{
	m_maxNode = Nodesize;

	//���ӹ����ڴ�
	void *p = mapShm();
	memset(p, 0x00, sizeof(int));
	//��maxnode д�빲���ڴ��ǰ�ĸ��ֽ�
	memcpy(p, &m_maxNode, sizeof(m_maxNode));
	//�Ͽ�����
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
	//���ӹ����ڴ�
	void *p = mapShm();
	//д�����ڴ�
	memcpy(&m_maxNode,p,sizeof(int));
	//�������ڴ�ת��Ϊ NodeShmInfo����,һ�������ڴ棬��Ŷ���ṹ����Ϣ
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

	//Ҫ��û���ҵ�
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
	//�Ͽ�����
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
