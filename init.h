/*
	20M=20480Kb=20971520 byte.
	所以创建一个文件,向这个文件中写入20971520个字节的数据即可模拟出一个20M的磁盘
	不妨取写入的字符为'#'.
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

}