﻿#ifndef _INCLUDE_XXXXXX01_H_
#define _INCLUDE_XXXXXX01_H_

#include "data_structure.h"

/* 系统初始化函数 */
/* 初始化整个文件系统,创建一个$DISK文件,向其中写入20M的' ',将这个$DISK文件模拟成一个磁盘 */	
/* 对文件系统执行格式化操作时先调用本函数 */
void init(){
	FILE *file=fopen(diskName,"w+"); //格式化磁盘,以'w+'方式打开文件,文件内容会被全部清空
	//判断'磁盘'($DISK文件)是否打开成功,失败则退出程序
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	char character=' ';
	for(int i=0;i<20971520;i++)
		fwrite(&character,sizeof(char),1,file); //向$DISK文件中写入20M的空白符
	fclose(file);
}

/* 成组链接法初始化函数 */
/* 0#是系统引导扇区,这个盘块中存放系统的空闲盘块号栈 */
/* 空闲盘块号栈是在内存中维护的一个系统栈,是Unix文件系统在分配和回收盘块时都要使用的核心数据结构 */
/* 本文件系统只是简单地模拟,没有中断机制,因而强制关闭会造成数据丢失,每次退出文件前需要调用'关机'*/
/* 函数,这个函数会将内存的空闲盘块号栈等系统数据写回'磁盘'引导区 */
/* 注意这个函数并不是在每次启动文件系统的时候调用,而是在格式化文件系统的时候调用 */
/* 本函数调用以后会同时在内存中加载空闲盘块号栈 */
void groupLink(){
	superStack[0]=50; //空闲盘块号栈的栈顶指针,它同时也指示当前栈中尚有的空闲盘块数
	/* 
		注意空闲盘块号栈是一个自底向上生成的一个栈,栈底是superStack[50],栈顶是superStack[1],之所以这样做
		是为了使盘块分配操作更加简单
	*/
	for(short i=1;i<=50;i++)
		superStack[51-i]=30+i;//初始状态时,空闲盘块号栈存放的是成组链接法中第一组空闲盘块的盘块号,即31#-80#
	FILE *file=fopen(diskName,"rb+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	/*写入系统空闲盘块号栈*/
	//fseek(file,0,SEEK_SET);
	fwrite(superStack,sizeof(short),51,file);
	totalBlockNum=20449;
	currentFreeBlockNum=20449;
	currentFreeiNodeNum=639;
	systemFileNum=0;
	fwrite(&totalBlockNum,sizeof(short),1,file);  //写入系统文件区盘块总数(20449)
	fwrite(&currentFreeBlockNum,sizeof(short),1,file); //写入当前可用的文件区空闲盘块数
	fwrite(&currentFreeiNodeNum,sizeof(short),1,file); //写入当前可用的iNode数
	fwrite(&systemFileNum,sizeof(short),1,file); //写入当前系统的文件总数
	/* 
	   80# 130# 180#...20380# 20430# 20480#(最后一组)都是每组的第一个盘块号,除了20480#,前面的都需要记录后面一组
	   所有可用的盘块号
	*/
	short blockNum,nextGroupBlockNum;
	for(blockNum=80;blockNum<=20380;blockNum+=50){
		fseek(file,1024*blockNum,SEEK_SET);
		fwrite(&BLOCKNUM,2,1,file); //先写入栈顶指针,除了最后一组,前面各组的栈顶指针值都是50
		for(nextGroupBlockNum=blockNum+50;nextGroupBlockNum>blockNum;nextGroupBlockNum--)
			fwrite(&nextGroupBlockNum,2,1,file);
	}

	/*
		最后一组比较特殊,单独拿出来处理,要注意对于每个盘块号栈,superStack[1]指示的是下一组第一个盘块
		的盘块号,这个盘块中记录了其下一组所有可用盘块的盘块号,superStack[1]同时作为一个标志位,当它为0
		的时候意味着已经到了最后一组
	*/
	fseek(file,1024*20430,SEEK_SET);
	/*注意最后一组虽然也是50个盘块,但实际可用的盘块只有49个(superStack[1]用作标志位)
							  这也意味着20480#盘块是系统无法使用的一个盘块 */
	short temp=49;
	fwrite(&temp,sizeof(short),1,file);	
	fwrite(&ENDFLAG,sizeof(short),1,file);	//结束标志位
	for(short blockNum=20479;blockNum>20430;blockNum--)
		fwrite(&blockNum,sizeof(short),1,file);
	
	fclose(file);
	/*至此成组链接初始化工作完成*/
}

/* 系统格式化的时候,需要初始化根目录,'Linux/Unix一切皆文件',因而根目录也是作为一个文件来处理 */
/* 对于根目录,它作为一个文件也需要一个iNode,本函数为根目录分配iNode */
/* 本函数没有将分配的iNode写入磁盘,只是写在了内存的iNode表中 */
void initialRootDIR(){
	systemiNode[0].fileType=DIRECTORY;
	for(short i=0;i<10;i++)
		systemiNode[0].iaddr[i]=i+21; //根目录占用的盘块是21#-30#
	systemiNode[0].iaddr[10]=-1; //结束标志位
	systemiNode[0].fileLength=10240;
	systemiNode[0].linkCount=0; /* 这里有疑问 暂时保留这个问题 */
}

/* 格式化磁盘,这是一个'危险'的动作,执行此函数将抹掉系统所有数据,并将系统还原到初始状态 */
/* 格式化操作以后,根目录区以及iNode区的所有空白项都被打上了-1标志,分配空间时以此为参考 */
void format(){
	/* 清空数据 */
	init();
	
	
	/* 成组链接初始化 */
	groupLink();

	/* 初始化系统根目录以及iNode */

	FILE *file=fopen(diskName,"rb+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	/* 初始化系统iNode */
	for(short i=0;i<640;i++)
		systemiNode[i].fileLength=-1;

	initialRootDIR(); //为根目录分配iNode

	/* 将初始化完毕的iNode写入系统iNode区(1#-20#盘块) */
	fseek(file,1024*1,SEEK_SET);
	fwrite(systemiNode,sizeof(INODE),640,file);


	/* 将初始化的根目录表写入磁盘的21#-30#盘块 */
	/* 根目录的第一项(rootDIR[0])作为其自身的目录项 */
	strcpy(rootDIR[0].fileName,"/");
	rootDIR[0].inodeNum=0;

	for(short i=1;i<640;i++)
		rootDIR[i].inodeNum=-1;
	fseek(file,1024*21,SEEK_SET);
	fwrite(rootDIR,sizeof(dirItem),640,file);
	
	/* 关闭文件,格式化操作完成 */
	fclose(file);
}


#endif