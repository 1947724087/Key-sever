#include<iostream>
#include<string>
#include<stdlib.h>
#include<string.h>
#include<time.h>

using namespace std;

void  RandomString(char* prandBuf, int len)
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