/*
	20M=20480Kb=20971520 byte.
	所以创建一个文件,向这个文件中写入20971520个字节的数据即可模拟出一个20M的磁盘
	不妨设写入的字符为'#'.
*/
#include "data_structure.h"
#include <stdio.h>
#include <stdlib.h>

/*初始化整个文件系统,创建一个DISK文件,向其中写入20M的'#',将这个DISK文件模拟成一个磁盘*/	
void init(){
	FILE *file=fopen(diskName,"w+");
	//判断'磁盘'(DISK文件)是否打开成功,失败则退出程序
	if(!file){
		printf("Error!\n");
		exit(0);
	}
	for(int i=0;i<20971520;i++) //向DISK文件中写入20M的'#'
		fprintf(file,"%c",'#');
	fclose(file);
}

/*成组链接法*/
/* 0#是系统超级块,这个块中存放超级盘块号栈 */
void groupLink(){
	superStack[0]=50; //超级盘块号栈的栈顶指针,它指示当前栈中尚有的空闲盘块数
	for(short i=1;i<=50;i++)
		superStack[i]=30+i;//初始状态时,超级盘块号栈存放的是成组链接法中第一组空闲盘块的盘块号,即31#-80#
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error!\n");
		exit(0);
	}
	fprintf(file,"%hd",currentFreeBlockNum); //在0#中写入当前系统可供分配的文件区空闲盘块数
	/*写入系统超级盘块号栈*/
	for(short i=0;i<=50;i++)
		fprintf(file,"%hd",superStack[i]);
	/* 
	   31# 81# 131#...20381# 20431#(最后一组)都是每组的第一个盘块号,除了20431#,前面的都需要记录后面一组
	   所有可用的盘块号
	*/
	for(short blockNum=31;blockNum<=20381;blockNum+=50){
		fseek(file,1024*blockNum,SEEK_SET);
		fprintf(file,"%hd",50);//先写入栈顶指针,除了最后一组,前面各组的栈顶指针值都是50
		for(short nextGroupBlockNum=blockNum+50;nextGroupBlockNum<=blockNum+49;nextGroupBlockNum++)
			fprintf(file,"%hd",nextGroupBlockNum);
	}
	/*
		最后一组比较特殊,单独拿出来处理,要注意对于每个盘块号栈,superStack[1]指示的是下一组第一个盘块
		的盘块号,这个盘块中记录了其下一组所有可用盘块的盘块号,superStack[1]同时作为一个标志位,当它为0
		的时候意味着已经到了最后一组
	*/
	fseek(file,1024*20431,SEEK_SET);
	fprintf(file,"%hd",49); //最后一组可用的盘块数只有49个(superStack[1]用作标志位)
	fprintf(file,"%hd",0); //结束标志位
	for(short blockNum=20432;blockNum<=20480;blockNum++)
		fprintf(file,"%hd",blockNum);
	/*至此成组链接初始化完成*/
}