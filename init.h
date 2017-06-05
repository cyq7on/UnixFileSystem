/*
	20M=20480Kb=20971520 byte.
	所以创建一个文件,向这个文件中写入20971520个字节的数据即可模拟出一个20M的磁盘
	不妨取写入的字符为'#'.
*/
#include <stdio.h>
#include <stdlib.h>

/*初始化整个文件系统,创建一个DISK文件,向其中写入20M的'#',将这个DISK文件模拟成一个磁盘*/	
void init(){
	FILE *file=fopen("DISK","w+");
	//判断文件是否打开成功,失败则退出程序
	if(!file){
		printf("Error!\n");
		exit(0);
	}
	for(int i=0;i<20971520;i++) //向DISK文件中写入20M的'#'
		fprintf(file,"%c",'#');
	fclose(file);
}