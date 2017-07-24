#include "utils.h"

//variavel global que contem o pid associado ao cliente em questao
int pid_cli;
int fds_serv,desc_cliente;//descritores
user k;
int x;

void reencaminha(){
    printf("\nCheguei aqui.\n");
    read(desc_cliente,&k,sizeof(user));//nao sei se vai chegar aqui bem
    if(k.rr.logado==1){
        printf("\nLogin validado.\n");
    }
    else{
        printf("\nNao Validado.\n");
    }
return;
}


void ativar_sinal_sigusr2(){
union sigval mysigval;

        //envio do sinal ao servidor
        sigqueue(x,SIGUSR2,mysigval);
        if(strcmp(k.comando,"exit")==0){
            return;
        }
        //este read nao sei se irá fazer
        reencaminha();
return;
}

void recebi_sinal_do_servidor(){

printf("\nRecebido sinal.\n");

//depois é aqui que deve ser feita a validacao para mostrar o campo
//vou ter de usar a estrutura sigval com o int-->para dizer quais as tarefas a serem feitas se é mostrar o jogo, ou avisar os clientes que o servidor terminou, etc

return;
}


int main(int argc, char *argv[]){
char nome_fifo_cliente[50];
int res2, result_res;
int ler_pid_serv;

//preparacao do sinal SIGUSR2, que vai ser utilizado para enviar os pareceres de login ao servidor
struct sigaction sa;
sa.sa_flags=SA_SIGINFO;//para ter os tres argumentos na funcao relativa ao sinal
sa.sa_handler=SIG_DFL;//utilizar o default nao o ignorar
sa.sa_sigaction=&recebi_sinal_do_servidor;
sigemptyset(&sa.sa_mask);//inicializacao do sinal
sigaction(SIGUSR2,&sa,NULL);

//obtencao do PID do servidor, necessario para enviar os sinais
ler_pid_serv=open("pid_servidor.txt",O_RDONLY);//tenho de melhorar isto, decerto é melhor criar uma estrutura com o pid do servidor
//é so ler um numero mais nada
read(ler_pid_serv,&x,sizeof(int));
//pronto já tenho o pid do servidor, ja lhe posso enviar aquilo que pretendo


    pid_cli=getpid();//aqui esta inerente o pid deste cliente
    printf("\nPid do Cliente: %d.\n",pid_cli);
    //colocar o pid no %d do FIFO do cliente
    sprintf(nome_fifo_cliente,FIFO_CLIENTE,pid_cli);

    //criação do fifo relativo ao cliente, agora já com o pid do cliente inerente
    res2=mkfifo(nome_fifo_cliente,0777);
    if(res2==-1){
        perror("\nErro ao criar o FIFO relativo ao cliente.\n");
        exit(EXIT_FAILURE);
    }
    //correu bem a criação do FIFO relativo ao cliente
    fprintf(stderr,"\nCriado o fifo do cliente.\n");

    //abertura do FIFO do cliente
    desc_cliente=open(nome_fifo_cliente,O_RDWR);
    if(desc_cliente==-1){
        perror("\nErro na abertura do cliente, então vou desligar nao faz sentido continuar.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"\nAbertura com sucesso do FIFO do cliente.\n");

    //associacao com o servidor
    fds_serv=open(FIFO_SERVIDOR,O_WRONLY);
    if(fds_serv==-1){
        perror("\nO servidor nao se encontra ligado, logo vou desligar.\n");
        close(desc_cliente);
        unlink(nome_fifo_cliente);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"\nCliente já se encontra associado ao servidor.\n");


    while(1){


        printf("<\n");
        scanf(" %[^\n]",k.comando);
                if(strcmp(k.comando,"login")==0){
                                printf("\nIntroduza o seu user.\n");
                                scanf(" %[^\n]",k.nome);
                                printf("\nIntroduza a sua pass.\n");
                                scanf(" %[^\n]",k.pass);
                                //definicao de outros aspectos importantes, que sao importantes serem reconhecidos pelo servidor
                                k.rr.logado=0;//esta a 0 inicialmente, porque ainda nao foi validado o seu login
                                k.c.pids=pid_cli;//pid do cliente
                                printf("\nLogado: %d\tPid: %d\n",k.rr.logado,k.c.pids);
                                //envio de dados ao servidor, com user e pass do cliente
                                write(fds_serv,&k,sizeof(user));
                                printf("\nEnvio do sinal, com a info.\n");
                                ativar_sinal_sigusr2();
                                //reencaminha();
                }
                if(strcmp(k.comando,"exit")==0){
                    //quando vou embora tenho de enviar um sinal ao servidor para colocar o logado deste cliente a 0
                    //Resolver problema de quando vai um gajo embora meter o indice onde as estruturas nao tenham nada
                    //quando for embora procuro o pid do gajo e meto a estrutura dele a 0-->memset
                    //posso utilizar sempre a mesma estrutura, visto que ela apenas vai sendo atualizado no campo logado

                        write(fds_serv,&k,sizeof(user));
                        ativar_sinal_sigusr2();//-->envio a notificacao ao servidor com o pid e dps este retira-o do campo "logados"
                        printf("\n\nOkOk Vou embora\n");
                        close(desc_cliente);
                        unlink(nome_fifo_cliente);
                        exit(EXIT_SUCCESS);
                }
            memset(k.comando,'\0',sizeof(k.comando));
    }


close(fds_serv);
close(desc_cliente);
unlink(nome_fifo_cliente);

return 0;
}
