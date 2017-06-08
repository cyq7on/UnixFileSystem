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
void arrayWrite(short _array[],short _blockNum){
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	fseek(file,1024*_blockNum,SEEK_SET);
	fwrite(_array,2,512,file);
	fclose(file);
}
/* 
	文件的大小是以Byte为单位给出的,但是对于磁盘空间的分配是以盘块为单位的.
	本函数实现根据文件的字节长度计算其文件所需的盘块数
*/
short convertFileLength(short _fileLength){
	return _fileLength%1024==0?_fileLength/1024:_fileLength/1024+1;
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

/* 回收一个盘块 */
void recycleAnBlock(short _diskNum){
	if(superStack[0]<=49){
		superStack[0]++;
		superStack[superStack[0]]=_diskNum;
	}
	/* 如果空闲盘块号栈满,则需要先将空闲盘块号栈的数据写入到新回收的这个盘块中 */
	else{
		FILE *file=fopen(diskName,"r+");
		if(!file){
			printf("Error! Can't open the $DISK\n");
			exit(0);
		}
		fseek(file,1024*_diskNum,SEEK_SET);
		fwrite(superStack,sizeof(short),51,file);
		fclose(file);
		superStack[0]=1;
		superStack[1]=_diskNum;
	}
	currentFreeBlockNum++; //系统空闲盘块数加一
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
	if(fileType==DIRECTORY){
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
/*void writeiNode(INODE *_inode){
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	fseek(file,1024*10+currentiNodeNum*sizeof(INODE),SEEK_SET);

}*/


/* 创建文件,需要给出文件名和文件长度 */
/* 创建目录文件的函数待会单独写,这个函数的参数列表不给出文件类型,默认就是NORMAL类文件 */
/* 
   本函数的返回值将作为本次操作的状态码传递给调用者供其参考
   -1:文件重名,操作失败
    0:系统空间不足,操作失败
    1:操作成功
*/
int creatFile(char _fileName[],int _fileLength){
	/* 
	   创建文件需要先扫描一下同级目录中有没有同名文件,如果有同名的需要再判断一下这个文件是目录
	   文件还是普通文件,同一目录下允许子目录和其中的文件同名,对于NORMAL类文件如果出现重名则拒绝
	   执行文件创建操作.
	*/
	short tempLength;
	if(!strcmp(currentDirName,"/")) //本系统中,根目录的项数是640,子目录的项数都是128
		tempLength=640;
	else
		tempLength=128;
	for(short i=0;i<tempLength;i++){
		if(!strcmp(currentDIR[i].fileName,_fileName)){  //发现同名项
			/* 如果这个同名项是目录,那么允许同名 */
			if(systemiNode[currentDIR[i].inodeNum].fileType==DIRECTORY) 
				continue; //这里不能直接break,因为必须要扫描完整个目录
			/* 出现同名文件了 */
			else
				return -1; 
		} 

	}
	/* 先根据文件的字节长度计算该文件所需要占用的盘块数目 */
	int fileLength=convertFileLength(_fileLength);
	/* 文件长度如果超过剩余盘块数或者系统当前iNode已经用尽则无法再分配 */
	if(fileLength>currentFreeBlockNum||currentFreeiNodeNum==0)
		//printf("Sorry,There is no spare disk block available for distribution!\n");
		return 0;

	/* 创建一个文件需要先申请iNode而后填写目录项,两个操作的顺序不能颠倒 */

	/* 申请iNode */
	/* 如何获取一个空白iNode号是一个值得考虑的问题,暂时采用线性扫描法 */
	short tempiNodeNum=-1; //这个变量用来记录分配到的iNode号
	for(short i=0;i<640;i++,tempiNodeNum++){
		if(systemiNode[i].fileLength==-1)
			break;
	}
	creatiNode(&systemiNode[tempiNodeNum],NORMAL,_fileLength,1);
	currentiNodeNum--; //当前可用的iNode数量减一

	/* 申请目录项 */
	/* 写入文件名(注意:strcpy()方法不会对内存做限制,长度超限会造成缓冲区溢出,产生不可预知的错误) */
	short tempDirNum=-1; //tempDirNum用来记录分配到的目录项号
	for(short i=0;i<tempLength;i++,tempDirNum++){
		if(currentDIR[i].inodeNum==-1)
			break;
	}
	strcpy(currentDIR[tempDirNum].fileName,_fileName);
	/* 写入iNode编号 */ 
	currentDIR[tempDirNum].inodeNum=tempiNodeNum;

	//文件创建工作完成
	return 1;
}

/* 创建一个子目录 */
/* 'Linux/Unix一切皆文件',所以对于目录的创建和普通文件有些类似 同样也会因为空间或者iNode耗尽而无法创建 */
int creatDir(char _dirName[]){
	short tempLength;
	if(!strcmp(currentDirName,"/")) //本系统中,根目录的项数是640,子目录的项数都是128
		tempLength=640;
	else
		tempLength=128;

	/* 检测当前路径下是否有同名的目录项 */
	for(short i=0;i<tempLength;i++){
		/* 对于目录来说,在同一级目录下出现同名,可以直接拒绝执行 */
		if(!strcmp(currentDIR[i].fileName,_dirName))
			return -1;
	}

	/* 在本系统中,每个子目录分配4个盘块 */
	if(currentFreeBlockNum<4||currentFreeiNodeNum==0)
		//printf("Sorry,There is no spare disk block available for distribution!\n");
		return 0;

	/* 重名问题以及空间问题已经检查完毕,下面开始正式创建 */

	/* 申请iNode */
	/* 如何获取一个空白iNode号是一个值得考虑的问题,暂时采用线性扫描法 */
	short tempiNodeNum=-1; //这个变量用来记录分配到的iNode号
	for(short i=0;i<640;i++,tempiNodeNum++){
		if(systemiNode[i].fileLength==-1)
			break;
	}
	creatiNode(&systemiNode[tempiNodeNum],DIRECTORY,1024*4,1);
	currentiNodeNum--; //当前可用的iNode数量减一

	/* 申请目录项 */
	/* 写入文件名(注意:strcpy()方法不会对内存做限制,长度超限会造成缓冲区溢出,产生不可预知的错误) */
	short tempDirNum=-1; //tempDirNum用来记录分配到的目录项号
	for(short i=0;i<tempLength;i++,tempDirNum++){
		if(currentDIR[i].inodeNum==-1)
			break;
	}
	strcpy(currentDIR[tempDirNum].fileName,_dirName);
	/* 写入iNode编号 */ 
	currentDIR[tempDirNum].inodeNum=tempiNodeNum;

	//目录创建工作完成
	return 1;

}


/* 删除当前目录下的指定文件 */
void deleteFile(char _fileName[]){
	short tempLength;
	if(!strcmp(currentDirName,"/")) //本系统中,根目录的项数是640,子目录的项数都是128
		tempLength=640;
	else
		tempLength=128;
	for(short i=0;i<tempLength;i++){
		if(!strcmp(currentDIR[i].fileName,_fileName)){
			/* 需要判断一下被删除的目标文件是目录文件还是非目录文件 */

			/* 如果是目录文件 */
			if(systemiNode[currentDIR[i].inodeNum].fileType==DIRECTORY){
				/* 对于目录文件的删除,需要检测一下它是否有下属文件或子目录,有的话拒绝执行删除操作 */
				
			}
		}
	}
}

#endif