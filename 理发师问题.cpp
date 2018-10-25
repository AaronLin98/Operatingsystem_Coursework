#include<iostream>
#include<cmath>
#include<time.h>
#include<cstring>
#include<string.h>
#include<windows.h>
#include<cstdlib>
using namespace std;

#define CHAIRS 5			//椅子数目
#define MAX_SIZE 10			//最多理发顾客

HANDLE babers;				//信号量barbers
HANDLE customers;			//信号量customers
HANDLE mutex;				//信号量mutex

int waiting =0;				//等待人数
int index =0;				//顾客编号
int count =0;				//总人数


 DWORD WINAPI customer(LPVOID lpParam){
	WaitForSingleObject(mutex,INFINITE);				//P(mutex)
	if(waiting <CHAIRS)
	{
		count ++;
		waiting ++;
		printf("\n第 %d 名顾客进入理发店\n",count);
		ReleaseSemaphore(customers,1,NULL);				//V(customers)
		ReleaseMutex(mutex);							//V(mutex)
		WaitForSingleObject(babers,INFINITE);			//P(babers)
	}
	else
	{
		printf("\n座位已满，新来顾客离开\n",count);
		ReleaseMutex(mutex);							//P(mutex)
	}
	return 0;
 }

 void Cut(){											//理发过程
	srand((unsigned)time(0));
	Sleep(rand()%101*80);
	index ++;
	printf("\n第 %d 名顾客理完发\n",index);
	if(index==MAX_SIZE)
	{
		printf("理发结束~~~~\n");
		Sleep(rand()%101*80);
		exit(0);
	}
}

 DWORD WINAPI baber(LPVOID lpParam){
	 while(true)
	 {
		WaitForSingleObject(customers,INFINITE);		//P(customers)
		WaitForSingleObject(mutex,INFINITE);			//P(mutex)
		waiting --;
		ReleaseSemaphore(babers,1,NULL);				//V(babers)
		ReleaseMutex(mutex);							//V(mutex)
		Cut();
	 }
	 return 0;
 }

 int main () {
	 HANDLE Thread_baber,Thread_customer[MAX_SIZE];
	 wstring s1=L"babers",s2=L"customers",s3=L"mutex";
															//创造信号量互斥量
	 customers=CreateSemaphore(NULL,0,MAX_SIZE,s2.c_str());
	 babers=CreateSemaphore(NULL,0,1,s1.c_str());
	 mutex=CreateMutex(NULL,FALSE,s3.c_str());
															//创造理发师线程
	 Thread_baber = CreateThread(NULL,0,baber,NULL,0,NULL);
															//创造顾客线程
	 for(int i=0;i<MAX_SIZE;i++)
	 {
		 srand((unsigned)time(0));
		 Thread_customer[i]=CreateThread(NULL,0,customer,NULL,0,NULL);
		 Sleep(rand()%101*50);
	 }
															//等待多线程
	 WaitForMultipleObjects(MAX_SIZE,Thread_customer,TRUE,INFINITE);
	 WaitForSingleObject(Thread_baber,INFINITE);

	 system("pause");
	 return 0;
 }
