#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<string>
#include<ctime>
#include<fstream>
#include<sstream>
#include<iomanip>
#include"head.h"
#include"inode.h"
#include"directory.h"
#include"command.h"
#include"datablock.h"
#include"superblock.h"
using namespace std;

HANDLE mutex;										//一个互斥量
HANDLE ghSemaphore1, ghSemaphore2, ghSemaphore3;	//三个信号量

char Input[2*BLKSIZE];		//输入内容
char command[20];			//命令
char config[BLKSIZE+30];	//参数


DWORD WINAPI read(LPVOID lpParam)
{
	while (1)
	{	
		WaitForSingleObject( ghSemaphore1,INFINITE);		//申请资源1
		WaitForSingleObject(mutex,INFINITE);
		PrintCurPath();
		cout<<">";
		cin.getline(Input,2*BLKSIZE,'\n');					//接受命令
		ReleaseMutex(mutex);
		ReleaseSemaphore(ghSemaphore2,1,NULL);				//释放资源2
	}
	return 0;
}

DWORD WINAPI analize(LPVOID lpParam)
{
	while (1)
	{
		WaitForSingleObject( ghSemaphore2,INFINITE);		//申请资源2
		WaitForSingleObject(mutex,INFINITE);
		SplitCmd(Input,command,config);						//命令解析
		ReleaseMutex(mutex);
		ReleaseSemaphore(ghSemaphore3,1,NULL);				//释放资源3
	}
	return 0;
}

DWORD WINAPI perform(LPVOID lpParam)
{
	while (1)
	{
		WaitForSingleObject( ghSemaphore3,INFINITE);		//申请资源3
		WaitForSingleObject(mutex,INFINITE);
		InterpretCmd(command,config);						//执行命令
		ReleaseMutex(mutex);
		ReleaseSemaphore(ghSemaphore1,1,NULL);				//释放资源1
	}
	return 0;
}

int main()
{
	cout<<"**************************  FileSystem  *************************"<<endl;
	cout<<"    ______________________created by lzy_____________________  "<<endl;
	fstream fin;
	fin.open("A.txt",ios::in|ios::out|ios::binary);
	if(!fin)
	{
		Init_SuperBlock();
		Init_INode();
		Init_DataBlock();
	}
	fin.close();
	cout<<"		你可以输入 help 获得相关命令的提示"<<endl;
	cout<<"	  你也可以输入 help command 获得command命令的详细提示"<<endl;
	//初始化路径
	num=1;
	Path[0]=0;
	strcpy(CurDirName,"FileSystem:");
	ReadSuperBlock();
	//定义线程变量
	DWORD readerid, analizerid, performerid;
	HANDLE  reader, analizer, performer;	
	//生成互斥量
    mutex = CreateMutex( NULL,FALSE,NULL);
	//生存信号量
    ghSemaphore1 = CreateSemaphore( NULL, 1,1,NULL); 
	ghSemaphore2 = CreateSemaphore( NULL, 0,1,NULL); 
	ghSemaphore3 = CreateSemaphore( NULL, 0,1,NULL); 
	//创造多线程
	reader = CreateThread(NULL, 0, read, NULL, 0, &readerid); 
	analizer = CreateThread(NULL, 0, analize, NULL, 0, &analizerid);
	performer = CreateThread(NULL, 0, perform, NULL, 0, &performerid);
	//等待线程
	WaitForSingleObject(reader, INFINITE);
	WaitForSingleObject(analizer, INFINITE);
	WaitForSingleObject(performer, INFINITE);
	//关闭线程
	CloseHandle(mutex);					
    CloseHandle(ghSemaphore1);			
	CloseHandle(ghSemaphore2);
	CloseHandle(ghSemaphore2);

	return 0;
}