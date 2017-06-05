/*
	磁盘大小为20M,每个盘块大小为1Kb,所有共有20480个盘块
	其中0#-30#为系统区,31#-20480#为文件区,即文件区大小为20450个盘块
	具体来说,0#为系统超级块
			 1#-20#存放文件系统iNode
			 21#-30#存放文件系统根目录
	Unix采用成组链接法组织空闲盘块,在本系统中,每50个盘块为一组,故文件区总共被分成409组
*/
#include <iostream>
using namespace std;
typedef unsigned char byte;

/*文件索引结点*/
struct iNode{
	byte fileType; //0-普通文件,1-目录文件,2-块设备,3-管道文件
	short iaddr[13]; //Unix混合索引方式,一共13个表项,每个表项2个字节
	int fileLength; //文件长度,以byte为单位
	byte linkCount; //文件连接计数
};

/*文件目录项*/
/*每个目录项占16个字节,所以一个盘块(1Kb)可以存放64个目录项*/
struct fileDirectory{
	char fileName[14]; //文件名,占14个字节
	short inodeNum; //索引结点号,占2个字节
};

short superStack[51]; //超级盘块号栈,采用Unix成组链接法组织空闲盘块,50个盘块为一组,superStack[0]为栈顶指针
short currentFreeBlockNum=20450; //当前可用的文件区空闲盘块数

