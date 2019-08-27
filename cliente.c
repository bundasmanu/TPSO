#include "utils.h"

//variavel global que contem o pid associado ao cliente em questao
int pid_cli;
int fds_serv,desc_cliente;//descritores
user k;
int x;
joga jjj[18]={{0}};
int existe_jogo_fut=0;
int pedido_jogo=0;
pthread_t mostra_campos;
char *comando3[50];
WINDOW *win_campo,*win_marcadores;
int tecla;
char parametros_join[2][50]={{""}};
int red_card=0;
int marc_esq=0,marc_dir=0;
int entrada_sem_bola=0;
int depois_sem_bola=0;
int intervalos=0;
int recebido_stop=0;


void inicializa_ncurses(){

initscr();//inicializar a variavel global stdscr

    start_color();
    clear();
    cbreak();
    noecho();
    keypad(stdscr,TRUE);
    nodelay(stdscr, TRUE);

}

void define_var_ambiente(){
char *dd,*ee;
//int NDEFESAS,NAVANCADOS;

        dd=getenv("NDEFESAS");
        if(dd!=NULL){
            NDEFESAS=atoi(dd);
            printf("NDEFESAS: %d\n",NDEFESAS);
        }

        ee=getenv("NAVANCADOS");
        if(ee!=NULL){
            NAVANCADOS=atoi(ee);
            printf("NAVANCADOS: %d\n",NAVANCADOS);
        }

return;
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

    mvwprintw(win,1,11,"%d",marc_esq);//-->depois colocar aqui a estrutura resultados com o parametro resultado esquerda

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
    mvwprintw(win,1,37,"%d",marc_dir);//-->depois colocar aqui a estrutura resultados com o parametro resultado direita
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

            if((i==0||i==20)){//linha grande de cima e baixo (horizontais)
                mvwaddch(win,i,j,'-');
            }
            if((j==0 && ((i>0 && i<6)||(i>14 && i<20))) ||(j==50 && ((i>0 && i<6)||(i>14 && i<20))) || (j==25 && (i>0 && i<20))){//linhas verticais lado esquerdo e direito
                mvwaddch(win,i,j,'|');
            }
            if(j==25 && i==10){//meio campo
                mvwaddch(win,i,j,'*');
            }
            if((j==0 && i>5 && i<15)||(j==50 && i>5 && i<15)){//balizas
                mvwaddch(win,i,j,'+');
            }
            if((i==5 && j>=1 && j<=4) || (i==15 && j>=1 && j<=4) || (i==5 && j<=49 && j>=46) || (i==15 && j<=49 && j>=46)){//marcas horizontais pequena area
                mvwaddch(win,i,j,'-');
            }
            if((i>=6 && i<=14 && j==5) || (i>=6 && i<=14 && j==45)){
                mvwaddch(win,i,j,'|');
            }
        }
    }
return;
}

