#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define  MAX_SIZE 5							//�ֿ����
#define	 Number   3							//��������������Ŀ

int buffer[MAX_SIZE + 1];					//�ֿ����
int front=0;								//��ͷ
int rear=0;									//��β

int count_p=0;								//��������Ŀ
int count_s=0;								//��������Ŀ

HANDLE mutex,empty,full;

DWORD WINAPI producer(LPVOID);
DWORD WINAPI consumer(LPVOID);

typedef struct item
{
	int index;								//���
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
		rand_seed = rand();							//�����������Ĳ�Ʒ
		count_p+=rand_seed;
		printf("������	%d	������	%d\n",data.index,rand_seed);
		if ((rear+1)%(MAX_SIZE+1)==front)			//�ֿ�����
		{
			printf("�ֿ�����\n");
		}
		else										//�ֿ�δ��
		{
			buffer[rear]=rand_seed;					//��buffer�ֿ������Ʒ
			rear=(rear+1)%(MAX_SIZE+1);				//����ָ�����
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
		if (front==rear)							//���������Ѳ�Ʒ����
		{
			printf("�ֿ�Ϊ�� \n");
		}
		else
		{
			rand_seed=buffer[front];
			count_s+=rand_seed;
			front=(front+1)%(MAX_SIZE+1);	
			printf("������	%d	������	%d\n",data.index,rand_seed);
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

	ThreadHandleP=(HANDLE *)malloc(Number *sizeof(HANDLE));		//��������������
	ThreadHandleS=(HANDLE *)malloc(Number *sizeof(HANDLE));		//��������������

	ThreadIdP=(int *)malloc(Number *sizeof(int));
	ThreadIdS=(int *)malloc(Number *sizeof(int));


	mutex = CreateMutex(NULL,FALSE,NULL);
	empty = CreateSemaphore(NULL,MAX_SIZE,MAX_SIZE,NULL); 
	full = CreateSemaphore(NULL,MAX_SIZE,MAX_SIZE,NULL);
        
	index_p = (IT *)malloc((Number + 1) * sizeof(IT));			//�����߱��
	index_s = (IT *)malloc((Number + 1) * sizeof(IT));			//�����߱��

	for(int i=0;i<Number;i++)
	{
		index_p[i+1].index=i+1;										//���������߽���
		ThreadHandleP[i]=CreateThread(NULL,0,producer,&index_p[i+1],0,(DWORD *)&ThreadIdP[i]);		
	}
	for(int i=0;i<Number;i++)
	{
		index_s[i+1].index=i+1;										//���������߽���
		ThreadHandleS[i]=CreateThread(NULL,0,consumer,&index_s[i+1],0,(DWORD *)&ThreadIdS[i]);
	}

																	//�ȴ����߳̽���
	WaitForMultipleObjects(Number, ThreadHandleP, true, INFINITE);
	WaitForMultipleObjects(Number, ThreadHandleS, true, INFINITE);

	for(int i=0;i<Number;i++)										//�ر��߳�
	{
		CloseHandle(ThreadHandleP[i]);
		CloseHandle(ThreadHandleS[i]);
	}
																	//�ر��߳�
	CloseHandle(mutex);
	CloseHandle(empty);
	CloseHandle(full);

	system("pause");

	return 0;
}