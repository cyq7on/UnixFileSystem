/*
	定义文件系统的'系统调用'
*/
#ifndef _INCLUDE_XXXXXX04_H_
#define _INCLUDE_XXXXXX04_H_

#include "data_structure.h"

/* 文件系统'启动'时,将系统的引导区数据(系统当前空闲盘块数、系统空闲盘块号栈等等)加载到内存 */
/* 每次'开机'时最先调用本函数 */
void load(){
	FILE *file=fopen(diskName,"r");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
 	fread(superStack,sizeof(short),BLOCKNUM+1,file);  //加载系统空闲盘块号栈
 	fread(&totalBlockNum,sizeof(short),1,file); //读取系统文件区物理盘块总数
 	fread(&currentFreeBlockNum,sizeof(short),1,file);  //读取系统当前空闲盘块数
 	fread(&currentFreeiNodeNum,sizeof(short),1,file); //读取系统当前可用的iNode数目
 	fread(&systemFileNum,sizeof(short),1,file); //读取系统文件总数
 	stackLock=0; //初始化stack排他锁
 	fseek(file,1024*1,SEEK_SET);
 	fread(systemiNode,sizeof(INODE),640,file); //加载系统iNode栈
 	fseek(file,1024*21,SEEK_SET);
 	fread(rootDIR,sizeof(dirItem),640,file); //加载系统根目录
 	currentDIR=rootDIR; //初始化当前路径指针
 	strcpy(currentDirName,"/");  //初始化当前路径名
 	currentDiriNode=&systemiNode[0]; //初始化当前目录的iNode指针
 	openedDirStack[0]=&systemiNode[0]; //初始化openedDirStack栈,让其指向根目录的iNode
 	openedDirStackPointer=0; //初始化openedDirStack栈栈顶指针
 	fclose(file);
}

/* 本函数将iNode段写回磁盘 */
/* iNode栈是在内存中维护的一个系统栈,每次有新的改动后最好将其立即写回磁盘,这样可以在一定程度上避免'非法关机'
   造成的错误
*/
void writeiNode(){
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	fseek(file,1024*1,SEEK_SET);
	fwrite(systemiNode,sizeof(INODE),640,file);
	fclose(file);
}

/* 将当前系统目录段写回磁盘 */
/* 根目录段的写回是比较简单的,因为根目录占用的盘块是确定的(21#-30#)
   如果是子目录则需要先获取子目录分配到的盘块,再将数据写入
*/
void writeCurrentDir(){
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	/* 如果当前目录是系统根目录 */
	if(!strcmp(currentDirName,"/")){
		fseek(file,1024*21,SEEK_SET);
		fwrite(currentDIR,sizeof(dirItem),640,file);
		fclose(file);
		return;
	}
	/* 如果当前不是在系统根目录下 */
	else{
		short i,j,count=0; //设置一个计数变量
		for(i=0;i<4;i++){
			fseek(file,1024*(currentDiriNode->iaddr[i]),SEEK_SET);
			for(j=0;j<64;j++)
				fwrite(&currentDIR[count++],sizeof(dirItem),1,file);
		}
		fclose(file);
		return;
	}
}


/* '关机'函数 */
/* 本函数与'开机'函数(load()函数)相对应,也是本系统一个重要的函数 */
/* 每次关闭文件系统前,要调用本函数,本函数将系统变量及系统栈写回'磁盘'系统区 */
void shutDown(){
	/* 将当前iNode栈写回磁盘 */
	writeiNode();

	/* 将当前目录栈写回磁盘 */
	writeCurrentDir();

	/* 将空闲盘块号栈写回磁盘 */
	FILE *file=fopen(diskName,"r+");
	if(!file){
		printf("Error! Can't open the $DISK\n");
		exit(0);
	}
	fseek(file,0,SEEK_SET);
	fwrite(superStack,sizeof(short),51,file);

	/* 将各个系统变量写回引导区 */
	fwrite(&totalBlockNum,sizeof(short),1,file);  //写入系统文件区空闲盘块总数(20449)
	fwrite(&currentFreeBlockNum,sizeof(short),1,file); //写入当前可用的文件区空闲盘块数
	fwrite(&currentFreeiNodeNum,sizeof(short),1,file); //写入当前可用的iNode数
	fwrite(&systemFileNum,sizeof(short),1,file); //写入当前系统的文件总数
	fclose(file);
}