/**Depois ver o tracejado aqui**/
void carrega_posicao_inicial_jogadores(WINDOW *win){
int g=(NAVANCADOS+NDEFESAS+1)*2;
int i;
int w=NDEFESAS;
int ww=NAVANCADOS;
int adiciona_num_jog_def=1;
int adiciona_pos_x_def=7;
int adiciona_pos_x_avan=7;
int adiciona_num_jog_avanc=5;
init_pair(3,COLOR_RED,COLOR_GREEN);//guarda_redes
init_pair(4,COLOR_BLUE,COLOR_GREEN);//defesas
init_pair(5,COLOR_WHITE,COLOR_GREEN);//avancados


    for(i=0;i<g;i++){//guarda redes equipa 0
        if(i==0){
            if(atoi(parametros_join[0])==0 && atoi(parametros_join[1])==0) {
                mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(3)| A_BOLD| A_UNDERLINE);
            }
            else{
                mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(3) | A_BOLD);
            }
        }
        if(i==(g/2)){//guarda redes equipa 1
            if(atoi(parametros_join[0])==1 && atoi(parametros_join[1])==0){
                mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(3) | A_BOLD| A_UNDERLINE);
            }
            else{
            mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(3) | A_BOLD);
            }
        }

    }
    for(i=0;i<g;i++){
        if(i>=1 && i<=NDEFESAS){//defesas da equipa 0
            if(atoi(parametros_join[0])==0 && i==atoi(parametros_join[1])){/**Se a equipa for a 1 e o elemento do array for igual ao passado como num_j coloco o underline**/
            mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(4) | A_BOLD| A_UNDERLINE);}
            else{
                mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(4) | A_BOLD);
            }
        }
        if(i>NDEFESAS && i<(g/2)){//fazer reset aos variaveis adiciona, depois de concluido as atribuicao de posicoes e num_j da equipa 0
            adiciona_num_jog_def=1;
            adiciona_pos_x_def=7;
        }

        if(i>(g/2) && i<=(g/2)+NDEFESAS){//defesas da equipa 1
            if(atoi(parametros_join[0])==1 && i==(atoi(parametros_join[1])*2)){/**Se a equipa for a 2 e o elemento do array for igual ao passado como num_j coloco o underline**/
                mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(4) | A_BOLD|A_UNDERLINE);
            }
            else{
                mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(4) | A_BOLD);
            }
        }

    }

    for(i=0;i<g;i++){
        if(i>=(NDEFESAS+1) && i<(g/2)){//avancados equipa 0
            if(atoi(parametros_join[0])==0 && i==atoi(parametros_join[1])){
                mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(5) | A_BOLD| A_UNDERLINE);
            }
            else{
                mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(5) | A_BOLD);
            }
        }
        if(i>=(g/2) && i<(g-NAVANCADOS)){
            adiciona_num_jog_avanc=5;
            adiciona_pos_x_avan=7;
        }
        if(i>=(g-NAVANCADOS)){
            if(atoi(parametros_join[0])==1 && i==(atoi(parametros_join[1])*2)){
                mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(5) | A_BOLD| A_UNDERLINE);
            }
            else{
                mvwaddch(win,jjj[i].c.x,jjj[i].c.y,jjj[i].num_j+'0'| COLOR_PAIR(5) | A_BOLD);
            }
        }

    }

    /**ALTERACAO**/
    for(i=0;i<g;i++){
        if(jjj[i].b.possui==1){
            mvwaddch(win,jjj[i].b.co.x,jjj[i].b.co.y,'0'| COLOR_PAIR(5) | A_BOLD);
        }

    }

    return;
}

void reencaminha2(){
int g=((NDEFESAS+NAVANCADOS+1)*2);
int i;
int a,b;

    //fprintf(stderr,"\nEntrei.\n");
    read(desc_cliente,jjj,sizeof(jjj));
    //fprintf(stderr,"\n%d\n",jjj[2].num_j);
}

void reencaminha(){
int g=((NDEFESAS+NAVANCADOS+1)*2);
int i;
    //printf("\nCheguei aqui.\n");
    read(desc_cliente,&k,sizeof(user));//nao sei se vai chegar aqui bem

    if(k.rr.logado==1){
        printf("\nLogin validado.\n");

    }
    else{
        printf("\nNao Validado.\n");
    }

    fprintf(stderr,"\nLogado e jogo: %d %d\n",k.rr.logado,k.t);
    reencaminha2();

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
        if(strcmp(k.comando,"logout")==0){
            mysigval.sival_int=4;
            sigqueue(x,SIGUSR2,mysigval);
            return;
        }
        if(strcmp(comando3[0],"join")==0){
            mysigval.sival_int=5;
            sigqueue(x,SIGUSR2,mysigval);
            return;
        }

return;
}

