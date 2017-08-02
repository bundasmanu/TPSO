#include "utils.h"

//variavel global que contem o pid associado ao cliente em questao
int pid_cli;
int fds_serv,desc_cliente;//descritores
user k;
int x;

void inicializa_ncurses(){

initscr();//inicializar a variavel global stdscr

    start_color();
    clear();
    cbreak();
    //noecho();

}


void desenhaMarcadores(WINDOW *win) {
    int i,j;
    int x=0,k=0;
        init_pair(1,COLOR_WHITE,COLOR_BLUE);
        wattrset(win,COLOR_PAIR(1));
        scrollok(win,TRUE);
        wbkgd(win,COLOR_PAIR(1));

    for(i=0;i<3;i++){
        for(j=0;j<25;j++){
            if((i==0||i==2) && (j!=0||j!=(25-1))){
                mvwaddch(win,i,j,'-');//so na 1 e ultima linha é que se mete se '-'
            }
            if(j==0||j==(25-1)){
                 mvwaddch(win,i,j,'|');//na 1 e ultima coluna metesse '|'
            }
            if(i==1 && x==0){
                wprintw(win,"Equipa 0: ");
                x++;
            }
        }
    }

    for(i=0;i<3;i++){
        for(j=26;j<51;j++){
            if(i==0 || i==2 && (j!=26||j!=(51-1))){
                mvwaddch(win,i,j,'-');
            }
            if(j==26||j==(51-1)){
                mvwaddch(win,i,j,'|');
            }
            if(i==1 && k==0){
                wprintw(win,"Equipa 1: ");
                k++;
            }
        }
    }
    //wgetch(win);
}

void desenhaCampo(WINDOW *win){
int i,j;

    init_pair(2,COLOR_BLACK,COLOR_GREEN);
    wattrset(win,COLOR_PAIR(2));
    scrollok(win,TRUE);
    wbkgd(win,COLOR_PAIR(2));

    for(i=0;i<21;i++){
        for(j=0;j<51;j++){
                        if((i==0||i==20)){
                mvwaddch(win,i,j,'-');
            }
            if((j==0 && (i>0 && i<20)) ||(j==50 && (i>0 && i<20)) || (j==25 && (i>0 && i<20))){
                mvwaddch(win,i,j,'|');
            }
        }
    }
return;
}

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
/*melhorar aqui isto utilizar o mysigval para para os inteiros certos*/

        if(strcmp(k.comando,"exit")==0){
            mysigval.sival_int=3;
            sigqueue(x,SIGUSR2,mysigval);
            return;//se o comando for exit nao vamos para a funcao reencaminha pois nao vamos ler nada do servidor terminamos logo
        }
        if(strcmp(k.comando,"login")==0){
        mysigval.sival_int=2;
        sigqueue(x,SIGUSR2,mysigval);
        reencaminha();
        }

return;
}

void recebi_sinal_do_servidor(int signo, siginfo_t *info, void *extra){
//depois é aqui que deve ser feita a validacao para mostrar o campo
//vou ter de usar a estrutura sigval com o int-->para dizer quais as tarefas a serem feitas se é mostrar o jogo, ou avisar os clientes que o servidor terminou, etc

//verificacao do valor que foi passado, no envio do sinal, por parte do servidor
int verifica=0;
verifica=info->si_value.sival_int;//-->contem o inteiro passado no envio do sinal

    if(verifica==1){
        fprintf(stderr,"\nRecebido sinal--> vai ser dado inicio ao jogo.\n");
        /*
        Aqui tera de ser desenhado o campo do jogo
        */
        WINDOW *win_campo, *win_marcadores;
        inicializa_ncurses();
        win_marcadores=newwin(4,51,0,0);
        wrefresh(win_marcadores);
        desenhaMarcadores(win_marcadores);
        wrefresh(win_marcadores);
        win_campo=newwin(22,51,3,3);//o cursor comeca na posicao (5,3)

        desenhaCampo(win_campo);

        wrefresh(win_campo);
        wgetch(win_marcadores);//este wgetch-->depois mudar, aqui tanto pode ser win_marcadores como win_campo, mas faz sentido que seja win_campo
        endwin();
    }
    if(verifica==2){
        fprintf(stderr,"\nRecebido sinal-->O servidor vai terminar.\n");
        return;
    }
return;
}

int verifica_estado_servidor(){

if(access(FIFO_SERVIDOR,F_OK)!=0){
    //o servidor nao ta a trabalhar
    return 0;
}
return 1;
}


int main(int argc, char *argv[]){
char nome_fifo_cliente[50];
int res2, result_res;
int ler_pid_serv;
int verif_est_serv;


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
                  verif_est_serv=verifica_estado_servidor();
                     if(verif_est_serv==1){//-->caso normal: o servidor ainda corre
                        write(fds_serv,&k,sizeof(user));
                        ativar_sinal_sigusr2();//-->envio a notificacao ao servidor com o pid e dps este retira-o do campo "logados"
                        printf("\n\nOkOk Vou embora\n");
                        close(desc_cliente);
                        unlink(nome_fifo_cliente);
                        exit(EXIT_SUCCESS);
                     }
                     if(verif_est_serv==0){
                        close(desc_cliente);
                        unlink(nome_fifo_cliente);
                        exit(EXIT_SUCCESS);
                     }
                }
            memset(k.comando,'\0',sizeof(k.comando));
    }


close(fds_serv);
close(desc_cliente);
unlink(nome_fifo_cliente);

return 0;
}
