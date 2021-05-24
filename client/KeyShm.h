#pragma once
#include "ShareMemory.h"
#include<iostream>
#include<string>

using namespace std;

struct NodeShmInfo
{
public:
	int status;
	char clientID[12];
	char severID[12];
	int seckeyID;
	char seckey[128];
};

class KeyShm :public ShareMemory
{
public:
	KeyShm(int key);
	KeyShm(int key, int Nodesize);
	KeyShm(char* pathName);
	KeyShm(char* pathName, int Nodesize);

	int KeyShmRD(const char* clientID, const char* serverID, NodeShmInfo* info);
	int KeyShmWR(NodeShmInfo* info);
	void printShm();

private:
	int m_maxNode;
};

