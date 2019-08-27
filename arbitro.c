#include "utils.h"
/*
Codigo relativo ao arbitro
apenas contacta com o servidor e com o cliente pa dar inicio ao jogo,
sempre que envia algo, deve ser definido um inteiro(no envio do sinal),
para o servidor saber distinguir o cliente do arbitro
*/

/*
Só depois do servidor enviar sinal que se pode dar inicio é que se dá o inicio
*/
int pid_s;
char msg[50];

void envia_ola_server(){
union sigval sig;

  if(access(FIFO_SERVIDOR,F_OK)==0){//o servidor encontra se a correr
        sig.sival_int=1;//ola
        sigqueue(pid_s,SIGUSR2,sig);
  }

}

void envia_info_server(){
union sigval sig;

  if(strcmp(msg,"termina")==0){/*envio sinal ao servidor para que o jogo acabe*/
        sig.sival_int=6;
        fprintf(stderr,"\nEnviado.\n");
        sigqueue(pid_s,SIGUSR2,sig);
  }

    if(strcmp(msg,"falta")==0){
        sig.sival_int=7;
        sigqueue(pid_s,SIGUSR2,sig);
    }

    if(strcmp(msg,"intervalo")==0){
        sig.sival_int=8;
        sigqueue(pid_s,SIGUSR2,sig);
    }

}

int main(int argc,char *argv[]){
int res,desc_arbitro,desc_s,ler_fich_pid_s;

struct sigaction sass;
sass.sa_flags=SA_SIGINFO;
sass.sa_handler=SIG_DFL;
sigemptyset(&sass.sa_mask);
sigaction(SIGUSR2,&sass,NULL);

//obtencao do pid do servidor, ele está inserido num ficheiro, que é criado quando o user executa o servidor
ler_fich_pid_s=open("pid_servidor.txt",O_RDONLY);
read(ler_fich_pid_s,&pid_s,sizeof(int));


if(access(FIFO_ARBITRO,F_OK)==0){
    fprintf(stderr,"\nJá esta um arbitro em execucao.\n");
    exit(EXIT_FAILURE);
}

res=mkfifo(FIFO_ARBITRO,0777);
if(res==-1){
    fprintf(stderr,"\nErro ao criar o FIFO do arbiitro.\n");
    exit(EXIT_FAILURE);
}

desc_arbitro=open(FIFO_ARBITRO,O_RDWR);
if(desc_arbitro==-1){
    fprintf(stderr,"\nErro ao abrir o FIFO do arbitro.\n");
    close(desc_arbitro);
    unlink(FIFO_ARBITRO);
    exit(EXIT_FAILURE);
}

desc_s=open(FIFO_SERVIDOR,O_WRONLY);
if(desc_s==-1){
    fprintf(stderr,"\nErro ao abrir o FIFO do servidor.\n");
}

envia_ola_server();

    while(1){
            printf("\n>\n");
            scanf(" %[^\n]",msg);
            if(strcmp(msg,"sair")==0){
                close(desc_arbitro);
                unlink(FIFO_ARBITRO);
                exit(EXIT_FAILURE);
            }
            if(strcmp(msg,"termina")==0){
                envia_info_server();
            }
            if(strcmp(msg,"falta")==0){
                envia_info_server();
            }
            if(strcmp(msg,"intervalo")==0){
                envia_info_server();
            }
    }

return 0;
}
