﻿#ifndef _INCLUDE_XXXXXX05_H_
#define _INCLUDE_XXXXXX05_H_

#include "data_structure.h"
#include "func.h"

/* '建立'文件界面 */
void creatFileInterface(){
	int userChoice;
	char _fileName[1000];
	int _fileLength,stateCode;
	
	while(1){
		system("cls");
		printf("\n\n\t\t\t\t建立文件\n\n");

		printf("\t\t 1.建立文件\n");
		printf("\t\t 2.返回主菜单\n\n");
		printf("\t\t请输入您要操作的序号: ");
		scanf("%d",&userChoice);
		printf("\n\n");
		if(userChoice==1){

			/* 创建文件需要给出文件名和文件长度 */
			getchar();
			printf("\t\t请输入文件名(字母数字下划线): ");
			scanf("%s",_fileName);
			while(1){
				if(fileNameFilter(_fileName)==403){
					printf("\t\t您输入的文件名非法(文件名只允许含有字母、数字或下划线，文件长度小于等于14个字符),请您重新输入: ");
					scanf("%s",_fileName);
				}
				else
					break;
			}
			
			
			//scanf("%d",&_fileLength);
			char tempInput[1000];
			while(1){
				
					RRR:
					printf("\n\t\t请输入文件长度(以Byte为单位): ");
					scanf("%s",tempInput);
					
					for(char *p=&tempInput[0];*p!='\0';p++){
						if(!(*p>='0'&&*p<='9')){
							
							printf("\t\t您的输入非法,请您看清楚是输入数字！: ");
							getchar();
							goto RRR;
						}
					}
					
					//else

				
				_fileLength=atoi(tempInput);
				
				if(_fileLength<=0){
					printf("\t\t您输入的文件长度不合法,请重新输入: ");
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
				getchar();
				continue;
			}
		}
		else if(userChoice==2)
			return;
		else{
			printf("\t\t您的输入有误!\n");
				getchar();
				getchar();
				getchar();
			return;
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
		printf("\t\t请输入您要操作的序号: ");
		scanf("%d",&userChoice);
		printf("\n\n");

		if(userChoice==1){

			/* 创建子目录只需给出子目录名称即可 */
			getchar();
			printf("\t\t请输入您要创建的目录名称(字母数字下划线): ");
			scanf("%s",_dirName);
			while(1){

				/* 子目录的命名规则和普通文件一样,因而可以与文件使用同一个过滤器 */
				if(fileNameFilter(_dirName)==403){
					printf("\t\t您输入的子目录名称非法(子目录名只允许含有字母、数字或下划线,并且长度在14位以下),请您重新输入: ");
					scanf("%s",_dirName);
				}
				else
					break;
			}

			/* 对目录名称的合法性检查完毕,下面正是开始创建工作 */
			stateCode=creatDir(_dirName);
			if(stateCode==403){
				printf("\n\t\t对不起,当前目录下已存在同名子目录,请您重新开始目录创建操作\n");
				getchar(); //空读
				getchar();
				continue;
			}
			else if(stateCode==500){
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
				getchar();
				continue;
			}

		}

		else if(userChoice==2)
			return;
		else{
			printf("\t\t您的输入有误!\n");
			getchar();
			getchar();
			return;
		}

	}
}

/* '打开文件'界面 */
void openFileInterface(){
	int userChoice;
	char _fileName[50];
	
	while(1){
		system("cls");
		printf("\n\n\t\t\t\t打开文件\n\n");

		printf("\t\t 1.打开文件\n");
		printf("\t\t 2.返回主菜单\n\n");
		printf("\t\t请输入您要操作的序号: ");
		scanf("%d",&userChoice);
		printf("\n\n");

		if(userChoice==1){

			/* 打开文件需要用户给出目标项的文件名 */
			getchar();

			/* 先当前目录下的所有文件显示出来 */
			printf("\t\t当前目录下的文件如下:\n");
			printCurrentDirInfo();


			printf("\t\t请输入您要打开的文件的名称: ");
			gets(_fileName);
			
			openFile(_fileName);
			getchar();
				

		}

		else if(userChoice==2)
			return;
		else{
			printf("\t\t您的输入有误!\n");
			getchar();
			return;
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
		printf("\t\t请输入您要操作的序号: ");
		scanf("%d",&userChoice);
		printf("\n\n");

		if(userChoice==1){

			/* 删除文件需要用户给出目标项的文件名 */
			getchar();

			/* 先当前目录下的所有文件显示出来 */
			printf("\t\t当前目录下的文件如下:\n");
			printCurrentDirInfo();


			printf("\t\t请输入您要删除的文件的名称: ");
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
					
					printf("\t\t您输入的文件名不存在,请您检查后重新输入: ");
					getchar();
					break;
			
				}
			}

		}

		else if(userChoice==2)
			return;
		else{
			printf("\t\t您的输入有误!\n");
			getchar();
			return;
		}

	}
}

/* '删除目录'界面 */
void deleteDirInterface(){
	int userChoice,stateCode;
	char _dirName[50];
	
	while(1){
		system("cls");
		printf("\n\n\t\t\t\t删除目录\n\n");

		printf("\t\t 1.删除目录\n");
		printf("\t\t 2.返回主菜单\n\n");
		printf("\t\t请输入您要操作的序号: ");
		scanf("%d",&userChoice);
		printf("\n\n");

		if(userChoice==1){

			/* 删除目录需要用户给出所要删除的目录名 */
			getchar();

			/* 先当前目录下的所有文件显示出来 */
			printf("\t\t当前目录下的文件如下:\n");
			printCurrentDirInfo();


			printf("\t\t请输入您要删除的目录名称: ");
			gets(_dirName);

			/* 过滤掉用户无端输入的换行 */
			while(1){
				if(_dirName[0]=='\0'){
					gets(_dirName);
					continue;
				}
				break;
			}

			while(1){
				stateCode=deleteDir(_dirName);
				if(stateCode==404){
					printf("\t\t您输入的目录名不存在,请您检查后重新输入: ");
					getchar();
					break;
				}
				else if(stateCode==403){
					printf("\t\t您要删除的目录下包含有文件或子目录,无法删除该目录");
					getchar();
					break;
				}
				else{
					printf("\t\t目录删除成功!\n");
					getchar();
					break;
				}
			}

		}

		else if(userChoice==2)
			return;
		else{
			printf("\t\t您的输入有误!摁下回车继续操作\n");
			getchar();
			continue;
		}

	}
}

/* '显示目录'界面 */
void printDirInterface(){

	printf("\n\n\t\t\t\t当前目录信息\n");
	//printf("\t\t----------------------------------------\n\n");

	printCurrentDirInfo();

	printf("\n\t\t摁下任意键返回主菜单");
	getchar();
	getchar();
}

/* '显示系统信息'界面 */
void printSystemInfoInterface(){
	printf("\n\n\t\t\t\t系统信息\n\n");
	printf("\t\t----------------------------------------\n\n");

	printSystemInfo();

	printf("\n\t\t----------------------------------------\n\n");
	printf("\n\t\t摁下任意键返回主菜单");
	getchar();
	getchar();
}


/* 系统主界面 */
void mainInterface(){
	
	
	int userChoice;
	while(1){
		system("cls");
		printf("\n\n\t\t\tUNIX FILE SYSTEM\n");
		printf("\t------------------------------------------------\n\n");
		printf("\t\t1. 建立文件\n");
		printf("\t\t2. 建立子目录\n");
		printf("\t\t3. 打开文件\n");
		printf("\t\t4. 删除文件\n");
		printf("\t\t5. 删除目录\n");
		printf("\t\t6. 显示目录\n");
		printf("\t\t7. 显示整个系统的信息\n");
		printf("\t\t8. 切换到子目录\n");
		printf("\t\t9. 返回到上一级目录\n");
		printf("\t\t10. 格式化磁盘\n");
		printf("\t\t11. 关机\n");
		printf("\n\t\t当前目录位置 : %s\n\n",currentDirName);
		printf("\n\t\t请输入您要操作的序号: ");
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

		/* 删除目录 */
		else if(userChoice==5){
			deleteDirInterface();
		}

		/* 显示当前目录信息 */
		else if(userChoice==6){
			system("cls");
			printDirInterface();
		}

		/* 显示系统当前信息 */
		else if(userChoice==7){
			system("cls");
			printSystemInfoInterface();
		}

		/* 切换到子目录 */
		else if(userChoice==8){
			char _childDirName[50];
			printf("\n\t\t当前目录下的文件如下: \n");
			printCurrentDirInfo();

			printf("\n\t\t请输入您要切换到的子目录名称: ");
			//gets(_childDirName);
			scanf("%s",_childDirName);
			openDir(_childDirName);
			
		}

		/* 返回上一级目录 */
		else if(userChoice==9){
			returnPreDir();
		}

		/* 格式化磁盘 */
		else if(userChoice==10){
			int isFormatChoice;
			printf("\n格式化磁盘将抹掉系统所有数据,并还原所有系统参数,您确定要继续吗？(此操作不可逆),确定请输入1 ");
			scanf("%d",&isFormatChoice);
			if(isFormatChoice==1){
				printf("\n\t\t正在格式化,请稍后...\n");
				format();
				load();
				printf("\n\t\t格式化完成!摁任意键继续");
				getchar();
				getchar();
			}
		}
		else if(userChoice==11){

			/* 这一步不是摆设,而是有相当重要的作用,如果不'关机'而直接关闭命令行窗口
			   则文件系统系统在下次启动的时候将会出错
			*/
			//printf("\n\n\t\t系统正在关机...\n\n");

			shutDown();

			printf("\n\n\t\t关机完成,摁下任意键退出\n");
			getchar();
			getchar();
			exit(0);
		}

	}
	

}



#endif
