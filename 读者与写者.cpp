#include<windows.h>
#include<stdio.h>
#include<time.h>
#include<iostream>
using namespace std;

#define MAX_SIZE 5
HANDLE wrt,mutex;					
int count_reader=0;					//读者数目


DWORD WINAPI writer(LPVOID pParam1)			//写者进程
{
	int writer=*(int *)pParam1;
	WaitForSingleObject(wrt,INFINITE);		//P(wrt)
	printf("写者%d开始写文件\n",writer);
	Sleep(rand()%101*50);
	printf("写者%d写文件完成\n",writer);
	ReleaseSemaphore(wrt,1,NULL);			//V(wrt)
	return 0;
}
DWORD WINAPI reader(LPVOID pParam2)			//读者进程
{
	int reader=*(int *)pParam2;
	WaitForSingleObject(mutex,INFINITE);	//P(Mutex)
	count_reader++;
	if (count_reader==1)
	{	
		WaitForSingleObject(wrt,INFINITE);	//P(wrt)
	}
	ReleaseMutex(mutex);					//V(mutex)
	printf("读者%d线程\n",reader);
	Sleep(rand()%101*20);
	printf("读者%d读文件完成\n",reader);
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

	mutex = CreateMutex(NULL,FALSE,NULL);			//生成互斥量
	wrt = CreateSemaphore(NULL,1,MAX_SIZE,NULL);	//生成信号量

	for(int i=0;i<MAX_SIZE;i++)
	{
		index_W[i+1]=i+1;							//写者线程
		Thread_W[i]=CreateThread(NULL,0,writer,&index_W[i+1],0,(DWORD *)&IdW[i]);
	}
	for(int i=0;i<MAX_SIZE;i++)
	{
		index_R[i+1]=i+1;							//读者线程
		Thread_R[i]=CreateThread(NULL,0,reader,&index_R[i+1],0,(DWORD *)&IdR[i]);
	}		
													//等待多线程结束
	WaitForMultipleObjects(MAX_SIZE,Thread_W,TRUE,INFINITE);
	WaitForMultipleObjects(MAX_SIZE,Thread_R,TRUE,INFINITE);

	for(int i=0;i<MAX_SIZE;i++)
	{												//关闭线程
		CloseHandle(Thread_W[i]);
		CloseHandle(Thread_R[i]);
	}
													//关闭线程
	CloseHandle(mutex);
	CloseHandle(wrt);

	system("pause");

	return 0;
}