/*
	定义文件系统的'系统调用'
*/
#ifndef _INCLUDE_XXXXXX04_H_
#define _INCLUDE_XXXXXX04_H_

#include "data_structure.h"

/* 文件系统'启动'时,将系统的必要信息(如空闲盘块、超级盘块号栈等)加载到内存 */
void load(){
	FILE *file=fopen(diskName,"r");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
 	fread(&currentFreeBlockNum,2,1,file);  //读取系统空闲盘块数
 	fread(superStack,2,BLOCKNUM+1,file);  //加载系统超级盘块号栈
 	fclose(file);
}

/*
	成组链接法中,当一组盘块已经分配完,则需要将下一组盘块的第一个盘块中记录的信息调入超级盘块号栈,因为这个
	操作经常被使用,因而将其封装成一个函数,每次要进行此操作时可调用本函数完成
*/
void arrayCopy(short _blockNum){
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	fseek(file,1024*_blockNum,SEEK_SET);
	fread(superStack,2,BLOCKNUM+1,file);
	fclose(file);
}

/* 
	文件的大小是以Byte为单位给出的,但是对于磁盘空间的分配是以盘块为单位的.
	本函数实现根据文件的字节长度计算其文件所需的盘块数
*/
short convertFileLength(short fileLength){
	return fileLength/1024+1;
}

/* 系统1#-20#是索引结点区,每个索引结点占32Byte,在一个盘块中可以存放32个索引结点*/
/* 对于iNode,本系统维护了一个iNode栈, */
/****/

/* 该函数实现分配一个空闲的文件区盘块,函数返回值是分配的盘块号 */
/* 该操作需要用到超级盘块号栈 */
short allocateAnEmptyBlock(){
	short result;
	/* 正常情况.空闲盘块号栈中的空闲的盘块数大于1 */
	if(superStack[0]>1){ 
		short=superStack[superStack[0]]; //
		superStack[0]--; //栈顶指针下移一位

		currentFreeBlockNum--; 

	}
}

/* 在Unix中,iNode是顺序排列的,所以不需要有单独的一个字段来记录iNode号 */
/* 创建一个iNode结点 */
void creatiNode(INODE *_inode,byte fileType,short iaddr[],fileLength,byte linkCount){
	_inode->fileType=fileType;
	/* 如果该文件是目录文件,那么分配4个盘块 */
	if(fileType==1){

	}

}

#endif