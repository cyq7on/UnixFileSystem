/*
	定义文件系统的'系统调用'
*/
#include "data_structure.h"

/* 文件系统'启动'时,将系统的必要信息(如空闲盘块、超级盘块号栈等)加载到内存 */
void load(){
	FILE *file=fopen(diskName,"r");
	if(!file){
		printf("Error! Can't open the DISK\n");
		exit(0);
	}
 	//fscanf(file,"%hd",&currentFreeBlockNum); 
 	fread(&currentFreeBlockNum,2,1,file);  //读取系统空闲盘块数
 	/*for(int i=0;i<=BLOCKNUM;i++)
 		fscanf(file,"%hd",&superStack[i]); //加载系统超级盘块号栈*/
 	fread(superStack,2,BLOCKNUM+1,file);
 	fclose(file);
}

/*
	成组链接法中,当一组盘块已经分配完,则需要将下一组盘块的第一个盘块中记录的信息调入超级盘块号栈,因为这个
	操作经常被使用,因而将其封装成一个函数,每次要进行此操作时可调用本函数完成
*/
void arrayCopy(short _blockNum){
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the DISK\n");
		exit(0);
	}
	fseek(file,1024*_blockNum,SEEK_SET);
	/*for(int i=0;i<=50;i++)
		fscanf(file,"%hd",superStack[i]);*/
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