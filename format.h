#ifndef _INCLUDE_XXXXXX01_H_
#define _INCLUDE_XXXXXX01_H_

#include "data_structure.h"

/* 初始化整个文件系统,创建一个$DISK文件,向其中写入20M的' ',将这个$DISK文件模拟成一个磁盘 */	
void init(){
	FILE *file=fopen(diskName,"w+"); //格式化磁盘,以'w+'方式打开文件,文件内容会被全部清空
	//判断'磁盘'($DISK文件)是否打开成功,失败则退出程序
	if(!file){
		printf("Error  Can't open the $DISK\n");
		exit(0);
	}
	char character=' ';
	for(int i=0;i<20971520;i++)
		fwrite(&character,1,1,file); //向$DISK文件中写入20M的空白符
	fclose(file);
}

/* 成组链接法 */
/* 0#是系统超级块,这个块中存放超级盘块号栈 */
/* 注意这个函数并不是在每次启动文件系统的时候调用,而是在格式化文件系统的时候调用 */
/* 本函数调用以后会同时在内存中加载超级盘块号栈 */
void groupLink(){
	superStack[0]=50; //超级盘块号栈的栈顶指针,它指示当前栈中尚有的空闲盘块数
	/* 
		注意空闲盘块号栈是一个自底向上生成的一个栈,栈底是superStack[50],栈顶是superStack[0],之所以这样做
		是为了使盘块分配操作更加简单
	*/
	for(short i=1;i<=50;i++)
		superStack[51-i]=30+i;//初始状态时,超级盘块号栈存放的是成组链接法中第一组空闲盘块的盘块号,即31#-80#
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	fwrite(&totalBlockNum,2,1,file);  //在0#中写入系统文件区空闲盘块总数(20450)
	/*写入系统超级盘块号栈*/
	fwrite(superStack,2,51,file);
	/* 
	   31# 81# 131#...20381# 20431#(最后一组)都是每组的第一个盘块号,除了20431#,前面的都需要记录后面一组
	   所有可用的盘块号
	*/
	for(short blockNum=31;blockNum<=20331;blockNum+=50){
		fseek(file,1024*blockNum,SEEK_SET);
		fwrite(&BLOCKNUM,2,1,file); //先写入栈顶指针,除了最后一组,前面各组的栈顶指针值都是50
		for(short nextGroupBlockNum=blockNum+50+49;nextGroupBlockNum>=blockNum+50;nextGroupBlockNum--)
			fwrite(&nextGroupBlockNum,2,1,file);
	}
	/*
		最后一组比较特殊,单独拿出来处理,要注意对于每个盘块号栈,superStack[1]指示的是下一组第一个盘块
		的盘块号,这个盘块中记录了其下一组所有可用盘块的盘块号,superStack[1]同时作为一个标志位,当它为0
		的时候意味着已经到了最后一组
	*/
	fseek(file,1024*20381,SEEK_SET);
	short temp=49;
	fwrite(&temp,2,1,file);	/*注意最后一组虽然也是50个盘块,但实际可用的盘块只有49个(superStack[1]用作标志位)
							  这也意味着20431#盘块是系统无法使用的一个盘块 */
	fwrite(&ENDFLAG,2,1,file);	//结束标志位
	for(short blockNum=20480;blockNum>=20432;blockNum--)
		fwrite(&blockNum,2,1,file);
	fclose(file);
	/*至此成组链接初始化工作完成*/
}

/* 系统格式化的时候,需要初始化根目录,'Linux/Unix一切皆文件',因而根目录也是作为一个文件来处理 */
/* 对于根目录,它作为一个文件也需要一个索引结点,本函数为根目录分配索引结点 */
void initialRootDIR(){
	systemiNode[0]->fileType=DIRECTORY;
	for(short i=0;i<10;i++)
		systemiNode[0]->iaddr[i]=i+21; //根目录占用的盘块是21#-30#
	systemiNode[0]->iaddr[10]=-1; //结束标志
	systemiNode[0]->fileLength=10240;
	systemiNode[0]->linkCount=0; /* 这里有疑问 暂时保留这个问题 */
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	fseek(file,1024*1,SEEK_SET);
	fwrite(&systemiNode[0],sizeof(INODE),1,file);
	fclose(file);
	//currentiNodeNum=1;
}

/* 格式化磁盘,这是一个'危险'的动作,执行此函数将抹掉系统所有数据,并将系统还原到初始状态 */
/* 格式化操作以后,根目录区以及iNode区的所有空白项都被打上了-1标志,分配空间时以此为参考 */
void format(){
	/* 清空数据 */
	init();

	/* 成组链接初始化 */
	groupLink();

	/* 初始化系统根目录以及iNode */
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	for(short i=0;i<640;i++)
		rootDIR[i].inodeNum=-1;
	fseek(file,1024*21,SEEK_SET);
	fwrite(rootDIR,sizeof(dirItem),640,file);
	for(short i=0;i<640;i++)
		systemiNode[i].fileLength=-1;
	initialRootDIR(); //为根目录分配iNode
	fseek(file,1024*1,SEEK_SET);
	fwrite(systemiNode,sizeof(INODE),640,file);
	fclose();

	/* 将数据加载到内存 */
	//load();
}


#endif