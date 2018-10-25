#include <windows.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <iostream>
#include <time.h>
#include <ctime>

#define SIZE 5
int philNumber=0;
HANDLE mutex,chopstick[5];

DWORD WINAPI phiol(LPVOID lparameter)
{
	srand((unsigned)time(0));
	int Number = philNumber;
	philNumber++;
	printf("��ѧ��	%d ˼��\n",Number);										//��ѧ��˼��
	Sleep(rand() % 101 * 20);	
	printf("��ѧ��	%d ����\n",Number);										//��ѧ�Ҽ���
	Sleep(rand() % 101 * 1);
	WaitForSingleObject(mutex,INFINITE);									//P(mutex)
	WaitForSingleObject(chopstick[philNumber],INFINITE);					//ȡ��߿���
	printf("��ѧ��	%d ȡ���� %d\n",Number,Number);
	Sleep(rand() % 101 * 10);
	WaitForSingleObject(chopstick[(philNumber + 1) % 5],INFINITE);			//ȡ�ұ߿���
	printf("��ѧ��	%d ȡ���� %d\n",Number,Number+1);
	Sleep(rand() % 101 * 10);
	ReleaseSemaphore(mutex,1,NULL);											//V(mutex)
	printf("��ѧ��	%d �ò�\n",Number);										//��ѧ�ҽ���			
	Sleep(rand() % 101 * 50);
	printf("��ѧ��	%d �Żؿ��� %d �Ϳ��� %d\n",Number,Number,Number+1);
	ReleaseSemaphore(chopstick[(philNumber +1) % 5],1,NULL);				//�Ż��ұ߿���
	ReleaseSemaphore(chopstick[philNumber],1,NULL);							//�Ż���߿���
	return 0;
}

int main()
{
	HANDLE philosopheri[5];

	mutex = CreateMutex(NULL, FALSE, TEXT("mutex"));		//���ɻ�����

	for (int i=0;i<SIZE;i++)
	{
		chopstick[i]= CreateSemaphore(NULL, 1, 1, NULL);	//����chopstick�ź���
	}

	for (int i = 0; i < SIZE; i++)
	{														//����philosopheri�ź���
		philosopheri[i]=CreateThread(NULL, 0, phiol, NULL, 0, NULL);
	}														
															//�ȴ����߳̽���
	WaitForMultipleObjects(SIZE, philosopheri, true, INFINITE);

	system("pause");
	return 0;
}