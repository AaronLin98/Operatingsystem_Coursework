#include<iostream>
#include<cmath>
#include<time.h>
#include<cstring>
#include<string.h>
#include<windows.h>
#include<cstdlib>
using namespace std;

#define CHAIRS 5			//������Ŀ
#define MAX_SIZE 10			//������˿�

HANDLE babers;				//�ź���barbers
HANDLE customers;			//�ź���customers
HANDLE mutex;				//�ź���mutex

int waiting =0;				//�ȴ�����
int index =0;				//�˿ͱ��
int count =0;				//������


 DWORD WINAPI customer(LPVOID lpParam){
	WaitForSingleObject(mutex,INFINITE);				//P(mutex)
	if(waiting <CHAIRS)
	{
		count ++;
		waiting ++;
		printf("\n�� %d ���˿ͽ�������\n",count);
		ReleaseSemaphore(customers,1,NULL);				//V(customers)
		ReleaseMutex(mutex);							//V(mutex)
		WaitForSingleObject(babers,INFINITE);			//P(babers)
	}
	else
	{
		printf("\n��λ�����������˿��뿪\n",count);
		ReleaseMutex(mutex);							//P(mutex)
	}
	return 0;
 }

 void Cut(){											//������
	srand((unsigned)time(0));
	Sleep(rand()%101*80);
	index ++;
	printf("\n�� %d ���˿����귢\n",index);
	if(index==MAX_SIZE)
	{
		printf("������~~~~\n");
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
															//�����ź���������
	 customers=CreateSemaphore(NULL,0,MAX_SIZE,s2.c_str());
	 babers=CreateSemaphore(NULL,0,1,s1.c_str());
	 mutex=CreateMutex(NULL,FALSE,s3.c_str());
															//������ʦ�߳�
	 Thread_baber = CreateThread(NULL,0,baber,NULL,0,NULL);
															//����˿��߳�
	 for(int i=0;i<MAX_SIZE;i++)
	 {
		 srand((unsigned)time(0));
		 Thread_customer[i]=CreateThread(NULL,0,customer,NULL,0,NULL);
		 Sleep(rand()%101*50);
	 }
															//�ȴ����߳�
	 WaitForMultipleObjects(MAX_SIZE,Thread_customer,TRUE,INFINITE);
	 WaitForSingleObject(Thread_baber,INFINITE);

	 system("pause");
	 return 0;
 }
