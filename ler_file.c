#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include <fcntl.h>

void le_fich(char *nomefich){
FILE *fr;
char user[50],pass[50];

	fr=fopen(nomefich,"r");
	if(fr==NULL){
		perror("\nErro ao abrir o ficheiro.\n");
		return;
	}
	
	while(fscanf(fr,"%s\t%s\n",user,pass)==2){
		fprintf(stderr,"\n%s\t%s\n",user,pass);
	}
return;
}

void main(){

	le_fich("users.txt");
}