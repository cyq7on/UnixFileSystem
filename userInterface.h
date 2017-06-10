#ifndef _INCLUDE_XXXXXX05_H_
#define _INCLUDE_XXXXXX05_H_

#include "data_structure.h"
#include "func.h"

/* 系统主界面 */
void mainInterface(){
	printf("\tUnix File System\n");
	printf("\t------------------------------------------------\n\n");
	printf("\t1. 建立文件\n");
	printf("\t2. 建立子目录\n");
	printf("\t3. 打开文件\n");
	printf("\t4. 删除文件\n");
	printf("\t5. 删除目录\n");
	printf("\t6. 显示目录\n");
	printf("\t7. 显示整个系统的信息\n");
	printf("\t8. 进入Shell模式\n");
	printf("\t9. 关机\n");

	int userChoice;
	while(1){
		printf("\n\n\t请输入您要操作的序号:");
		scanf("%d",&userChoice);

		/* 建立文件 */
		if(userChoice==1){

		}
	}
	

}

/* '建立'文件界面 */
void creatFileInterface(){
	int userChoice;
	while(1){
		system("cls");
		printf("\n\t\t建立文件\n");
		printf("\t 1.建立文件\n");
		printf("\t 2.返回主菜单\n\n");
		printf("\t请输入您要操作的序号:");
		scanf("%d",&userChoice);
		if(userChoice==1){
					
	}
	/* 建立文件需要给出文件名和文件长度 */
}

#endif