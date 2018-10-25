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

HANDLE mutex;										//һ��������
HANDLE ghSemaphore1, ghSemaphore2, ghSemaphore3;	//�����ź���

char Input[2*BLKSIZE];		//��������
char command[20];			//����
char config[BLKSIZE+30];	//����


DWORD WINAPI read(LPVOID lpParam)
{
	while (1)
	{	
		WaitForSingleObject( ghSemaphore1,INFINITE);		//������Դ1
		WaitForSingleObject(mutex,INFINITE);
		PrintCurPath();
		cout<<">";
		cin.getline(Input,2*BLKSIZE,'\n');					//��������
		ReleaseMutex(mutex);
		ReleaseSemaphore(ghSemaphore2,1,NULL);				//�ͷ���Դ2
	}
	return 0;
}

DWORD WINAPI analize(LPVOID lpParam)
{
	while (1)
	{
		WaitForSingleObject( ghSemaphore2,INFINITE);		//������Դ2
		WaitForSingleObject(mutex,INFINITE);
		SplitCmd(Input,command,config);						//�������
		ReleaseMutex(mutex);
		ReleaseSemaphore(ghSemaphore3,1,NULL);				//�ͷ���Դ3
	}
	return 0;
}

DWORD WINAPI perform(LPVOID lpParam)
{
	while (1)
	{
		WaitForSingleObject( ghSemaphore3,INFINITE);		//������Դ3
		WaitForSingleObject(mutex,INFINITE);
		InterpretCmd(command,config);						//ִ������
		ReleaseMutex(mutex);
		ReleaseSemaphore(ghSemaphore1,1,NULL);				//�ͷ���Դ1
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
	cout<<"		��������� help �������������ʾ"<<endl;
	cout<<"	  ��Ҳ�������� help command ���command�������ϸ��ʾ"<<endl;
	//��ʼ��·��
	num=1;
	Path[0]=0;
	strcpy(CurDirName,"FileSystem:");
	ReadSuperBlock();
	//�����̱߳���
	DWORD readerid, analizerid, performerid;
	HANDLE  reader, analizer, performer;	
	//���ɻ�����
    mutex = CreateMutex( NULL,FALSE,NULL);
	//�����ź���
    ghSemaphore1 = CreateSemaphore( NULL, 1,1,NULL); 
	ghSemaphore2 = CreateSemaphore( NULL, 0,1,NULL); 
	ghSemaphore3 = CreateSemaphore( NULL, 0,1,NULL); 
	//������߳�
	reader = CreateThread(NULL, 0, read, NULL, 0, &readerid); 
	analizer = CreateThread(NULL, 0, analize, NULL, 0, &analizerid);
	performer = CreateThread(NULL, 0, perform, NULL, 0, &performerid);
	//�ȴ��߳�
	WaitForSingleObject(reader, INFINITE);
	WaitForSingleObject(analizer, INFINITE);
	WaitForSingleObject(performer, INFINITE);
	//�ر��߳�
	CloseHandle(mutex);					
    CloseHandle(ghSemaphore1);			
	CloseHandle(ghSemaphore2);
	CloseHandle(ghSemaphore2);

	return 0;
}