/* 打开当前路径下的指定子目录 */
/* 输入参数: 目录名称 */
/* 返回: 操作状态码 0:操作成功 404:目标项未找到 */
int openDir(char _dirName[]){
	short tempLength;
	if(!strcmp(currentDirName,"/")) //本系统中,根目录的项数是640,子目录的项数都是256
		tempLength=640;
	else
		tempLength=256;
	for(short i=0;i<tempLength;i++){
		if(!strcmp(currentDIR[i].fileName,_dirName)){
			if(systemiNode[currentDIR[i].inodeNum].fileType==DIRECTORY){
				/* 已找到目标目录项 */
				/* 下一步工作是将该目录的所有项提取到tempDir中 */
				short j,k,count=0;
				FILE *file=fopen(diskName,"r");
				if(!file){
					printf("Error! Can't open the $DISK\n");
					exit(0);
				}
				for(j=0;j<4;j++){
					fseek(file,1024*systemiNode[currentDIR[i].inodeNum].iaddr[j],SEEK_SET);
					for(k=0;k<64;k++)
						fread(&tempDir[count++],sizeof(dirItem),1,file);
				}
				fclose(file);
				/* 已将盘块中的目录数据加载至系统内存的临时目录区 */
				/* 将当前目录push进栈中 */
				openedDirStack[++openedDirStackPointer]=&systemiNode[currentDIR[i].inodeNum];
				strcat(currentDirName,"/");
				strcat(currentDirName,_dirName); //切换系统当前目录名称
				currentDIR=tempDir; //切换当前目录指针
				return 0;
			}
		}
	}
	/* 目标项未找到 */
	return 404;
}

/* 返回上一级目录 */
/* 输入参数:NULL */
/* 返回参数:操作状态码 0:操作成功 403:当前目录已是根目录,操作被拒绝 */
int returnPreDir(){
	if(!strcmp(currentDirName,"/"))
		return 403;
	/* 如果上一级目录是根目录,则不需要进行I/O操作即可进行目录切换 */
	if(openedDirStack[openedDirStackPointer-1]->iaddr[0]==21){
		openedDirStackPointer--; //更改openedDirStack栈的栈顶指针
		strcpy(currentDirName,"/"); //切换当前目录名称
		currentDIR=rootDIR; //切换当前目录指针
		currentDiriNode=&systemiNode[0]; //切换当前目录的iNode指针
		return 0;
	}
	else{
		/* 通过磁盘I/O,将上一级目录的目录表写入内存的临时目录区 */
		FILE *file=fopen(diskName,"r");
		if(!file){
			printf("Error! Can't open the $DISK\n");
			exit(0);
		}
		openedDirStackPointer--;
		short i,j,count=0;
		for(i=0;i<4;i++){
			fseek(file,1024*openedDirStack[openedDirStackPointer]->iaddr[i],SEEK_SET);
			for(j=0;j<64;j++){
				fread(&tempDir[count++],sizeof(dirItem),1,file);
			}			
		}
		currentDIR=tempDir; //切换当前目录指针
		/* 下一步的工作是截掉currentDirName从最后一个出现的'/'到字符串结尾的这段子串 */
		/* 例如当前目录是'/usr/bin/test',则需要截掉'/test',将其转化为'/usr/bin' */
		char tempString[80];
		for(char *p=&currentDirName[0],int Number=0;*p!='\0'&&Number<=openedDirStackPointer;p++){
			strcat(tempString,*p);
			if(*p=='/')
				Number++;
		}
		strcpy(currentDirName,tempString); //切换当前目录名称
		return 0;			
	}

}

