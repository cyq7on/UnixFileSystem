#ifndef _INCLUDE_XXXXXX05_H_
#define _INCLUDE_XXXXXX05_H_

#include "data_structure.h"
#include "func.h"

/* '建立'文件界面 */
void creatFileInterface(){
	int userChoice;
	char _fileName[50];
	int _fileLength,stateCode;
	while(1){
		system("cls");
		printf("\n\n\t\t\t建立文件\n\n");

		printf("\t\t 1.建立文件\n");
		printf("\t\t 2.返回主菜单\n\n");
		printf("\t\t请输入您要操作的序号:");
		scanf("%d",&userChoice);
		if(userChoice==1){

			/* 创建文件需要给出文件名和文件长度 */
			getchar();
			printf("\t\t请输入文件名:");
			gets(_fileName);
			while(1){
				if(fileNameFilter(_fileName)==403){
					printf("\t\t您输入的文件名非法(文件名只允许含有字母、数字或下划线),请您重新输入:");
					gets(_fileName);
				}
				else
					break;
			}

			printf("\t\t请输入文件长度(以Byte为单位):");
			scanf("%d",&_fileLength);
			while(1){
				if(_fileLength<=0){
					printf("\t\t您输入的文件长度不合法,请重新输入:");
					scanf("%d",&_fileLength);
				}
				else
					break;
			}

			/* 文件名和文件长度输入完毕,下面正式开始文件创建工作 */
			stateCode=creatFile(_fileName,_fileLength);
			if(stateCode==500){
				printf("\t\t对不起,当前目录下已存在同名文件,请您重新开始文件创建操作\n");
				getchar(); //空读
				continue;
			}
			else if(stateCode==403){
				printf("\t\t对不起,系统当前空间不足,文件创建失败!\n");
				getchar();
				continue;
			}
			else{
				printf("\t\t文件创建成功!\n");
				getchar();
				getchar();
				continue;
			}
		}
		else if(userChoice==2)
			return;
		else{
			printf("\t\t您的输入有误!\n");
			continue;
		}
					
	}
}


/* 系统主界面 */
void mainInterface(){
	

	int userChoice;
	while(1){
		system("cls");
		printf("\n\n\t\t\tUnix File System\n");
		printf("\t------------------------------------------------\n\n");
		printf("\t\t1. 建立文件\n");
		printf("\t\t2. 建立子目录\n");
		printf("\t\t3. 打开文件\n");
		printf("\t\t4. 删除文件\n");
		printf("\t\t5. 删除目录\n");
		printf("\t\t6. 显示目录\n");
		printf("\t\t7. 显示整个系统的信息\n");
		printf("\t\t8. 进入Shell模式\n");
		printf("\t\t9. 关机\n");
		printf("\n\n\t\t请输入您要操作的序号:");
		scanf("%d",&userChoice);

		/* 建立文件 */
		if(userChoice==1){
			creatFileInterface();
		}
	}
	

}



#endif