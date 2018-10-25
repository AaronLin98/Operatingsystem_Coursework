#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define  MAX_SIZE 5							//仓库筐数
#define	 Number   3							//生产者消费者数目

int buffer[MAX_SIZE + 1];					//仓库队列
int front=0;								//队头
int rear=0;									//队尾

int count_p=0;								//生产者数目
int count_s=0;								//消费者数目

HANDLE mutex,empty,full;

DWORD WINAPI producer(LPVOID);
DWORD WINAPI consumer(LPVOID);

typedef struct item
{
	int index;								//编号
}IT;

DWORD WINAPI producer(LPVOID index_p)
{
	int rand_seed;
	IT data = *(IT *)index_p;
	srand((unsigned)time(0));
	while(1)
	{
		Sleep(rand()%101*10);
		WaitForSingleObject(empty,INFINITE);		//P(empty);
		WaitForSingleObject(mutex,INFINITE);		//P(mutex);
		rand_seed = rand();							//生产者生产的产品
		count_p+=rand_seed;
		printf("生产者	%d	生产了	%d\n",data.index,rand_seed);
		if ((rear+1)%(MAX_SIZE+1)==front)			//仓库已满
		{
			printf("仓库已满\n");
		}
		else										//仓库未满
		{
			buffer[rear]=rand_seed;					//往buffer仓库中添产品
			rear=(rear+1)%(MAX_SIZE+1);				//队列指针后移
		}
		ReleaseMutex(mutex);						//V(mutex)
		ReleaseSemaphore(full,1,NULL);				//V(full)
		if(count_p>1000000)
		{	
			break;
		}
	}
	return 0;
}

DWORD WINAPI consumer(LPVOID index_s)
{
	int rand_seed;
	IT data = *(IT *)index_s;
	srand((unsigned)time(0));
	while(1)
	{
		Sleep(rand()%101*10);
		WaitForSingleObject(full,INFINITE);			//P(full)
		WaitForSingleObject(mutex,INFINITE);		//P(mutex)
		if (front==rear)							//消费者消费产品过程
		{
			printf("仓库为空 \n");
		}
		else
		{
			rand_seed=buffer[front];
			count_s+=rand_seed;
			front=(front+1)%(MAX_SIZE+1);	
			printf("消费者	%d	消费了	%d\n",data.index,rand_seed);
		}
		ReleaseMutex(mutex);						//V(mutex)
		ReleaseSemaphore(empty,1,NULL);				//V(empty)
		if(count_s>=1000000)
		{
			break;
		}
	}
	return 0;
}
int main()
{
	int *ThreadIdP,*ThreadIdS;

	IT  *index_p,*index_s;

	HANDLE *ThreadHandleP,*ThreadHandleS;

	ThreadHandleP=(HANDLE *)malloc(Number *sizeof(HANDLE));		//创建三个生产者
	ThreadHandleS=(HANDLE *)malloc(Number *sizeof(HANDLE));		//创建三个消费者

	ThreadIdP=(int *)malloc(Number *sizeof(int));
	ThreadIdS=(int *)malloc(Number *sizeof(int));


	mutex = CreateMutex(NULL,FALSE,NULL);
	empty = CreateSemaphore(NULL,MAX_SIZE,MAX_SIZE,NULL); 
	full = CreateSemaphore(NULL,MAX_SIZE,MAX_SIZE,NULL);
        
	index_p = (IT *)malloc((Number + 1) * sizeof(IT));			//生产者编号
	index_s = (IT *)malloc((Number + 1) * sizeof(IT));			//消费者编号

	for(int i=0;i<Number;i++)
	{
		index_p[i+1].index=i+1;										//创建生产者进程
		ThreadHandleP[i]=CreateThread(NULL,0,producer,&index_p[i+1],0,(DWORD *)&ThreadIdP[i]);		
	}
	for(int i=0;i<Number;i++)
	{
		index_s[i+1].index=i+1;										//创建消费者进程
		ThreadHandleS[i]=CreateThread(NULL,0,consumer,&index_s[i+1],0,(DWORD *)&ThreadIdS[i]);
	}

																	//等待多线程结束
	WaitForMultipleObjects(Number, ThreadHandleP, true, INFINITE);
	WaitForMultipleObjects(Number, ThreadHandleS, true, INFINITE);

	for(int i=0;i<Number;i++)										//关闭线程
	{
		CloseHandle(ThreadHandleP[i]);
		CloseHandle(ThreadHandleS[i]);
	}
																	//关闭线程
	CloseHandle(mutex);
	CloseHandle(empty);
	CloseHandle(full);

	system("pause");

	return 0;
}