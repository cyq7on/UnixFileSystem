/*
	磁盘大小为20M,每个盘块大小为1KB,所有共有20480个盘块
	其中0#-30#为系统区,31#-20480#为文件区,即文件区大小为20450个盘块
	具体来说,0#为系统超级块
			 1#-20#存放文件系统iNode
			 		每个索引结点占32个字节,所以1个盘块可以存放32个索引结点
			 21#-30#存放文件系统根目录
	Unix采用成组链接法组织空闲盘块,在本系统中,每50个盘块为一组,故文件区总共被分成409组
	Unix采用增量式索引组织方式来组织文件,在索引结点中设置13个地址项,在本系统中每个地址项的长度
	为2个字节,每个盘块的大小是1KB,前10个地址项是直接地址项,因此系统中文件大小在10KB及以下的文件
	采用直接寻址方式,这些文件至多会占用10个地址块,所以在iNode地址项的前10位依次记录这些文件占用
	的盘块号,iaddr(10)这一项是一次间址项,它指出一个盘块号,在这个盘块号中存放了文件的所有盘块的盘
	块号,一个盘块号占两个byte,一个盘块大小为1KB,所以一个盘块最多可以记录512个盘块号,再加上10个直
	接地址项,所以iaddr(0)-iaddr(10)所允许的文件的最大长度为512KB+10KB,iaddr(11)这一项提供两级索引
	分配方式,它指出一个盘块号,在这个盘块中记录了512个一次间址块的盘块号,每个一次间址块中记录了512
	个盘块,所以在iaddr(11)中可以支持512*512KB=256MB的文件大小(准确些讲是256MB+512KB+10KB)
*/
#ifndef _INCLUDE_XXXXXX02_H_
#define _INCLUDE_XXXXXX02_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char byte;

/* 文件索引结点(存放于系统区1#-20#盘块上) */
/* 每个索引结点占32Byte,所以一个盘块最多可以存放32个iNode,整个系统最多可以存放640个iNode */
typedef struct iNode{
	byte fileType; //0-普通文件,1-目录文件,2-块设备,3-管道文件
	short iaddr[13]; //Unix增量式索引组织方式,一共13个地址项,每个地址项占2个字节
	int fileLength; //文件长度,以byte为单位
	byte linkCount; //文件连接计数
}INODE;

/* 文件目录项 */
/* 每个目录项占16个字节,所以一个盘块(1KB)最多可以存放64个目录项 */
typedef struct fileDirectoryEntry{
	char fileName[14]; //文件名,占14个字节
	short inodeNum; //索引结点号,占2个字节
}dirItem;

/*文件目录*/
/*struct fileDirectory{
	struct fileDirectoryEntry item[64]; //每个盘块可以存放64个目录项
};*/

/*
	明确几个问题,0#是系统超级块,超级块中存放两样东西,一个是超级盘块号栈,另一个是系统当前可供分配
	的空闲盘块的数目,供建立新文件或者目录时参考,即currentFreeBlockNum.
*/
const char diskName[]="$DISK"; //磁盘名称
const short BLOCKNUM=50; //Unix成组链接法组织空闲盘块时是将所有空闲盘块划成若干组,在本系统中每组含有50个空闲盘块
const short ENDFLAG=0; //Unix成组链接法的结束标志位
const byte NORMAL=0; //普通文件
const byte DIRECTORY=1; //目录文件
const byte BLOCKDEVICE=2; //块设备文件
const byte PIP=3; //管道文件
const short totalBlockNum=20450; //文件系统文件区总盘块数('磁盘'格式化的时候会用到这个常量)
short superStack[51]; //超级盘块号栈,采用Unix成组链接法组织空闲盘块,50个盘块为一组,superStack[0]为栈顶指针
short currentFreeBlockNum; //系统当前的文件区空闲盘块数
short currentUsingBlockNum; //当前正在使用的盘块组的第一个盘块的盘块号
INODE systemiNode[640]; //系统iNode栈,1#-20#盘块是iNode区,本文件系统至多支持640个文件
short currentFreeiNodeNum; //当前可供分配的iNode数量
/*short currentDIRNum=0; //当前的目录项号,初始状态为0*/
dirItem rootDIR[640]; //系统根目录栈,21#-30#盘块是系统根目录区,根目录下至多支持640个文件(包块子目录)
dirItem tempDIR[128]; //系统临时目录栈
dirItem *currentDIR=rootDIR; //当前的目录指针
char currentDirName[50]; //当前的目录名称
INODE *currentiNode;



#endif