void recebi_sinal_do_servidor(int signo, siginfo_t *info, void *extra){
//depois é aqui que deve ser feita a validacao para mostrar o campo
//vou ter de usar a estrutura sigval com o int-->para dizer quais as tarefas a serem feitas se é mostrar o jogo, ou avisar os clientes que o servidor terminou, etc

//verificacao do valor que foi passado, no envio do sinal, por parte do servidor
int verifica=0;
int g=((NDEFESAS+NAVANCADOS+1)*2);
int i;
verifica=info->si_value.sival_int;//-->contem o inteiro passado no envio do sinal

    if(verifica==1){
        fprintf(stderr,"\nRecebido sinal--> vai ser dado inicio ao jogo.\n");
        /*
        Aqui tera de ser desenhado o campo do jogo
        */
        k.t=1;//colocacao da variavel a 1
        read(desc_cliente,jjj,sizeof(jjj));
        /*for(i=0;i<g;i++){
            fprintf(stderr,"\n%d e %d\n",jjj[i].eq,jjj[i].num_j);
        }*/

    }

    if(verifica==2){
        fprintf(stderr,"\nRecebido sinal-->O servidor vai terminar.\n");
        return;
    }
    /*Recebi terminar jogo do servidor*/
    if(verifica==3){
        pthread_cancel(mostra_campos);
        k.t=0;
        endwin();
    }
    if(verifica==4){
        pthread_cancel(mostra_campos);
        red_card++;
    }
    if(verifica==5){/*Recebi golo*/
        marc_dir++;
        wclear(win_marcadores);
        desenhaMarcadores(win_marcadores);
        wrefresh(win_marcadores);
    }
    if(verifica==6){/*Recebi golo*/
        marc_esq++;
        wclear(win_marcadores);
        desenhaMarcadores(win_marcadores);
        wrefresh(win_marcadores);
    }

    /*if(verifica==7){
        intervalos++;
    }*/

    if(verifica==8){
        recebido_stop++;
        k.t=0;
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

void atualiza_posicoes_jogadores(joga *k,WINDOW *win){
int i,j;
int g=((NDEFESAS+NAVANCADOS+1)*2);
int apagado_x;
int apagado_y;

    /**1 Equipa**/
    for(i=0;i<(g/2);i++){
        if(i==0 && jjj[i].pid_cliente==pid_cli){/*guarda redes*/
            mvwaddch(win,k[i].c.x,k[i].c.y,k[i].num_j+'0'| COLOR_PAIR(3) | A_BOLD| A_UNDERLINE);
        }
        if(i==0 && jjj[i].pid_cliente!=pid_cli){/*guarda redes*/
            mvwaddch(win,k[i].c.x,k[i].c.y,k[i].num_j+'0'| COLOR_PAIR(3) | A_BOLD);
        }
        if(i>=1 && i<(1+NDEFESAS) && jjj[i].pid_cliente==pid_cli){/*Defesas*/
            mvwaddch(win,k[i].c.x,k[i].c.y,k[i].num_j+'0'| COLOR_PAIR(4) | A_BOLD| A_UNDERLINE);
        }
        if(i>=1 && i<(1+NDEFESAS) && jjj[i].pid_cliente!=pid_cli){/*Defesas*/
            mvwaddch(win,k[i].c.x,k[i].c.y,k[i].num_j+'0'| COLOR_PAIR(4) | A_BOLD);
        }
        if(i>=(1+NDEFESAS) && i<(g/2) && jjj[i].pid_cliente==pid_cli){/*Avancados*/
            mvwaddch(win,k[i].c.x,k[i].c.y,k[i].num_j+'0'| COLOR_PAIR(5) | A_BOLD| A_UNDERLINE);
        }
        if(i>=(1+NDEFESAS) && i<(g/2) && jjj[i].pid_cliente!=pid_cli){/*Avancados*/
            mvwaddch(win,k[i].c.x,k[i].c.y,k[i].num_j+'0'| COLOR_PAIR(5) | A_BOLD);
        }
    }

    /**2 Equipa**/
    for(j=(g/2);j<g;j++){
        if(j==(g/2) && jjj[j].pid_cliente==pid_cli){/*guarda redes*/
            mvwaddch(win,k[j].c.x,k[j].c.y,k[j].num_j+'0'| COLOR_PAIR(3) | A_BOLD| A_UNDERLINE);
        }
        if(j==(g/2) && jjj[j].pid_cliente!=pid_cli){/*guarda redes*/
            mvwaddch(win,k[j].c.x,k[j].c.y,k[j].num_j+'0'| COLOR_PAIR(3) | A_BOLD);
        }
        if(j>(g/2) && j<=((g/2)+NDEFESAS) && jjj[j].pid_cliente==pid_cli){/*defesas*/
            mvwaddch(win,k[j].c.x,k[j].c.y,k[j].num_j+'0'| COLOR_PAIR(4) | A_BOLD| A_UNDERLINE);
        }
        if(j>(g/2) && j<=((g/2)+NDEFESAS) && jjj[j].pid_cliente!=pid_cli){/*defesas*/
            mvwaddch(win,k[j].c.x,k[j].c.y,k[j].num_j+'0'| COLOR_PAIR(4) | A_BOLD);
        }
        if(j>((g/2)+NDEFESAS) && j<g && jjj[j].pid_cliente==pid_cli){/*avancados*/
            mvwaddch(win,k[j].c.x,k[j].c.y,k[j].num_j+'0'| COLOR_PAIR(5) | A_BOLD| A_UNDERLINE);
        }
        if(j>((g/2)+NDEFESAS) && j<g && jjj[j].pid_cliente!=pid_cli){
            mvwaddch(win,k[j].c.x,k[j].c.y,k[j].num_j+'0'| COLOR_PAIR(5) | A_BOLD);
        }
    }

    /**Bola-->alteracao**/
    for(i=0;i<g;i++){
        if(k[i].b.possui==1){/*Jogador tem a bola ou ultimo jogador que teve a bola*/
            mvwaddch(win,k[i].b.co.x,k[i].b.co.y,'o'| COLOR_PAIR(5) | A_BOLD);
        }
        if((k[i].b.possui==0 && k[i].b.co.x!=0 && k[i].b.co.y!=0) ){
            mvwaddch(win,k[i].b.co.x,k[i].b.co.y,'o'| COLOR_PAIR(5) | A_BOLD);
        }

    }


return;
}

void *thread_leitura(){
int g=((NDEFESAS+NAVANCADOS+1)*2);
int i;

do{
    /*Recebi novas posicoes*/
    read(desc_cliente,&jjj,sizeof(jjj));

    /**Atualizo posicoes**/
    wclear(win_campo);/*Necessito de apaga o que tinha senao continuava o que tava anteriormente*/
    desenhaCampo(win_campo);
    atualiza_posicoes_jogadores(jjj,win_campo);
    wrefresh(win_campo);
}while(pedido_jogo==1 && k.t==1);

pthread_cancel(mostra_campos);

}

void visualizacao_campo(){

inicializa_ncurses();
win_marcadores=newwin(4,51,0,0);
wrefresh(win_marcadores);
desenhaMarcadores(win_marcadores);
wrefresh(win_marcadores);
win_campo=newwin(22,51,3,3);//o cursor comeca na posicao (5,3)
desenhaCampo(win_campo);
wrefresh(win_campo);
carrega_posicao_inicial_jogadores(win_campo);
wrefresh(win_campo);

int x;
int res3;
char lala[50];
comand c;

/**alteracoes**/
sprintf(lala,FIFO_TECLAS,pid_cli);
mkfifo(lala,0777);
res3=open(lala,O_RDWR);
/**criacao da thread de leitura**/

pthread_create(&mostra_campos,NULL,&thread_leitura,NULL);

do{
    //write(fds_serv,&pid_cli,sizeof(pid_cli));/*Mudei isto*/
    c.key=wgetch(win_campo);
    c.pidss=getpid();
    x=write(res3,&c,sizeof(comand));/*int pa nao dar problemas*/

    if(c.key=='q' || red_card==1 || recebido_stop==1){
        delwin(win_campo);
        delwin(win_marcadores);
        endwin();
        pedido_jogo=0;
        pthread_cancel(mostra_campos);
        close(res3);
        unlink(lala);
        red_card=0;
        recebido_stop=0;
        return;
    }


}while(k.t==1 && pedido_jogo==1);
close(res3);

return;
}


int main(int argc, char *argv[]){
char nome_fifo_cliente[50];
int res2, result_res;
int ler_pid_serv;
int verif_est_serv;
char *token_2;
int x_t;


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

define_var_ambiente();

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
    x_t=0;
        printf("<\n");
        scanf(" %[^\n]",k.comando);
        //fprintf(stderr,"\n%s\n",k.comando);
        token_2=strtok(k.comando," ");
                while(token_2!=NULL){
                    comando3[x_t]=token_2;
                    x_t++;
                    token_2=strtok(NULL," ");
                }
                if(strcmp(comando3[0],"login")==0){
                                strcpy(k.nome,comando3[1]);
                                strcpy(k.pass,comando3[2]);
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
                /**comando exit**/
                if(strcmp(k.comando,"exit")==0 && k.rr.logado==1){
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
                        unlink(FIFO_TECLAS);
                        exit(EXIT_SUCCESS);
                     }
                     if(verif_est_serv==0){
                        close(desc_cliente);
                        unlink(nome_fifo_cliente);
                        unlink(FIFO_TECLAS);
                        exit(EXIT_SUCCESS);
                     }
                }
                else if(strcmp(k.comando,"exit")==0 && k.rr.logado==0){
                        close(desc_cliente);
                        unlink(nome_fifo_cliente);
                        exit(EXIT_SUCCESS);
                }

                /**comando logout**/
                if(strcmp(k.comando,"logout")==0 && k.rr.logado==1){
                    /*
                    Aqui o que tem de ser feito é enviar um sinal ao servidor com o pid do cliente
                    e do lado do servidor este tem de colocar o logado a 0
                    */
                    k.c.pids=getpid();//já tem, mas colocar outra vez nao faz mal
                    pedido_jogo=0;
                    write(fds_serv,&k,sizeof(user));
                    ativar_sinal_sigusr2();
                }
                else if(strcmp(k.comando,"logout")==0 && k.rr.logado==0){
                    fprintf(stderr,"\nNao se encontra logado, logo nao pode fazer logout.\n");
                }

                /**comando join**/
                if(strcmp(comando3[0],"join")==0 && k.rr.logado==1 && k.t==1){
                  if((atoi(comando3[1])>=0 && atoi(comando3[1])<2) && (atoi(comando3[2])>=0 && atoi(comando3[2])<(8))){
                    pedido_jogo=1;
                    /*Parametros a serem utilizados ao carregar a posicao dos jogadores, sao estragadas pelo sprintf, por isso tive de recorrer a este novo array de strings*/
                    strcpy(parametros_join[0],comando3[1]);
                    strcpy(parametros_join[1],comando3[2]);

                    sprintf(k.comando,"%s %s %s",comando3[0],comando3[1],comando3[2]);
                    comando3[0]="join";
                    write(fds_serv,&k,sizeof(user));
                    ativar_sinal_sigusr2();
                    if(k.t==1 && pedido_jogo==1){
                        visualizacao_campo();
                    }
                    //comando3[0]="";
                  }
                }
                else if(strcmp(comando3[0],"join")==0 && k.rr.logado==0){
                    fprintf(stderr,"\nPrimeiro faça login.\n");
                }
                else if(strcmp(comando3[0],"join")==0 && k.rr.logado==1 && k.t==0){
                    fprintf(stderr,"\nNao há jogos a decorrer neste momento.\n");
                }

            memset(k.comando,'\0',sizeof(k.comando));
    }

//free(jjj);
close(fds_serv);
close(desc_cliente);
unlink(nome_fifo_cliente);

return 0;
}
