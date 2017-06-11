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
		printf("\n\n\t\t\t\t建立文件\n\n");

		printf("\t\t 1.建立文件\n");
		printf("\t\t 2.返回主菜单\n\n");
		printf("\t\t请输入您要操作的序号:");
		scanf("%d",&userChoice);
		printf("\n\n");
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
				printf("\n\t\t对不起,当前目录下已存在同名文件,请您重新开始文件创建操作\n");
				getchar(); //空读
				getchar();
				continue;
			}
			else if(stateCode==403){
				printf("\n\t\t对不起,系统当前空间不足,文件创建失败!\n");
				getchar();
				getchar();
				continue;
			}
			else{
				printf("\n\t\t文件创建成功(摁下回车继续操作)!\n");
				getchar();
				getchar();
				printCurrentDirInfo();
				//printf("%d",currentFreeBlockNum);
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

/* '建立子目录'界面 */
void creatDirInterface(){
	int userChoice,stateCode;
	char _dirName[50];
	
	while(1){
		system("cls");
		printf("\n\n\t\t\t\t建立子目录\n\n");

		printf("\t\t 1.建立子目录\n");
		printf("\t\t 2.返回主菜单\n\n");
		printf("\t\t请输入您要操作的序号:");
		scanf("%d",&userChoice);
		printf("\n\n");

		if(userChoice==1){

			/* 创建子目录只需给出子目录名称即可 */
			getchar();
			printf("\t\t请输入您要创建的目录名称:");
			gets(_dirName);
			while(1){

				/* 子目录的命名规则和普通文件一样,因而可以与文件使用同一个过滤器 */
				if(fileNameFilter(_dirName)==403){
					printf("\t\t您输入的子目录名称非法(子目录名只允许含有字母、数字或下划线),请您重新输入:");
					gets(_dirName);
				}
				else
					break;
			}

			/* 对目录名称的合法性检查完毕,下面正是开始创建工作 */
			stateCode=creatDir(_dirName);
			if(stateCode==500){
				printf("\n\t\t对不起,当前目录下已存在同名子目录,请您重新开始目录创建操作\n");
				getchar(); //空读
				getchar();
				continue;
			}
			else if(stateCode==403){
				printf("\n\t\t对不起,系统当前空间不足,目录创建失败!\n");
				getchar();
				getchar();
				continue;
			}
			else{
				printf("\n\t\t目录创建成功(摁下回车继续操作)!\n");
				getchar();
				getchar();
				printCurrentDirInfo();
				//printf("%d",currentFreeBlockNum);
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

/* '打开文件'界面 */
void openFileInterface(){
	int userChoice,stateCode;
	char _fileName[50];
	
	while(1){
		system("cls");
		printf("\n\n\t\t\t\t打开文件\n\n");

		printf("\t\t 1.打开文件\n");
		printf("\t\t 2.返回主菜单\n\n");
		printf("\t\t请输入您要操作的序号:");
		scanf("%d",&userChoice);
		printf("\n\n");

		if(userChoice==1){

			/* 打开文件需要用户给出目标项的文件名 */
			getchar();

			/* 先当前目录下的所有文件显示出来 */
			printf("\t\t当前目录下的文件如下:\n");
			printCurrentDirInfo();


			printf("\t\t请输入您要打开的文件的名称:");
			gets(_fileName);
			//getchar();
			while(1){
				stateCode=openFile(_fileName);
				if(stateCode==0){
					getchar();
					break;
				}
				else{
					printf("\t\t您输入的文件名不存在,请您检查后重新输入:");
					gets(_fileName);
				}
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

/* '删除文件'界面 */
void deleteFileInterface(){
	int userChoice,stateCode;
	char _fileName[50];
	
	while(1){
		system("cls");
		printf("\n\n\t\t\t\t删除文件\n\n");

		printf("\t\t 1.删除文件\n");
		printf("\t\t 2.返回主菜单\n\n");
		printf("\t\t请输入您要操作的序号:");
		scanf("%d",&userChoice);
		printf("\n\n");

		if(userChoice==1){

			/* 删除文件需要用户给出目标项的文件名 */
			getchar();

			/* 先当前目录下的所有文件显示出来 */
			printf("\t\t当前目录下的文件如下:\n");
			printCurrentDirInfo();


			printf("\t\t请输入您要删除的文件的名称:");
			gets(_fileName);

			/* 过滤掉用户无端输入的回车 */
			while(1){
				if(_fileName[0]=='\0'){
					gets(_fileName);
					continue;
				}
				break;
			}
			
			while(1){
				stateCode=deleteFile(_fileName);
				if(stateCode==0){
					printf("\t\t文件删除成功!\n");
					getchar();
					break;
				}
				else{
					printf("\t\t您输入的文件名不存在,请您检查后重新输入:");
					gets(_fileName);
				}
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

		/* 建立子目录 */
		else if(userChoice==2){
			creatDirInterface();
		}

		/* 打开文件 */
		else if(userChoice==3){
			openFileInterface();
		}

		/* 删除文件 */
		else if(userChoice==4){
			deleteFileInterface();
		}

	}
	

}



#endif