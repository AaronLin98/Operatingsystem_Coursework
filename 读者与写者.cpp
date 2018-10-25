#include<windows.h>
#include<stdio.h>
#include<time.h>
#include<iostream>
using namespace std;

#define MAX_SIZE 5
HANDLE wrt,mutex;					
int count_reader=0;					//������Ŀ


DWORD WINAPI writer(LPVOID pParam1)			//д�߽���
{
	int writer=*(int *)pParam1;
	WaitForSingleObject(wrt,INFINITE);		//P(wrt)
	printf("д��%d��ʼд�ļ�\n",writer);
	Sleep(rand()%101*50);
	printf("д��%dд�ļ����\n",writer);
	ReleaseSemaphore(wrt,1,NULL);			//V(wrt)
	return 0;
}
DWORD WINAPI reader(LPVOID pParam2)			//���߽���
{
	int reader=*(int *)pParam2;
	WaitForSingleObject(mutex,INFINITE);	//P(Mutex)
	count_reader++;
	if (count_reader==1)
	{	
		WaitForSingleObject(wrt,INFINITE);	//P(wrt)
	}
	ReleaseMutex(mutex);					//V(mutex)
	printf("����%d�߳�\n",reader);
	Sleep(rand()%101*20);
	printf("����%d���ļ����\n",reader);
	WaitForSingleObject(mutex,INFINITE);	//P(mutex)
	count_reader--;
	if (count_reader==0)
	{
		ReleaseSemaphore(wrt,1,NULL);		//V(wrt)
	}
	ReleaseMutex(mutex);					//V(mutex)
	return 0;	
}

int main()
{
	int *IdW,*IdR;
	int index_W[MAX_SIZE+1],index_R[MAX_SIZE+1];
	HANDLE Thread_W[MAX_SIZE],Thread_R[MAX_SIZE];

	IdR = (int *)malloc(MAX_SIZE * sizeof(int));
	IdW = (int *)malloc(MAX_SIZE * sizeof(int));

	mutex = CreateMutex(NULL,FALSE,NULL);			//���ɻ�����
	wrt = CreateSemaphore(NULL,1,MAX_SIZE,NULL);	//�����ź���

	for(int i=0;i<MAX_SIZE;i++)
	{
		index_W[i+1]=i+1;							//д���߳�
		Thread_W[i]=CreateThread(NULL,0,writer,&index_W[i+1],0,(DWORD *)&IdW[i]);
	}
	for(int i=0;i<MAX_SIZE;i++)
	{
		index_R[i+1]=i+1;							//�����߳�
		Thread_R[i]=CreateThread(NULL,0,reader,&index_R[i+1],0,(DWORD *)&IdR[i]);
	}		
													//�ȴ����߳̽���
	WaitForMultipleObjects(MAX_SIZE,Thread_W,TRUE,INFINITE);
	WaitForMultipleObjects(MAX_SIZE,Thread_R,TRUE,INFINITE);

	for(int i=0;i<MAX_SIZE;i++)
	{												//�ر��߳�
		CloseHandle(Thread_W[i]);
		CloseHandle(Thread_R[i]);
	}
													//�ر��߳�
	CloseHandle(mutex);
	CloseHandle(wrt);

	system("pause");

	return 0;
}