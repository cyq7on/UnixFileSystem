#include "format.h"
#include "data_structure.h"
int main(){
	init();
	groupLink();
	FILE *file=fopen(diskName,"r");
	short test[51];
	fseek(file,1024*31,SEEK_SET);
	fread(test,2,51,file);
	for(short i=0;i<=50;i++)
		printf("%d ",test[i]);
}