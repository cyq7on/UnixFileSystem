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
	FILE *file=fopen(diskName,"r");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	fseek(file,1024*_blockNum,SEEK_SET);
	fread(superStack,2,BLOCKNUM+1,file);
	fclose(file);
}

/* 本函数实现将一个长度为512个元素的short型数组写入$DISK的指定盘块中,Unix混合索引的一级及二级索引分配时使用 */
void arrayWrite(short array[],short _blockNum){
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	fseek(file,1024*_blockNum,SEEK_SET);
	fwrite(array,2,512,file);
	fclose(file);
}
/* 
	文件的大小是以Byte为单位给出的,但是对于磁盘空间的分配是以盘块为单位的.
	本函数实现根据文件的字节长度计算其文件所需的盘块数
*/
short convertFileLength(short fileLength){
	return fileLength/1024+1;
}


/* 该函数实现分配一个空闲的文件区盘块,函数返回值是分配的盘块号 */
/* 该操作需要用到超级盘块号栈 */
short allocateAnEmptyBlock(){
	short result;
	/* 正常情况.空闲盘块号栈中的空闲的盘块数大于1 */
	if(superStack[0]>1){ 
		result=superStack[superStack[0]]; //
		superStack[0]--; //栈顶指针下移一位
		currentFreeBlockNum--;
		return result;
	}
	else{
		/* 标志位为0,说明文件区盘块已经用尽,无法分配 */
		if(superStack[1]==0){
			printf("Sorry,There is no spare disk block available for distribution!\n");
			return -1;
		}
		/* 当前组盘块已经只剩下最后一个了,需要先将下一组的第一个盘块的信息写入盘块号栈,再分配 */
		result=superStack[1]; /* 将盘块号赋值给result,注意这个操作与下一个操作的区别,这个是取出盘块号
		                         下一个操作是取出盘块号对应的内容 */
		arrayCopy(superStack[1]);
		currentFreeBlockNum--;
		return result;
	}
}


/* 在Unix中,iNode是顺序排列的,所以不需要有单独的一个字段来记录iNode号 */
/* 创建一个iNode结点 */
void creatiNode(INODE *_inode,byte fileType,int fileLength,byte linkCount){

	short i=0; //定义一个循环变量
	_inode->fileType=fileType;



   /*------------------------分配盘块BEGIN------------------------------*/
	for(i=0;i<13;i++)
		_inode->iaddr[i]=-1;
	/* 如果该文件是目录文件,那么分配4个盘块 */
	if(fileType==1){
		for(i=0;i<4;i++)
			_inode->iaddr[i]=allocateAnEmptyBlock();
	}

	/* Unix采用混合索引方式,因而对于文件的分配是一个相对复杂的过程 */
	else{
		short count=convertFileLength(fileLength); //计算需要分配给文件的盘块数

		/* 直接寻址方式 */
		for(i=0;i<10&&count>0;i++,count--)
			_inode->iaddr[i]=allocateAnEmptyBlock();

		/* 一级索引分配方式 */
		if(count>0){ 
			_inode->iaddr[10]=allocateAnEmptyBlock(); //分配一次间址块
			/* 在本系统中,一个盘块的大小为1KB,每个盘块号占2Byte,所以一个索引块最多可以存放512个盘块号 */
			short singleIndirect[512];
			for(i=0;i<512;i++)
				singleIndirect[i]=-1;
			for(i=0;i<512&&count>0;i++,count--)
				singleIndirect[i]=allocateAnEmptyBlock();
			arrayWrite(singleIndirect,_inode->iaddr[10]); //将一次索引盘块号都记录到索引块中


			/* 二级索引分配方式 */
			if(count>0){
				_inode->iaddr[11]=allocateAnEmptyBlock(); //记录索引块块号的索引块
				short doubleIndirect[512];
				for(i=0;i<512;i++)
					doubleIndirect[i]=-1;
				for(i=0;i<512&&count>0;i++){

					singleIndirect[i]=allocateAnEmptyBlock(); /* 最内层记录文件实际盘块号的各个索引块 */

					for(short j=0;j<512&&count>0;j++,count--){
							doubleIndirect[j]=allocateAnEmptyBlock(); //文件实际占用的盘块
					}
					/*程序执行到这里,亦即是跳出了上面这个for循环,对应于两种情况
					  第I种情况是count为0了,也就是说文件所需要的盘块已经全部分配完毕
					  第II种情况是singleIndirect[i]这个盘块已经装满了文件的盘块号
					  不管是哪种情况,都需要将doubleIndirect数组的数据写入到singleIndirect[i]盘块中
					*/
					arrayWrite(doubleIndirect,singleIndirect[i]);
				}
				arrayWrite(singleIndirect,_inode->iaddr[11]);
			}
			//到二级索引的时候,文件系统支持的文件长度已经达到256MB以上了,完全支持对整个$DISK的操作
			//因而三级索引先一放,最后有时间再来补充
		}


	}
	/*------------------------分配盘块END------------------------------*/

	_inode->fileLength=fileLength;
	_inode->linkCount=linkCount;
}


/* 本函数将iNode写入系统iNode区(1#-20#盘块) */
void writeiNode(INODE *_inode){
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	fseek(file,,SEEK_SET);

}

#endif