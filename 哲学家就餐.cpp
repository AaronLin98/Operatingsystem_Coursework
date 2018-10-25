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
	printf("哲学家	%d 思考\n",Number);										//哲学家思考
	Sleep(rand() % 101 * 20);	
	printf("哲学家	%d 饥饿\n",Number);										//哲学家饥饿
	Sleep(rand() % 101 * 1);
	WaitForSingleObject(mutex,INFINITE);									//P(mutex)
	WaitForSingleObject(chopstick[philNumber],INFINITE);					//取左边筷子
	printf("哲学家	%d 取筷子 %d\n",Number,Number);
	Sleep(rand() % 101 * 10);
	WaitForSingleObject(chopstick[(philNumber + 1) % 5],INFINITE);			//取右边筷子
	printf("哲学家	%d 取筷子 %d\n",Number,Number+1);
	Sleep(rand() % 101 * 10);
	ReleaseSemaphore(mutex,1,NULL);											//V(mutex)
	printf("哲学家	%d 用餐\n",Number);										//哲学家进餐			
	Sleep(rand() % 101 * 50);
	printf("哲学家	%d 放回筷子 %d 和筷子 %d\n",Number,Number,Number+1);
	ReleaseSemaphore(chopstick[(philNumber +1) % 5],1,NULL);				//放回右边筷子
	ReleaseSemaphore(chopstick[philNumber],1,NULL);							//放回左边筷子
	return 0;
}

int main()
{
	HANDLE philosopheri[5];

	mutex = CreateMutex(NULL, FALSE, TEXT("mutex"));		//生成互斥量

	for (int i=0;i<SIZE;i++)
	{
		chopstick[i]= CreateSemaphore(NULL, 1, 1, NULL);	//生成chopstick信号量
	}

	for (int i = 0; i < SIZE; i++)
	{														//生成philosopheri信号量
		philosopheri[i]=CreateThread(NULL, 0, phiol, NULL, 0, NULL);
	}														
															//等待多线程结束
	WaitForMultipleObjects(SIZE, philosopheri, true, INFINITE);

	system("pause");
	return 0;
}