/*
	成组链接法中,当一组盘块已经分配完,则需要将下一组盘块的第一个盘块中记录的信息调入空闲盘块号栈,因为这个
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
	fwrite(_array,sizeof(short),512,file);
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
/* 该操作需要用到空闲盘块号栈 */
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
/* 创建一个iNode结点,并分配所需盘块 */
/* 状态码:
   403: iNode或系统空闲盘块已耗尽,无法创建新的iNode
   0: 操作成功
*/
int creatiNode(INODE *_inode,byte fileType,int fileLength,byte linkCount){

	/* 如果系统iNode已经用完,则iNode创建操作失败 */
	if(currentFreeiNodeNum==0)
		return 403;

	short i=0; //定义一个循环变量
	_inode->fileType=fileType;



   /*------------------------分配盘块BEGIN------------------------------*/
	for(i=0;i<13;i++)
		_inode->iaddr[i]=-1;
	/* 如果该文件是目录文件,那么分配4个盘块 */
	/* 创建目录文件不仅仅是分配盘块,还要初始化目录项 */
	if(fileType==DIRECTORY){

		/* 如果系统可用空闲盘块数不足4个,则无法创建目录,本次操作失败 */
		if(currentFreeBlockNum<4)
			return 403;

		dirItem tempOneBlockDir[64];
		FILE *file=fopen(diskName,"r+");
		if(!file){
			printf("Error! Can't open the $DISK\n");
			exit(0);
		}
		for(i=0;i<4;i++){
			_inode->iaddr[i]=allocateAnEmptyBlock();
			/* 分配完四个盘块以后,将这四个盘块写满空目录项 */
			for(short k=0;k<64;k++){
				tempOneBlockDir[k].inodeNum=-1;
			}
			fseek(file,1024*_inode->iaddr[i],SEEK_SET);
			fwrite(tempOneBlockDir,sizeof(dirItem),64,file);
		}
		fclose(file);
	}

	/* Unix采用混合索引方式,因而对于文件的分配是一个相对复杂的过程 */
	else{
		short count=convertFileLength(fileLength); //计算需要分配给文件的盘块数

		/* 先计算此次分配所需使用的盘块数,注意Unix采用混合索引分配方式,因而计算所需盘块数时要考虑索引块*/
		int needIndexBlockNum; 

		/* 直接分配 */
		if(count<=10)
			needIndexBlockNum=0;
		/* 一次间址分配 */
		else if(count>10&&count<=512+10)
			needIndexBlockNum=1;
		/* 二次间址分配 */
		else if(count>512+10&&count<=512*512+512+10)
			needIndexBlockNum=512+1+1;
		//......
		//本系统最多到二次间址分配方式

		/* 如果系统可用空闲盘块数小于此次分配所需的盘块数目,则此次分配失败 */
		if(currentFreeBlockNum<needIndexBlockNum+count)
			return 403;

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

	/* 本次操作成功 */
	return 0;
}


/* 创建文件,需要给出文件名和文件长度 */
/* 创建目录文件的函数待会单独写,这个函数的参数列表不给出文件类型,默认就是NORMAL类文件 */
/* 
   本函数的返回值将作为本次操作的状态码传递给调用者供其参考
   500: 文件重名,操作失败
   403: 系统空间不足,操作失败
    0 : 操作成功
*/
int creatFile(char _fileName[],int _fileLength){
	/* 
	   创建文件需要先扫描一下同级目录中有没有同名文件,如果有同名的需要再判断一下这个文件是目录
	   文件还是普通文件,同一目录下允许子目录和其中的文件同名,对于NORMAL类文件如果出现重名则拒绝
	   执行文件创建操作.
	*/
	short tempLength;
	if(!strcmp(currentDirName,"/")) //本系统中,根目录的项数是640,子目录的项数都是256
		tempLength=640;
	else
		tempLength=256;
	for(short i=0;i<tempLength;i++){
		if(!strcmp(currentDIR[i].fileName,_fileName)){  //发现同名项
			/* 如果这个同名项是目录,那么允许同名 */
			if(systemiNode[currentDIR[i].inodeNum].fileType==DIRECTORY) 
				continue; //这里不能直接break,因为必须要扫描完整个目录
			/* 出现同名文件了 */
			else
				return 500; 
		} 

	}
	/* 先根据文件的字节长度计算该文件所需要占用的盘块数目 */
	short fileLength=convertFileLength(_fileLength);
	/* 文件长度如果超过剩余盘块数或者系统当前iNode已经用尽则无法再分配 */
	/* 
	   这里要注意,对于系统是否有足够盘块供文件使用,不能简单地比对系统剩余盘块数和文件所需盘块数
	   因为对于Unix来说采用增量式索引组织方式,因而计算时应考虑到索引块的存在.
	*/
	 /*一个文件需要使用的索引块数(indexBlockNum)和其自身所需盘块数(fileBlockNum)有如下的简单关系:

	   if fileBlockNum<=10 then indexBlockNum==0
	   else if fileBlockNum>10&&fileBlockNum<=512+10 then indexBlockNum==1
	   else if fileBlockNum>512+10&&fileBlockNum<512*512+512+10 the indexBlockNum==512+1+1
	   ......
	 */
	int needIndexBlockNum; 

	/* 直接分配 */
	if(fileLength<=10)
		needIndexBlockNum=0;
	/* 一次间址分配 */
	else if(fileLength>10&&fileLength<=512+10)
		needIndexBlockNum=1;
	/* 二次间址分配 */
	else if(fileLength>512+10&&fileLength<=512*512+512+10)
		needIndexBlockNum=512+1+1;
	//......
	//本系统最多到二次间址分配方式

	/* 如果系统可用空闲盘块数小于此次分配所需的盘块数目,则此次分配失败 */
	if(currentFreeBlockNum<needIndexBlockNum+count)
		return 403;

	/* 创建一个文件需要先申请iNode而后填写目录项,两个操作的顺序不能颠倒 */

	/* 申请iNode */
	/* 如何获取一个空白iNode号是一个值得考虑的问题,暂时采用线性扫描法 */
	short tempiNodeNum=-1; //这个变量用来记录分配到的iNode号
	for(short i=0;i<640;i++,tempiNodeNum++){
		if(systemiNode[i].fileLength==-1)
			break;
	}
	creatiNode(&systemiNode[tempiNodeNum],NORMAL,_fileLength,1);
	currentFreeiNodeNum--; //当前可用的iNode数量减一

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

	/* 系统文件总数加一 */
	systemFileNum++;

	/* 文件创建工作完成 */
	return 1;
}

/* 创建一个子目录 */
/* 'Linux/Unix一切皆文件',所以对于目录的创建和普通文件有些类似 同样也会因为空间或者iNode耗尽而无法创建 */
int creatDir(char _dirName[]){
	short tempLength;
	if(!strcmp(currentDirName,"/")) //本系统中,根目录的项数是640,子目录的项数都是256
		tempLength=640;
	else
		tempLength=256;

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
	currentFreeiNodeNum--; //当前可用的iNode数量减一

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


/* 删除当前目录下的子目录 */
/*
   状态码:
   403: 目标删除项下有子目录或文件,删除操作被拒绝
   404: 目标删除项未找到
   0: 目录删除成功
*/
int deleteDir(char _dirName[]){
	short tempLength;
	if(!strcmp(currentDirName,"/")) //本系统中,根目录的项数是640,子目录的项数都是256
		tempLength=640;
	else
		tempLength=256;
	for(short i=0;i<tempLength;i++){
		if(!strcmp(currentDIR[i].fileName,_dirName)){
			/* 需要判断一下被删除的目标文件是目录文件还是非目录文件,因为同级路径下允许子目录和文件同名 */

			/* 检测一下是否为目录文件 */
			if(systemiNode[currentDIR[i].inodeNum].fileType==DIRECTORY){
				/* 对于目录文件的删除,需要检测一下currentDIR[i]是否有下属文件或子目录,有的话拒绝执行删除操作 */
				/* 将currentDIR[i]的目录项加载到系统临时目录栈 */
				FILE *file=fopen(diskName,"r+");
				if(!file){
					printf("Error! Can't open the $DISK\n");
					exit(0);
				}

				/* 将四个盘块的目录项数据按照次序读入到系统临时目录栈中 */
				short tempCount=0; //临时用计数器
				for(short j=0;j<4;j++){
					fseek(file,1024*systemiNode[currentDIR[i].inodeNum].iaddr[j],SEEK_SET);
					for(short k=0;k<64;k++){
						fread(&tempDir[tempCount++],sizeof(dirItem),1,file);
					}
				}
				fclose(file);

				/* 此时,current[i]的下属目录项已经全部加载至系统临时目录栈 */
				/* 下一步工作即是遍历临时目录栈,检测栈中是否有非空项,如果有则拒绝执行删除操作 */
				for(short j=0;j<256;j++){
					if(tempDir[j].inodeNum!=-1)
						return 403;
				}

				/* 程序若能执行至此,则说明目标删除项是一个空目录,可以执行删除操作 */
				/* 删除操作需要先释放占用的盘块而后释放占用的iNode最后释放目录项 顺序不能乱*/

				/* StepI: 回收盘块 */
				for(short j=0;j<4;j++){
					recycleAnBlock(systemiNode[currentDIR[i].inodeNum].iaddr[j]);
				}

				/* StepII: 释放iNode */
				systemiNode[currentDIR[i].inodeNum].fileLength=-1;

				/* StepIII: 回收目录项 */
				currentDIR[i].inodeNum=-1;

				currentFreeBlockNum+=4; //系统空闲盘块数加四
				currentFreeiNodeNum++; //系统iNode数加一

				/* 至此,目录回收工作完成 */
				return 0;


			}
		}
	}
	/* 程序若能执行到这里,说明未找到目标项 */
	return 404;
}

/* 删除文件 */
/* 
   状态码:
   404: 指定文件未找到,删除操作失败
   0: 删除成功
*/
void deleteFile(char _fileName[]){
	short tempLength;
	if(!strcmp(currentDirName,"/")) //本系统中,根目录的项数是640,子目录的项数都是256
		tempLength=640;
	else
		tempLength=256;
	for(short i=0;i<tempLength;i++){
		if(!strcmp(currentDIR[i].fileName,_fileName)){
			/* 同级路径下允许目录名和文件名相同,因此还需要判断一下这个文件是目录文件还是非目录文件*/

			/* 
				对于目录的删除和普通文件的删除虽然类似但仍有一些较为特殊的差异,因而对于目录的删除我们
				采用另外一个专门的函数来处理,本函数只负责非目录文件的删除操作
			*/

			/* 如果不是目录文件,那么开始进行删除操作 */
			if(systemiNode[currentDIR[i].inodeNum].fileType!=DIRECTORY){
				/* Unix采用混合索引方式,因而对于盘块的回收和对其分配一样,都是相对较为复杂的操作 */

				/* 先读出该文件的长度,进而计算出它所占用的盘块数目 */
				short count=convertFileLength(systemiNode[currentDIR[i].inodeNum].fileLength);
				short j; //定义一个循环变量

				/* 直接地址块的回收 */
				for(j=0;j<10;j++,count--){
					if(count==0){
						/* 回收iNode */
						systemiNode[currentDIR[i].inodeNum].fileLength=-1;
						currentFreeiNodeNum++;
						/* 清除目录项 */
						currentDIR[i].inodeNum=-1;
						/* 系统文件总数减一 */
						systemFileNum--;
						return 0;
					}
					recycleAnBlock(systemiNode[currentDIR[i].inodeNum].iaddr[j]);
				}

				FILE *file=fopen(diskName,"r+");
				if(!file){
					printf("Error! Can't open the $DISK\n");
					exit(0);
				}

				/* 一次间址块的回收 */
				short tempStack[512]; //定义一个临时栈用来暂存从索引块中取出的索引号
				if(count>0){
					fseek(file,1024*systemiNode[currentDIR[i].inodeNum].iaddr[10],SEEK_SET);
					fread(tempStack,sizeof(short),512,file);
					for(j=0;j<512;j++,count--){
						if(count==0){
							/* 回收iNode */
							systemiNode[currentDIR[i].inodeNum].fileLength=-1;
							currentFreeiNodeNum++;
							/* 清除目录项 */
							currentDIR[i].inodeNum=-1;
							/* 系统文件总数减一 */
							systemFileNum--;
							fclose(file);
							return 0;
						}
						recycleAnBlock(tempStack[j]);
					}
				}

				/* 二次间址块的回收 */
				if(count>0){
					short innerTempStack[512]; //内层临时栈
					fseek(file,1024*systemiNode[currentDIR[i].inodeNum].iaddr[11],SEEK_SET);
					fread(tempStack,sizeof(short),512,file);
					for(j=0;j<512;j++){
						/*if(tempStack[j]==-1){
							fclose(file);
							return 0;
						}*/
						fseek(file,1024*tempStack[j],SEEK_SET);
						fread(innerTempStack,sizeof(short),512,SEEK_SET);
						for(short k=0;k<512;k++,count--){
							if(count==0){
								/* 回收iNode */
								systemiNode[currentDIR[i].inodeNum].fileLength=-1;
								currentFreeiNodeNum++;
								/* 清除目录项 */
								currentDIR[i].inodeNum=-1;
								/* 系统文件总数减一 */
								systemFileNum--;
								fclose(file);
								return 0;
							}
							allocateAnEmptyBlock(innerTempStack[k]);
						}
					}
				}

				/* 三次间址块 */

				/* PASS */
			}
		}
	}

	/* 未找到目标删除项 */
	return 404;
}

/* 打印当前目录下的信息 */
void printCurrentDirInfo(){
	short tempLength,flag=0;
	if(!strcmp(currentDirName,"/")) //本系统中,根目录的项数是640,子目录的项数都是256
		tempLength=640;
	else
		tempLength=256;
	char _fileType[10];
	for(short i=0;i<tempLength;i++){
		/*  文件名   文件长度 文件类型 */
		if(systemiNode[currentDIR[i].inodeNum].fileLength!=-1){
			/* flag用来标识当前目录是否为空目录,flag==0表示目录为空 */
			flag=1; 
			if(systemiNode[currentDIR[i].inodeNum].fileType==NORMAL){
				strcpy(_fileType,"NORMAL");
				printf("%s\t%dKB\t%s\n",currentDIR[i].fileName,systemiNode[currentDIR[i].inodeNum].fileLength,_fileType);
			}
			else{
				strcpy(_fileType,"DIRECTORY");
				printf("%s/\t\t%s\n",currentDIR[i].fileName,_fileType);
			}
		}
	}
	if(!flag)
		printf("The current directory is empty!\n");
}

/* 显示整个文件系统的统计信息 */
void printSystemInfo(){
	/*  */
}


