#include "utils.h"

user w;
int desc_serv, desc_cli;//Descritores
int logs;
char *fich;
char fifo_pid_cli[50];
int result_res2;
//char comando[3][50]={"","",""};//necessario para o user username password
int pid_servidor;//-->pid do servidor
pthread_t valida_start;
int ha_jogo=0;
joga jj[NUM_JOG]={{0}};
int tenho_usuarios_no_jogo=0;
pthread_mutex_t mutex,mutex2;
int verifica=0;//controla threads criadas
user u[NUM_JOG]={{"","","",0,0}};
int recebi_sinals=0;
int pid_cliente;
int entra_thread;
pthread_t jogadores_campo[10],golos;/*tava no cria_threads com o ndefesas e navancados*/
int chule;
int falta=0;
int player;
int entrada=0;
int marcador_esq=0,marcador_dir=0;
int intervalo=0;
int stops=0;


void inicializa_mutexes(){

pthread_mutex_init(&mutex,NULL);
pthread_mutex_init(&mutex2,NULL);

}

/**Escreve no FIFO dos clientes logados o array referente aos users**/
void inicializacao_jogo(){
int i;
//joga *j=(joga *)args;

//fprintf(stderr,"\nEntrei.\n");

    for(i=0;i<NUM_JOG;i++){
        if(u[i].c.pids!=0){
            //fprintf(stderr,"\nChico\n");
            sprintf(fifo_pid_cli,FIFO_CLIENTE,u[i].c.pids);
            desc_cli=open(fifo_pid_cli,O_WRONLY);
            write(desc_cli,jj,sizeof(jj));
            close(desc_cli);
        }

    }
}

void carrega_posicao_inicial_jogadores(joga *jj){
int g=(NAVANCADOS+NDEFESAS+1)*2;
//jj= (jogar)malloc(sizeof(joga)* g);
int i;
int w=NDEFESAS;
int ww=NAVANCADOS;
int adiciona_num_jog_def=1;
int adiciona_pos_x_def=7;
int adiciona_pos_x_avan=7;
int adiciona_num_jog_avanc=5;

    for(i=0;i<g;i++){//guarda redes equipa 0
        if(i==0){
            jj[i].c.x=10;
            jj[i].c.y=2;
            jj[i].num_j=0;
            jj[i].eq=0;
            jj[i].pid_cliente=0;
        }
        if(i==(g/2)){//guarda redes equipa 1
            jj[i].c.x=10;
            jj[i].c.y=48;
            jj[i].num_j=0;
            jj[i].eq=1;
            jj[i].pid_cliente=0;
        }

    }
    for(i=0;i<g;i++){
        if(i>=1 && i<=NDEFESAS){//defesas da equipa 0
            jj[i].c.x=adiciona_pos_x_def;
            jj[i].c.y=8;
            jj[i].num_j=adiciona_num_jog_def;
            jj[i].eq=0;
            jj[i].pid_cliente=0;
            adiciona_num_jog_def=adiciona_num_jog_def;
            adiciona_pos_x_def=adiciona_pos_x_def;
            adiciona_num_jog_def=adiciona_num_jog_def+1;
            adiciona_pos_x_def=adiciona_pos_x_def+3;
        }
        if(i>NDEFESAS && i<(g/2)){//fazer reset aos variaveis adiciona, depois de concluido as atribuicao de posicoes e num_j da equipa 0
            adiciona_num_jog_def=1;
            adiciona_pos_x_def=7;
        }

        if(i>(g/2) && i<=(g/2)+NDEFESAS){//defesas da equipa 1
            jj[i].c.x=adiciona_pos_x_def;
            jj[i].c.y=40;
            jj[i].num_j=adiciona_num_jog_def;
            jj[i].eq=1;
            jj[i].pid_cliente=0;
            adiciona_num_jog_def=adiciona_num_jog_def;
            adiciona_pos_x_def=adiciona_pos_x_def;
            adiciona_num_jog_def=adiciona_num_jog_def+1;
            adiciona_pos_x_def=adiciona_pos_x_def+3;
        }
    }

    for(i=0;i<g;i++){
        if(i>=(NDEFESAS+1) && i<(g/2)){//avancados equipa 0
            jj[i].c.x=adiciona_pos_x_avan;
            jj[i].c.y=15;
            jj[i].num_j=adiciona_num_jog_avanc;
            jj[i].eq=0;
            jj[i].pid_cliente=0;
            adiciona_num_jog_avanc=adiciona_num_jog_avanc+1;
            adiciona_pos_x_avan=adiciona_pos_x_avan+3;
        }
        if(i>=(g/2) && i<(g-NAVANCADOS)){
            adiciona_num_jog_avanc=5;
            adiciona_pos_x_avan=7;
        }
        if(i>=(g-NAVANCADOS)){
            jj[i].c.x=adiciona_pos_x_avan;
            jj[i].c.y=31;
            jj[i].num_j=adiciona_num_jog_avanc;
            jj[i].eq=1;
            jj[i].pid_cliente=0;
            adiciona_num_jog_avanc=adiciona_num_jog_avanc+1;
            adiciona_pos_x_avan=adiciona_pos_x_avan+3;
        }

    }

    /**alteracao**/
    for(i=0;i<g;i++){
        if(NAVANCADOS>=1){
            if(i==1){/*Mudar dps isto*/
                jj[i].b.co.x=7;
                jj[i].b.co.y=9;
                jj[i].b.possui=1;
            }
            else{
                jj[i].b.co.x=0;
                jj[i].b.co.y=0;
                jj[i].b.possui=0;
            }
        }
    }

}

int verifica_login(char *nomefich){
FILE *fr;
char name[50],passwd[50];

fr=fopen(nomefich,"r");
    if(fr==NULL){
        perror("\nErro ao tentar abrir o ficheiro.\n");
        close(desc_serv);
        unlink(FIFO_SERVIDOR);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"\nAberto o Ficheiro, estamos a validar.\n");

    while(fscanf(fr,"%s\t%s",name,passwd)==2){
            if(strcmp(w.nome,name)==0){
                if(strcmp(w.pass,passwd)==0){
                        //w.rr->logado=1;//colocado no ponteiro o estado do utilizador a 1
                        return 1;
                }
            }
            //senao encontrar o nome
            fscanf(fr,"\n");//faço o \n que necessito
    }
fclose(fr);
return 0;
}

void coloca_pid_servidor_fich(char *nomefich){
int fich_serv;
pid_servidor=getpid();

    fich_serv=open(nomefich,O_CREAT | O_WRONLY,0777);
    if(fich_serv==-1){
        exit(EXIT_FAILURE);
    }
    //nao vou fazer prints, para nao sobrecarregar de texto a consola

    write(fich_serv,&pid_servidor,sizeof(int));
close(fich_serv);

return;
}

int coloca_user_ficheiro(char *nomefich, char *mat[50]){
FILE *fr;
char user[50],pass[50];

    //printf("\nEntrei.\n");
    //printf("\nMat1: %s Mat2: %s\n",mat[1],mat[2]);

    fr=fopen(nomefich,"a+");//colocar sempre no fundo
    if(fr==NULL){
        perror("\nErro ao abrir o ficheiro do usuarios.\n");
        return 0;
    }
    //else
    while(fscanf(fr,"%s\t%s\n",user,pass)==2){
        //caso já existe o user associado
        if(strcmp(user,mat[1])==0){
            fprintf(stderr,"\nJá existe esse user, altere o nome do seu user.\n");
            return 0;
        }
        //caso nao exista user associado,então
        if(strcmp(user,mat[1])!=0 && feof(fr)){//se nao encontrar e chegar ao fim do ficheiro entao agora posso escrever o user no ficheiro
            fprintf(fr,"%s\t%s\n",mat[1],mat[2]);
            fprintf(stderr,"\nUser Colocado no Ficheiro.\n");
            fclose(fr);//necessario o fclose
            return 1;
        }
    }
fclose(fr);
return 0;
}

void le(){

printf("\nRecebi pedido por parte de um cliente.\n");
        result_res2=read(desc_serv,&w,sizeof(user));
        if(result_res2 < sizeof(user)){
            fprintf(stderr,"\nResposta nao foi recebida na integra.\n");
            return;
        }
        fprintf(stderr,"\nPedido de Validacao entregue, vamos verificar.\n");

}

void envia_join_stop(){
int gg=(NDEFESAS+NAVANCADOS+1)*2;
int i;
char *b;

            union sigval sig;
            sig.sival_int=8;
            for(i=0;i<gg;i++){
                if(jj[i].pid_cliente!=0){
                    sigqueue(jj[i].pid_cliente,SIGUSR2,sig);
                }
            }

}

void recebi_sinal(int signo, siginfo_t *info, void *extra){
int i=0,kk=0;//variavel que permite controlar o numero de users ate chegar a 20-->usado no comando "users"
char *token_desc;
char *descodificar[50];
int adiciona=0;
int equipa,num_joga;
int gg=(NDEFESAS+NAVANCADOS+1)*2;
int t=0,tt=0;/*controla insercoes-->login*/
int nova_var;

switch(signo){

    case SIGUSR2:
    /**Recebi login**/
    if(info->si_value.sival_int==2){
            recebi_sinals++;
            le();
            logs=verifica_login("users.txt");
            if(logs==1){//Se existe
                    //envio da resposta ao cliente
                    //necessario saber o pid do cliente, que esta inserido na estutura do usuario
                    //basta enviar a verificar, no lado do cliente é que vai existir a validacao (se encontrou ou nao)
                    //agora vou colocar o pid no FIFO_SERVIDOR, e abri-lo para enviar a info ao cliente com aquele pid
                    printf("\nPid relativo ao cliente que enviou a mensagem: %d",w.c.pids);
                    fflush(stdout);
                    sprintf(fifo_pid_cli,FIFO_CLIENTE,w.c.pids);
                    //abro o cliente pa escrita
                    desc_cli=open(fifo_pid_cli,O_WRONLY);
                    if(desc_cli==-1){
                        perror("\nErro ao abrir o cliente.\n");
                    }
                    else{
                        fprintf(stderr,"\nAberto FIFO do cliente pa escrita.\n");

                        w.rr.logado=1;//já se encontra logado e dps envio
                        if(ha_jogo==1){
                            w.t=1;
                        }
                        if(ha_jogo==0){
                            w.t=0;
                        }

                        /**Tinha trabalhado com array dinamico, mas a criacao da thread
                        mudava a posicao de memoria do array e dps não acedia aos dados corretamente
                        entao tive de usar um array "normal"**/

                        for(i=0;i<NUM_JOG;i++){
                            if(u[i].c.pids==w.c.pids){/*se tiver feito login, dps logout e dps login outra vez*/
                                u[i].rr.logado=1;
                                tt++;/**variavel que ve se um cliente se encontra na situacao logout e faz de novo login, entao coloca apenas uma vez no array**/
                            }
                            if(u[i].c.pids==0 && u[i].rr.logado==0 && t==0 && tt==0){/**posicao do array vazia, posso fazer insercao**/
                                   t++;
                                   u[i].c.pids=w.c.pids;
                                   u[i].rr.logado=1;
                                   strcpy(u[i].nome,w.nome);
                            }
                        }

                        //ate chegar ao limite de 16 users logados-->para já nao tou a validar isso
                        //escreve na estrutura elementar o logado que depois é passado ao cliente-->no write
                        write(desc_cli,&w,sizeof(user));
                        write(desc_cli,&jj,sizeof(jj));

                        close(desc_cli);//fecho logo o descritor do cliente, dps de enviada a informacao
                        return;
                    }
            }
            else{//se nao coincide
                    sprintf(fifo_pid_cli,FIFO_CLIENTE,w.c.pids);
                    //abro o cliente pa escrita
                    desc_cli=open(fifo_pid_cli,O_WRONLY);
                    if(desc_cli==-1){
                        perror("\nErro ao abrir o cliente.\n");
                    }
                    fprintf(stderr,"\nAberto FIFO do cliente pa escrita.\n");

                    w.rr.logado=0;
                    w.t=0;

                    write(desc_cli,&w,sizeof(user));

                    close(desc_cli);//fecho logo o descritor do cliente, dps de enviada a informacao
                    return;

            }
    }
    /**Recebi um exit**/
    if(info->si_value.sival_int==3){
        le();
        //agora o que tenho de fazer é percorrer todos os clientes logados e ver onde ta aquele com o pid enviado
        //para esse cliente-->retiro-o dos logados-->ou atraves do memset ou outra forma qualquer
        fprintf(stderr,"\nw: %d\n",w.c.pids);
        //fprintf(stderr,"\nu: %d\n",u[i].c.pids);

        for(i=0;i<NUM_JOG;i++){
            if(u[i].c.pids==w.c.pids){
                /*Fazer reset dos jogadores no sistema*/
                u[i].c.pids=0;
                u[i].rr.logado=0;
                strcpy(u[i].nome,"");
            }
        }
    }

    /**Recebi ola do arbitro**/
    if(info->si_value.sival_int==1){//recebido ola do arbitro
        fprintf(stderr,"\nOla Arbitro.\n");
    }

    /**Recebi logout**/
    if(info->si_value.sival_int==4){//logout do usuario
        /*
        Aqui é necessario fazer o read do que foi escrito pelo cliente (ou seja ler o pid associado ao cliente)
        e procura-lo na matriz, e colocar o valor (logado) da matriz a 0, ou seja o comando "matriz" permite ver os users que nao estados logados,
        mas estao associados ao servidor, enquanto o "users" mostra os que estao logados
        */
        read(desc_serv,&w,sizeof(user));

        for(i=0;i<NUM_JOG;i++){
            if(w.c.pids==u[i].c.pids){
                fprintf(stderr,"\nLogout efetuado pelo user: %d",w.c.pids);
                u[i].rr.logado=0;/*Nao esta logado mas esta associado ao user, logo o pid mantem-se*/
            }
        }

    }
    /**Recebi join**/
    if(info->si_value.sival_int==5){//join
        /*
            Definir no cabecalho uma estrutura de jogadores e colocar lá o pid, num equipa seleccionada, jogador,etc
            fazer aqui em baixo os atoi's
        */
        /*
        fazer strtok do cabecalho recebido
        */
        recebi_sinals++;
        //fprintf(stderr,"\nCheguei aqui.\n");

        read(desc_serv,&w,sizeof(user));
        token_desc=strtok(w.comando," ");
        while(token_desc!=NULL){
            descodificar[adiciona]=token_desc;
            adiciona++;
            token_desc=strtok(NULL," ");
        }
        /*Agora fazer o atoi, para saber a equipa e o jogador que sao e dps posso preencher essas informacoes no jogador*/
        equipa=atoi(descodificar[1]);
        //fprintf(stderr,"\nEquipa: %d\n",equipa);
        num_joga=atoi(descodificar[2]);
        //fprintf(stderr,"\nNum_jog: %d\n",num_joga);
        for(i=0;i<gg;i++){
            if(jj[i].eq==equipa && jj[i].num_j==num_joga){
                jj[i].pid_cliente=w.c.pids;/**Coloco pid do cliente associado ao jogador**/
            }
        }
        tenho_usuarios_no_jogo++;
    }
    /**Termina jogo**/
    if(info->si_value.sival_int==6){/*Recebi termina do arbitro*/
        fprintf(stderr,"\nJogo Terminado.\n");
        union sigval sig;

        if(ha_jogo==1){
                    stops++;/*Join das Threads*/
                    ha_jogo=0;/*Controlao jogo*/
                    verifica=0;/*Criacao das Threads*/
                    envia_join_stop();
                    if(marcador_esq==marcador_dir){
                        fprintf(stderr,"\nAcabaram empatados.\n");
                    }
                    if(marcador_esq>marcador_dir){
                        fprintf(stderr,"\nGanhou a equipa 0\n");
                    }
                    if(marcador_esq<marcador_dir){
                        fprintf(stderr,"\nGanhou a equipa 1\n");
                    }
        }
    }
    /**Falta do jogador**/
    if(info->si_value.sival_int==7){

        //fprintf(stderr,"\nFalta sinal.\n");

        if(ha_jogo==1){
            for(i=0;i<gg;i++){
                if(jj[i].b.possui==1){/*Se um jogador possuir a bola imobilizo-o 5 segundos*/
                    falta=1;
                    player=jj[i].pid_cliente;
                }
            }
        }

    }

    /**Intervalo do jogo**/
    /*if(info->si_value.sival_int==8){
        union sigval sig;
        sig.sival_int=7;
        carrega_posicao_inicial_jogadores(jj);
        for(i=0;i<NUM_JOG;i++){
            if(jj[i].pid_cliente!=0){
                sprintf(fifo_pid_cli,FIFO_CLIENTE,jj[i].pid_cliente);
                desc_cli=open(fifo_pid_cli,O_WRONLY);
                write(desc_cli,&jj,sizeof(jj));
                close(desc_cli);
            }
        }
        fprintf(stderr,"\nVou agora enviar o sinal.\n");
        for(i=0;i<gg;i++){
            if(jj[i].pid_cliente!=0){
                sigqueue(jj[i].pid_cliente,SIGUSR2,sig);
            }
        }
    }*/

    break;

case SIGUSR1:

    //fazer aqui implementacao do encerramento
    close(desc_serv);
    unlink(FIFO_SERVIDOR);
    exit(EXIT_SUCCESS);

break;

case SIGALRM:

            fprintf(stderr,"\nTerminado o jogo.\n");
            ha_jogo=0;/*Para o jogo*/
            /*for(i=0;i<gg;i++){
                pthread_cancel(jogadores_campo[i]);
            }*/
break;


}//fecha switch
}

void verifica_logados(){
    int i;

printf("\nLogados:\n");
    for(i=0;i<NUM_JOG;i++){//definir tamanho de jogadores-->os que inseri - apaguei = tamanho dos users
        //percorrer todos os jogadores
        if(u[i].rr.logado==1){
            fprintf(stderr,"Pid: %d\n",u[i].c.pids);
        }

    }

return;
}

void envia_sinal_entrar_jogo(user n[NUM_JOG]){//recebe o vetor dinamico-->funcao que tem como objectivo mostrar o jogo aos clientes
int i;
union sigval sig;
sig.sival_int=1;//->quando o cliente receber o sinal vai verificar o inteiro que foi passado como parametro

//falta definir os inteiros nesta funcao e no cliente  verificar se da uniao recebeu 1 ou 2 e agir conforme o pedido
    inicializacao_jogo();


    for(i=0;i<NUM_JOG;i++){
        if(n[i].c.pids!=0){
            sigqueue(n[i].c.pids,SIGUSR2,sig);
        }
    }

    ha_jogo=1;

return;
}

void envia_sinal_sair_jogo_aos_clis(user nn[NUM_JOG]){//funcao que diz aos clientes que o servidor vai terminar
//percorrer o vetor dinamico e enviar sinal a todos os users logados
int i;
union sigval sig;
sig.sival_int=2;//-->sair do jogo tem associado

/**Envio sinal aos users que tao logados e tambem aos que estao associados ao servidor, mas nao tao logados**/
/**Users logados-->tem PID e logado==1
Users associados ao servidor, mas nao logados, tem PID e logado==0**/

    for(i=0;i<NUM_JOG;i++){
        if(u[i].c.pids!=0){
            sigqueue(nn[i].c.pids,SIGUSR2,sig);
        }
    }


return;
}

void envia_sinal_cartao_vermelho(user u[NUM_JOG],joga jj[NUM_JOG],char pal[50]){
int i,j;
union sigval sig;

//fprintf(stderr,"\nEntrei.\n");
//fprintf(stderr,"\nPal: %s\n",pal);


if(strlen(pal)>0){/**se recebi o comando com o nome**/
//fprintf(stderr,"\nEntrei.\n");
    for(i=0;i<NUM_JOG;i++){
        if(strcmp(pal,u[i].nome)==0){//se existe o user
            /**Vai ficar na situacao de**/
            for(j=0;j<NUM_JOG;j++){
                if(jj[j].pid_cliente==u[i].c.pids){
                    /*Envio sinal ao user*/
                    sig.sival_int=4;
                    jj[j].pid_cliente=0;
                    sigqueue(u[i].c.pids,SIGUSR2,sig);
                    //fprintf(stderr,"\nEnviado.\n");

                }
            }
        }
    }
}
return;
}

void envia_sigusr1_terminar(){
union sigval sig;
pid_servidor=getpid();

    /**Recebo SIGUSR1  e na funcao que recebe os sinais termino a execucao**/
    sigqueue(pid_servidor,SIGUSR1,sig);
    //Recebe SIGUSR1 e termina
return;
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

void lista_matriz(user n[NUM_JOG]){
int i;

    fprintf(stderr,"\nUsers (logados ou nao).\n");
    for(i=0;i<NUM_JOG;i++){
        if(n[i].c.pids!=0){
            fprintf(stderr,"%d\n",n[i].c.pids);
        }
    }

return;
}

void lista_players(){
int i;
int gg=((NDEFESAS+NAVANCADOS+1)*2);

    for(i=0;i<gg;i++){
        fprintf(stderr,"\n%d, %d\n",jj[i].eq,jj[i].num_j);
    }
}

/**aqui o x e o y sao os valores da posicao iterada e nao da posicao atual**/
int verifica_jogada(int x,int y,int posicao){
int i;
int g=(NDEFESAS+NAVANCADOS+1)*2;

    for(i=0;i<g;i++){
        if(posicao==0 && i==0){/*guarda redes so podem andar poucas posicoes*/
            if((x>=6 && x<=14) && (y>=2 && y<=4)){/*guarda redes da 1 equipa*/
                //fprintf(stderr,"\nredes.\n");
                return 1;/*pode andar*/
            }
        }
        if(posicao==0 && i==(g/2)){/*guarda redes da 2 equipa*/
            if((x>=6 && x<=14) && (y>=49 && y<=50)){
                return 1;
            }
        }

        if(posicao!=0 && i!=0 && i!=(g/2)){/*restantes jogadores de campo*/
            if((x>0 && x<21) && (y>0 && y<50)){
                return 1;
            }

        }
    }
return 0;
}

/**funcao que verifica se um jogador nao ultrapassa outro**/
int nao_colide_com_jogador(int x, int y){
int i;
int gg=((NDEFESAS+NAVANCADOS+1)*2);

    for(i=0;i<gg;i++){
        if(x==jj[i].c.x && y==jj[i].c.y){/*jogador*/
            return 0;
        }
    }
return 1;
}

/**Depois tenho de fazer para a bola**/
int bola_nao_colide_jogador(int x, int y){/*O x e o y são a posicao iterada da bola*/
int i;
int gg=((NDEFESAS+NAVANCADOS+1)*2);

    for(i=0;i<gg;i++){
        if(x==jj[i].c.x && y==jj[i].c.y){
            return 0;
        }
    }
return 1;
}

/**Funcao a ser utilizada nas threads de jogo, com o intuito de verificar se posso clicar num botao e passar ao jogador, se nao existe o jogador nem entra no case**/
int verifica_jogador_existe(int origem,int eq){
int i;
int gg=((NDEFESAS+NAVANCADOS+1)*2);

    for(i=0;i<gg;i++){
        if(jj[i].num_j==origem && jj[i].eq==eq){
            return 1;
        }
    }
return 0;
}

void passar_a_bola(int origem,int eq,int dest){
/*
1-Origem jogador que faz passe e destino que recebe a bola
2-Verificar posicao do jogador origem, para criar precisao de remate
3-Se sim, entao começo a criar a situacao de passe:
3.1- O passe tem uma eficácia:
    .25% guarda redes;   .80% defesas;       .60% avancados;
3.2- Se detetar um jogador pelo caminho para no sítio onde ta o jogador
3.3- A velocidade deve ser a mesma do jogador que a possui
*/
int i,j,k;
int gg=((NDEFESAS+NAVANCADOS+1)*2);
int num_aleatorio;
int vetor[2];
int valida_vetor=0;/*variavel que ve se o vetor é valido no inicio ou nao*/
int res;


//fprintf(stderr,"\nChegada passe da bola.\n");

    for(i=0;i<gg;i++){
        if(jj[i].num_j==origem && jj[i].eq==eq && jj[i].b.possui==1){/*Descoberto o jogador origem e verificacao se este possui a bola*/
                for(j=0;j<gg;j++){
                        if(jj[j].num_j==dest && jj[j].eq==jj[i].eq){/*Obtencao do vetor entre os dois pontos-->vetor entre jogadores da mesma equipa*/
                            vetor[0]=jj[j].c.x-jj[i].c.x;
                            vetor[1]=jj[j].c.y-jj[i].c.y;
                        }
                }

            do{

                    if(vetor[0]>0){
                        jj[i].b.co.x++;
                        vetor[0]--;
                    }
                    if(vetor[0]<0){
                        jj[i].b.co.x--;
                        vetor[0]++;
                    }
                    if(vetor[1]>0){
                        jj[i].b.co.y++;
                        vetor[1]--;
                    }
                    if(vetor[1]<0){
                        jj[i].b.co.y--;
                        vetor[1]++;
                    }

                /**jj[i]-->orig portador da bola e o jj[k] sao todos os jogadores presentes no campo se tocar na adjacencia dele a bola fica a pertencer-lhe**/
                /*Nova posicao-->verificar se toca num jogador, se tocar paro logo e esse novo jogador é que fica com a posse da bola*/
                for(k=0;k<gg;k++){
                    if(jj[k].num_j!=jj[i].num_j || jj[i].eq!=jj[k].eq){
                        //fprintf(stderr,"\nChegada passe da bola.\n");
                        if( ((jj[i].b.co.x-jj[k].c.x==-1 || jj[i].b.co.x-jj[k].c.x==1) && (jj[i].b.co.y-jj[k].c.y==0)) || ((jj[i].b.co.y-jj[k].c.y==-1 || jj[i].b.co.y-jj[k].c.y==1) && (jj[i].b.co.x-jj[k].c.x==0)) || ((jj[i].b.co.x-jj[k].c.x==0) && (jj[i].b.co.y-jj[k].c.y==0)) ){
                            jj[i].b.possui=0;
                            jj[k].b.possui=1;
                            /*Nova posicao da bola*/
                            if(jj[i].eq==0){/**Para ficar virado para a baliza-->equipa 0**/
                                jj[k].b.co.x=jj[i].b.co.x;
                                jj[k].b.co.y=jj[i].b.co.y;
                            }
                            else{/**Para ficar virado para a baliza-->equipa 1**/
                                jj[k].b.co.x=jj[i].b.co.x;
                                jj[k].b.co.y=jj[i].b.co.y;
                            }
                            jj[i].b.co.x=0;
                            jj[i].b.co.y=0;
                            //fprintf(stderr,"\nEntrei.\n");
                            return;
                        }
                    }
                }
                //usleep(300000);
            }while(1);
        }
    }
return;
}

void vetor_jogador_baliza(int pos_bali_x,int pos_bali_y,int pid){/*Pid para nao coincidir com o jogador que rematou a bola*/
int i,j;
int gg=((NDEFESAS+NAVANCADOS+1)*2);
int vetor[2];


    for(i=0;i<gg;i++){
        if(jj[i].pid_cliente==pid){
            vetor[0]=pos_bali_x-jj[i].b.co.x;
            vetor[1]=pos_bali_y-jj[i].b.co.y;

//fprintf(stderr,"\nEntrada na funcao vetor_baliza  vetor[0] e vetor[1] %d %d.\n",vetor[0],vetor[1]);

    do{
    //fprintf(stderr,"\nEntrada.\n");
        if(vetor[0]>0){
            jj[i].b.co.x++;
            vetor[0]--;
        }
        if(vetor[0]<0){
            jj[i].b.co.x--;
            vetor[0]++;
        }
        if(vetor[1]>0){
            jj[i].b.co.y++;
            vetor[1]--;
        }
        if(vetor[1]<0){
            jj[i].b.co.y--;
            vetor[1]++;
        }

        /**Alteracao**/
        //fprintf(stderr,"\nPosicao atual vetor: %d %d\n",jj[i].b.co.x,jj[i].b.co.y);


    //fprintf(stderr,"\nPUM.\n");
        for(j=0;j<gg;j++){
            if(jj[j].num_j!=jj[i].num_j || jj[i].eq!=jj[j].eq){
                if( (jj[i].b.co.x-jj[j].c.x==0 && (jj[i].b.co.y-jj[j].c.y==-1 || jj[i].b.co.y-jj[j].c.y==1)) || (jj[i].b.co.y-jj[j].c.y==0 && (jj[i].b.co.x-jj[j].c.x==-1 || jj[i].b.co.x-jj[j].c.x==1))){
                    //fprintf(stderr,"\nReturn pid pid : %d %d.\n",pid,jj[i].pid_cliente);
                            jj[i].b.possui=0;
                            jj[j].b.possui=1;
                            jj[j].b.co.x=jj[i].b.co.x;
                            jj[j].b.co.y=jj[i].b.co.y;
                            jj[i].b.co.x=0;
                            jj[i].b.co.y=0;
                            return;
                        }
            }
        }

        if(vetor[0]==0 && vetor[1]==0){
            //fprintf(stderr,"\nSai\n");
            /**Foi golo e na thread de validar golos coloca a bola no redes adversário**/
            return;
        }


    }while(1);/**Ate chegar a baliza**/
}
}
}

void remate_baliza(int origem,int eq){
/*
1-Verificar se o jogador possui a bola, se possuir
2- Entao sorteio 1 numero entre 0 e 8-->se for 0 posicao 6 da baliza e 8 14--> as balizas estao na coluna 0 linhas 6 a 14 e coluna 50 linhas 6 a 14
3-Coloco la a bola
4-Se tocar num jogador entretanto paro, se nao é golo
*/
int i,j;
int gg=((NDEFESAS+NAVANCADOS+1)*2);
int numero_sorteado;

    for(i=0;i<gg;i++){
        if(jj[i].num_j==origem && jj[i].eq==eq){/**Encontrado jogador**/
            if(jj[i].b.possui==1){/**Se possui a bola**/
                /*Sorteio numero*/
                srand((unsigned)time(NULL));
                numero_sorteado=rand()%9;
                //fprintf(stderr,"\nAntes: %d %d\n",jj[i].b.co.x,jj[i].b.co.y);
                    if(numero_sorteado==0 && eq==0){
                        vetor_jogador_baliza(6,50,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==0 && eq==1){
                        vetor_jogador_baliza(6,1,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==1 && eq==0){
                        vetor_jogador_baliza(7,50,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==1 && eq==1){
                        vetor_jogador_baliza(7,1,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==2 && eq==0){
                        vetor_jogador_baliza(8,50,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==2 && eq==1){
                        vetor_jogador_baliza(8,1,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==3 && eq==0){
                        vetor_jogador_baliza(9,50,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==3 && eq==1){
                        vetor_jogador_baliza(9,1,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==4 && eq==0){
                        vetor_jogador_baliza(10,48,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==4 && eq==1){
                        vetor_jogador_baliza(10,1,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==5 && eq==0){
                        vetor_jogador_baliza(11,50,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==5 && eq==1){
                        vetor_jogador_baliza(11,1,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==6 && eq==0){
                        vetor_jogador_baliza(12,50,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==6 && eq==1){
                        vetor_jogador_baliza(12,1,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==7 && eq==0){
                        vetor_jogador_baliza(13,50,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==7 && eq==1){
                        vetor_jogador_baliza(13,1,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==8 && eq==0){
                        vetor_jogador_baliza(14,50,jj[i].pid_cliente);
                    }
                    if(numero_sorteado==8 && eq==1){
                        vetor_jogador_baliza(14,1,jj[i].pid_cliente);
                    }

               //fprintf(stderr,"\nDepois: %d %d\n",jj[i].b.co.x,jj[i].b.co.y);

            }
            else{
                return;
            }
        }
    }

}

int verifica_adjacencias_e_malhas(int origem,int eq){
int i,j;
int gg=((NDEFESAS+NAVANCADOS+1)*2);

for(i=0;i<gg;i++){
    if(jj[i].num_j==origem && jj[i].eq==eq){
                    //fprintf(stderr,"\nAdjacencias.\n");
                    /*Verificacao se toca na adjacencia em algum jogador*/
                    for(j=0;j<gg;j++){
                        if( (jj[i].num_j!=jj[j].num_j || jj[i].eq!=jj[j].eq) ){
                            if( jj[i].b.co.x==0 && (jj[i].b.co.y-jj[j].c.y==-1||jj[i].b.co.y-jj[j].c.y==1) || jj[i].b.co.y==0 && (jj[i].b.co.x-jj[j].c.x==-1 || jj[i].b.co.x-jj[j].c.x==1)){
                                  jj[i].b.possui=0;
                                  jj[j].b.possui=1;
                                  jj[j].b.co.y=jj[i].b.co.y;
                                  jj[j].b.co.x=jj[i].b.co.x;
                                  jj[i].b.co.x=0;
                                  jj[i].b.co.y=0;
                                  //fprintf(stderr,"\n1\n");
                                  return 1;
                            }
                            if((jj[i].b.co.y-jj[j].c.y==0 && jj[i].b.co.x-jj[j].c.x==0)){
                                  jj[i].b.possui=0;
                                  jj[j].b.possui=1;
                                  jj[j].b.co.y=jj[i].b.co.y;
                                  jj[j].b.co.x=jj[i].b.co.x;
                                  jj[i].b.co.x=0;
                                  jj[i].b.co.y=0;
                                 //fprintf(stderr,"\n2\n");
                                return 1;
                            }
                        }
                    }
                        /*Verificacao se toca em alguma malha*/
                        /**Ver malhas valores, podem nao estar corretos**/
                        if(jj[i].b.co.x==1 || jj[i].b.co.x==19 || jj[i].b.co.y==1 || jj[i].b.co.y==50){
                            jj[i].b.possui=0;
                            //fprintf(stderr,"\n3\n");
                            return 1;
                        }
    }
}
return 0;
}

void remate_aleatorio(int origem,int eq){
int i,j;
int primeiro_sorteio,segundo_sorteio;
int gg=((NDEFESAS+NAVANCADOS+1)*2);
int toca;
/*
1-sortear 1 numero em que 0-horizontal, 1-vertical, 2 diagonal
2-sortear 2 numero que define a orientacao do vetor mediante a direccao que foi sorteado atras, ou seja um remate na horizontal pode ir no sentido da baliza adversaria ou passe para tras
3-Mediante isso mover a bola até atingir um jogador-->"aí para a bola", ou tocar nas malhas laterais*/

//srand((unsigned)time(NULL));
primeiro_sorteio=rand()%3;
//fprintf(stderr,"\nNumero sorteado: %d\n",primeiro_sorteio);

for(i=0;i<gg;i++){
    if(jj[i].num_j==origem && jj[i].eq==eq){
        if(primeiro_sorteio==0){/*Horizontal*/
            if(jj[i].num_j!=0){/*Se nao for guarda redes posso chutar em ambos os sentidos na horizontal*/
                //srand((unsigned)time(NULL));
                segundo_sorteio=rand()%1;
                if(segundo_sorteio==0){/*Baliza adversaria*/
                    do{
                        if(jj[i].eq==0){
                            jj[i].b.co.y++;
                        }
                        else{
                            jj[i].b.co.y--;
                        }

                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            return;
                        }
                    }while(1);
                }
                if(segundo_sorteio==1){/*Passe para tras*/
                    do{
                        if(jj[i].eq==0){
                            jj[i].b.co.y--;
                        }
                        else{
                            jj[i].b.co.y++;
                        }

                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            return;
                        }
                    }while(1);
                }
            }
            /**Se for guarda redes apenas posso chutar para a baliza adversaria**/
            if(jj[i].num_j==0){
                    do{
                        if(jj[i].eq==0){/*Guarda redes equipa 0*/
                            jj[i].b.co.y++;
                        }
                        if(jj[i].eq==1){/*Guarda redes equipa 1*/
                            jj[i].b.co.y--;
                        }

                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            return;
                        }
                    }while(1);
            }
        }
        if(primeiro_sorteio==1){/*Vertical*/
                //srand((unsigned)time(NULL));
                segundo_sorteio=rand()%1;

                if(segundo_sorteio==0){/*Para cima*/
                    do{

                        jj[i].b.co.x--;

                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            return;
                        }
                    }while(1);
                }
                else{/*Para baixo*/
                    do{
                        jj[i].b.co.x++;

                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            return;
                        }
                    }while(1);
                }
        }
        if(primeiro_sorteio==2){/*Diagonal*/
            //srand((unsigned)time(NULL));
            segundo_sorteio=rand()%4;

            if(jj[i].eq==0 && (segundo_sorteio==2 || segundo_sorteio==3)){/*Nao chuta para a propria baliza-->o guarda redes*/
                if(segundo_sorteio==2){/*Para cima e para o lado*/
                    do{
                        jj[i].b.co.x--;
                        jj[i].b.co.y++;
                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                                //jj[i].b.co.y++;/*Ficar com a bola pa frente*/
                                return;
                        }
                    }while(1);
                }
                if(segundo_sorteio==3){
                    do{
                        jj[i].b.co.x++;
                        jj[i].b.co.y++;
                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            //jj[i].b.co.y++;/*Ficar com a bola pa frente*/
                            return;
                        }
                    }while(1);
                }
            }
            if(jj[i].eq==0 && jj[i].num_j!=0 && (segundo_sorteio==0 || segundo_sorteio==1)){
                if(segundo_sorteio==0){
                    do{
                        jj[i].b.co.x--;
                        jj[i].b.co.y--;
                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            //jj[i].b.co.y++;/*Ficar com a bola pa frente*/
                            return;
                        }
                    }while(1);
                }
                if(segundo_sorteio==1){
                    do{
                        jj[i].b.co.x++;
                        jj[i].b.co.y--;
                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            //jj[i].b.co.y++;/*Ficar com a bola pa frente*/
                            return;
                        }
                    }while(1);
                }
            }
            if(jj[i].eq==1 && (segundo_sorteio==2 || segundo_sorteio==3)){/*Nao chuta para a propria baliza-->o guarda redes*/
                if(segundo_sorteio==2){/*Para cima e para o lado*/
                    do{
                        jj[i].b.co.x--;
                        jj[i].b.co.y--;
                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                                //jj[i].b.co.y++;/*Ficar com a bola pa frente*/
                                return;
                        }
                    }while(1);
                }
                if(segundo_sorteio==3){
                    do{
                        jj[i].b.co.x++;
                        jj[i].b.co.y--;
                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            //jj[i].b.co.y++;/*Ficar com a bola pa frente*/
                            return;
                        }
                    }while(1);
                }
            }
            if(jj[i].eq==1 && jj[i].num_j!=0 && (segundo_sorteio==0 || segundo_sorteio==1)){
                if(segundo_sorteio==0){
                    do{
                        jj[i].b.co.x--;
                        jj[i].b.co.y++;
                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            //jj[i].b.co.y++;/*Ficar com a bola pa frente*/
                            return;
                        }
                    }while(1);
                }
                if(segundo_sorteio==1){
                    do{
                        jj[i].b.co.x++;
                        jj[i].b.co.y++;
                        /*Chamada da funcao que verifica as malhas e se toca na adjacencia dos jogadores;*/
                        //fprintf(stderr,"\nVou entrar.\n");
                        toca=verifica_adjacencias_e_malhas(jj[i].num_j,jj[i].eq);
                        if(toca==1){
                            //jj[i].b.co.y++;/*Ficar com a bola pa frente*/
                            return;
                        }
                    }while(1);
                }
            }


        }

    }
}
}

/**Verifica se um jogador tem ou nao a bola na sua posse**/
int tem_ou_nao_bola(int num_j,int eq){
int i;
int g=(NDEFESAS+NAVANCADOS+1)*2;

    for(i=0;i<g;i++){
        if(jj[i].num_j==num_j && jj[i].eq==eq){
            if(jj[i].b.possui==0){
                return 0;
            }
            else{
                return 1;
            }
        }
    }
}

/**Funcao que esta sempre a ver quem tem a bola no momento, importante para quando a bola nao tem dono e outra pessoa a retem**/
void capturas_bola(int origem,int eq){
int i,j;
int gg=((NDEFESAS+NAVANCADOS+1)*2);
int alguem_bola=0;
int res;

//fprintf(stderr,"\nCaptura bolas.\n");

    for(i=0;i<gg;i++){
        if(jj[i].b.possui==1){
            alguem_bola++;
        }
    }

    if(alguem_bola==0){
        for(i=0;i<gg;i++){
            if(jj[i].num_j==origem && jj[i].eq==eq){
                //fprintf(stderr,"\nEncontrado player.\n");
                for(j=0;j<gg;j++){
                    if(jj[j].b.co.x!=0 && jj[j].b.co.y!=0 && jj[j].b.possui==0 && (jj[i].num_j!=jj[j].num_j || jj[i].eq!=eq)){/*Diferente pessoa resgata bola*/
                        //fprintf(stderr,"j x e y: %d %d\n",jj[j].b.co.x,jj[j].b.co.y);
                        if((jj[j].b.co.x-jj[i].c.x==0 && (jj[j].b.co.y-jj[i].c.y==-1 || jj[j].b.co.y-jj[i].c.y==1)) || (jj[j].b.co.y-jj[i].c.y==0 && (jj[j].b.co.x-jj[i].c.x==-1 || jj[j].b.co.x-jj[i].c.x==1))){
                                //fprintf(stderr,"\nVerificacaoo.\n");
                                //fprintf(stderr,"\nValor de x e y: %d %d\n",jj[j].b.co.x,jj[j].b.co.y);
                                jj[i].b.possui=1;
                                jj[i].b.co.x=jj[j].b.co.x;
                                jj[i].b.co.y=jj[j].b.co.y;
                                jj[j].b.co.x=0;
                                jj[j].b.co.y=0;

                        }
                    }
                    if(jj[j].b.co.x!=0 && jj[j].b.co.y!=0 && jj[j].b.possui==0 && (jj[i].num_j==jj[j].num_j && jj[i].eq==eq)){/*Perde a bola mas quem a captura é a msm pessoa*/
                        //fprintf(stderr,"j x e y: %d %d\n",jj[j].b.co.x,jj[j].b.co.y);
                        if((jj[j].b.co.x-jj[i].c.x==0 && (jj[j].b.co.y-jj[i].c.y==-1 || jj[j].b.co.y-jj[i].c.y==1)) || (jj[j].b.co.y-jj[i].c.y==0 && (jj[j].b.co.x-jj[i].c.x==-1 || jj[j].b.co.x-jj[i].c.x==1))){
                                //fprintf(stderr,"\nVerificacaoo.\n");
                                //fprintf(stderr,"\nValor de x e y: %d %d\n",jj[j].b.co.x,jj[j].b.co.y);
                                jj[i].b.possui=1;
                                jj[i].b.co.x=jj[j].b.co.x;
                                jj[i].b.co.y=jj[j].b.co.y;
                        }
                }
        }
    }
}
}
}

void ultimo_adquire_a_bola(int origem,int eq){
/*
1-se o jogador recebido possui a bola, e se esta toca na adjacencia de um qualquer jogador no terreno, entao este ultimo adquire a bola
2-se o jogador nao tem a bola mas toca na bola entao esta pertence-lhe agora
3-Um jogador rouba a bola ao jogador, o outro jogador nao se move, este nao volta a ficar com a bola(nao fazia sentido), tem de se deslocar e ir tocar na adjacencia da bola outra vez-->a variavel aqui faz isso
4-Variavel aqui começa a 0 se um jogador rouba a bola a este, aqui passa a 1, e para evitar a situacao de nao se mexer o jogador, so dps de se mexer o jogador e voltar a tocar na bola(visto que entra no if, pois saimos do programa e aqui esta a 0), entao este fica na posse da bola
*/

/**Problema aqui sobrepoem-se, quando nao se deixa parado o jogador**/

int i,j;
int gg=((NDEFESAS+NAVANCADOS+1)*2);
int aqui=0;/*Variavel que permite que quando um jogador rouba a bola e o jogador a quem a bola foi roubada se ficar na msm posicao entao nao volte a possui-la, tem de sair de la e voltar a tar perto da bola*/

//fprintf(stderr,"\nUltimo adquire a bola.\n");

    for(i=0;i<gg;i++){
        if(jj[i].num_j==origem && jj[i].eq==eq){/*Jogador recebido como parametro*/
            for(j=0;j<gg;j++){
                if(jj[i].b.possui==1 && (jj[j].eq!=jj[i].eq || jj[j].num_j!=jj[i].num_j)){/*Caso tenha a bola o jogador e alguem a roube*/
                    if((jj[i].b.co.x-jj[j].c.x==0 && (jj[i].b.co.y-jj[j].c.y==-1 || jj[i].b.co.y-jj[j].c.y==1)) || (jj[i].b.co.y-jj[j].c.y==0 && (jj[i].b.co.x-jj[j].c.x==1 ||jj[i].b.co.x-jj[j].c.x==-1))){
                        //fprintf(stderr,"\nEntrada.\n");
                        jj[i].b.possui=0;
                        jj[j].b.possui=1;
                        jj[j].b.co.x=jj[i].b.co.x;
                        jj[j].b.co.y=jj[i].b.co.y;
                        jj[i].b.co.x=0;
                        jj[i].b.co.y=0;
                        aqui++;
                    }
                }
                if((jj[i].b.possui==0 && jj[j].b.possui==1) && jj[j].pid_cliente!=jj[i].pid_cliente && aqui==0){/*Caso este a roube*/
                    if((jj[j].b.co.x-jj[i].c.x==0 && (jj[j].b.co.y-jj[i].c.y==-1 || jj[j].b.co.y-jj[i].c.y==1)) || (jj[j].b.co.y-jj[i].c.y==0 && (jj[j].b.co.x-jj[i].c.x==1 ||jj[j].b.co.x-jj[i].c.x==-1))){
                        //fprintf(stderr,"\nEntrada2.\n");
                        jj[i].b.possui=1;
                        jj[j].b.possui=0;
                        jj[i].b.co.x=jj[j].b.co.x;
                        jj[i].b.co.y=jj[j].b.co.y;
                        jj[j].b.co.x=0;
                        jj[j].b.co.y=0;
                    }
                }
            }
        }
    }
}

void envia_sinal_golo(int baliza){
union sigval sig;
int gg=((NDEFESAS+NAVANCADOS+1)*2);
int i;

    if(baliza==0){
        sig.sival_int=5;
        for(i=0;i<gg;i++){
            if(jj[i].pid_cliente!=0){
                sigqueue(jj[i].pid_cliente,SIGUSR2,sig);
            }
        }
    }
    if(baliza==1){
        sig.sival_int=6;
        for(i=0;i<gg;i++){
            if(jj[i].pid_cliente!=0){
            //fprintf(stderr,"\nEntrada.\n");
                sigqueue(jj[i].pid_cliente,SIGUSR2,sig);
            }
        }
    }
return;
}

/**Aqui é necessario ver se o jogador tem a bola e se tiver**/
void jogador_aleatorio(int origem,int eq){
/*
1-Verifico qual a posicao do jogador a ser analisado
2-Depois verifico se este tem bola ou nao
2.1-Se tem bola chamo a funcao remate aleatorio quando erra, ou gero numero aleatorio para passar a bola a um colega de equipa
2.2- Senao possuir a bola:
2.2.1-Gero numero aleatorio
2.2.2-25%anda pa direita, 25%para a esquerda, 25%cima,25%baixo
2.2.3-Mas o guarda redes nao pode passar a grande area e os defesas nao podem avancar o meio campo
*/
int i,j;
int gg=((NDEFESAS+NAVANCADOS+1)*2);
int primeiro_sorteio,segundo_sorteio,terceiro_sorteio;
int sorteia_movimento;
int novas_p_x,novas_p_y;
int resultado,coincide;


    for(i=0;i<gg;i++){
        if(jj[i].num_j==origem && jj[i].eq==eq){/*Encontrado jogador*/
            if(jj[i].b.possui==1){/*Se este possuir a bola*/
                //srand((unsigned)time(NULL));
                primeiro_sorteio=rand()%101;
                if(jj[i].num_j==0 && primeiro_sorteio<=25){/*Guarda redes so coloca bem a bola 25%*/
                        //srand((unsigned)time(NULL));
                        segundo_sorteio=rand()%2;/**0-defesas e 1-avancados**/
                        //fprintf(stderr,"\nSegundo sorteio: %d.\n",segundo_sorteio);
                        if(segundo_sorteio==0){
                            //srand((unsigned)time(NULL));
                            terceiro_sorteio=rand()%NDEFESAS;/**0-defesas e 1-avancados**/
                            //fprintf(stderr,"\nTerceiro sorteio: %d.\n",terceiro_sorteio);
                            passar_a_bola(jj[i].num_j,jj[i].eq,1+terceiro_sorteio);
                        }
                        if(segundo_sorteio==1){
                            //srand((unsigned)time(NULL));
                            terceiro_sorteio=rand()%NAVANCADOS;/**0-defesas e 1-avancados**/
                            //fprintf(stderr,"\nTerceiro sorteio: %d.\n",terceiro_sorteio);
                            passar_a_bola(jj[i].num_j,jj[i].eq,1+4+terceiro_sorteio);//numero de defesas que poderao existir
                        }
                }
                if(jj[i].num_j==0 && primeiro_sorteio>25){/*Se passa mal->remate aleatorio*/
                    remate_aleatorio(jj[i].num_j,jj[i].eq);
                }
                if(jj[i].num_j>0 && jj[i].num_j<=NDEFESAS && primeiro_sorteio<=80){/*Defesas colocam bem a bola*/
                    //srand((unsigned)time(NULL));
                    segundo_sorteio=rand()%3;/**0-guarda redes 1-defesas e 2-avancados**/
                    if(segundo_sorteio==0){
                        passar_a_bola(jj[i].num_j,jj[i].eq,0);/*Passe ao guarda redes*/
                    }
                    if(segundo_sorteio==1){
                        do{
                            //srand((unsigned)time(NULL));
                            terceiro_sorteio=rand()%NDEFESAS;
                        }while(terceiro_sorteio+1==jj[i].num_j);/*Para nao passar ao proprio jogador*/
                        passar_a_bola(jj[i].num_j,jj[i].eq,terceiro_sorteio+1);
                    }
                    if(segundo_sorteio==2){
                            //srand((unsigned)time(NULL));
                            terceiro_sorteio=rand()%NAVANCADOS;
                            passar_a_bola(jj[i].num_j,jj[i].eq,terceiro_sorteio+1+4);//numero de defesas que poderao existir
                    }

                }
                if(jj[i].num_j>0 && jj[i].num_j<NDEFESAS && primeiro_sorteio>80){
                    remate_aleatorio(jj[i].num_j,jj[i].eq);
                }

                if(jj[i].num_j>NDEFESAS && primeiro_sorteio<=60){/*Avancados rematam bem*/
                    if(jj[i].eq==0){
                        if(jj[i].b.co.y>=40){/*Se o avancado tiver nos ultimos dez metros remata à baliza*/
                            remate_baliza(jj[i].num_j,jj[i].eq);
                        }
                    }
                    if(jj[i].eq==1){
                        if(jj[i].b.co.y<=10){/*Se o avancado tiver nos ultimos dez metros remata à baliza*/
                            remate_baliza(jj[i].num_j,jj[i].eq);
                        }
                    }
                    if((jj[i].eq==0 && jj[i].c.y>10 && jj[i].c.y<40) || (jj[i].eq==1 && jj[i].c.y<40 && jj[i].c.y>10)){
                        //srand((unsigned)time(NULL));
                        segundo_sorteio=rand()%3;
                        if(segundo_sorteio==0){
                            passar_a_bola(jj[i].num_j,jj[i].eq,0);
                        }
                        if(segundo_sorteio==1){
                            //srand((unsigned)time(NULL));
                            terceiro_sorteio=rand()%NDEFESAS;
                            passar_a_bola(jj[i].num_j,jj[i].eq,terceiro_sorteio+1);
                        }
                        if(segundo_sorteio==2){
                            do{
                                //srand((unsigned)time(NULL));
                                terceiro_sorteio=rand()%NAVANCADOS;
                            }while(terceiro_sorteio+1+4==jj[i].num_j);/*Para nao passar ao proprio jogador*/
                            passar_a_bola(jj[i].num_j,jj[i].eq,terceiro_sorteio+1+4);//numero de defesas que poderao existir
                        }
                    }
                }
                /**Falta acabar os if's dos defesas->so nao pode passar a si proprio
                e para os avancados**/
            }
            else{/*Senao possui a bola, entao anda para tras e para a frente, cima e baixo*/
                //srand((unsigned)time(NULL));
                sorteia_movimento=rand()%4;
                if(sorteia_movimento==0){/*frente*/
                    novas_p_y=jj[i].c.y+1;/*Movimento a ser validado*/
                    novas_p_x=jj[i].c.x;/*Movimento a ser validado*/
                    resultado=verifica_jogada(novas_p_x,novas_p_y,jj[i].num_j);/*Verifica se o redes esta dentro do perimetro da grande area*/
                    coincide=nao_colide_com_jogador(novas_p_x,novas_p_y);/*Verifica se nao anda em cima de outro jogador*/
                    if(resultado==1 && coincide==1){/*Posso fazer movimento*/
                        jj[i].c.y++;
                    }
                }
                if(sorteia_movimento==1){/*Tras*/
                    novas_p_y=jj[i].c.y-1;/*Movimento a ser validado*/
                    novas_p_x=jj[i].c.x;/*Movimento a ser validado*/
                    resultado=verifica_jogada(novas_p_x,novas_p_y,jj[i].num_j);/*Verifica se o redes esta dentro do perimetro da grande area*/
                    coincide=nao_colide_com_jogador(novas_p_x,novas_p_y);/*Verifica se nao anda em cima de outro jogador*/
                    if(resultado==1 && coincide==1){/*Posso fazer movimento*/
                        jj[i].c.y--;
                    }
                }
                if(sorteia_movimento==2){/*Cima*/
                    novas_p_y=jj[i].c.y;/*Movimento a ser validado*/
                    novas_p_x=jj[i].c.x-1;/*Movimento a ser validado*/
                    resultado=verifica_jogada(novas_p_x,novas_p_y,jj[i].num_j);/*Verifica se o redes esta dentro do perimetro da grande area*/
                    coincide=nao_colide_com_jogador(novas_p_x,novas_p_y);/*Verifica se nao anda em cima de outro jogador*/
                    if(resultado==1 && coincide==1){/*Posso fazer movimento*/
                        jj[i].c.x--;
                    }
                }
                if(sorteia_movimento==3){/*Baixo*/
                    novas_p_y=jj[i].c.y;/*Movimento a ser validado*/
                    novas_p_x=jj[i].c.x+1;/*Movimento a ser validado*/
                    resultado=verifica_jogada(novas_p_x,novas_p_y,jj[i].num_j);/*Verifica se o redes esta dentro do perimetro da grande area*/
                    coincide=nao_colide_com_jogador(novas_p_x,novas_p_y);/*Verifica se nao anda em cima de outro jogador*/
                    if(resultado==1 && coincide==1){/*Posso fazer movimento*/
                        jj[i].c.x++;
                    }
                }

            }
        }
    }
}

/**Thread que verifica se houve golos e envia sinal para o cliente atualizar os marcadores**/
void *verifica_golos(){
/*
1-Verificar sempre onde anda a bola ou se um jogador a possui, ou se nao a possuir verifcar a sua posicao
2-se esta toca na baliza, verifico em qual das balizas e chamo funcao que envia sinal ao cliente
*/
int i,j;
int gg=((NDEFESAS+NAVANCADOS+1)*2);

do{
    for(i=0;i<gg;i++){
        if(jj[i].b.possui==1 || (jj[i].b.possui==0 && (jj[i].b.co.x!=0 && jj[i].b.co.y!=0))){/*Se o jogador possuir a bola ou se esta andar perdida*/
            if( (jj[i].b.co.y==1 && (jj[i].b.co.x>5 && jj[i].b.co.x<15))){
                marcador_dir++;
                fprintf(stderr,"\nResultado: %d %d.\n",marcador_esq,marcador_dir);
                envia_sinal_golo(0);/*Marcou a equipa da direita, atualizar no cliente, o marcador da direita*/
                usleep(1000000);//atualizar e nao estar sempre a correr
                jj[i].b.possui=0;
                /**Depois meter a bola no redes-->fazer aqui diretamente se jog for de uma eq meto a bola no redes da outra equipa**/
                for(j=0;j<gg;j++){
                    if(jj[j].eq==0 && jj[j].num_j==0){/*Guarda redes equipa adversaria*/
                        jj[i].b.co.x=0;
                        jj[i].b.co.y=0;
                        jj[j].b.possui=1;
                        jj[j].b.co.x=jj[j].c.x;
                        jj[j].b.co.y=jj[j].c.y+1;
                    }
                }
            }
            if((jj[i].b.co.y==50 && (jj[i].b.co.x>5 && jj[i].b.co.x<15))){
                marcador_esq++;
                fprintf(stderr,"\nResultado: %d %d.\n",marcador_esq,marcador_dir);
                envia_sinal_golo(1);/*Marcou a equipa da esquerda, atualizar no cliente, o marcador da esquerda*/
                usleep(1000000);
                jj[i].b.possui=0;
                /**Depois meter a bola no redes-->fazer aqui diretamente se jog for de uma eq meto a bola no redes da outra equipa**/
                for(j=0;j<gg;j++){
                    if(jj[j].eq==1 && jj[j].num_j==0){/*Guarda redes equipa adversaria*/
                        jj[i].b.co.x=0;
                        jj[i].b.co.y=0;
                        jj[j].b.possui=1;
                        jj[j].b.co.x=jj[j].c.x;
                        jj[j].b.co.y=jj[j].c.y-1;
                    }
                }
            }
        }
    }
}while(ha_jogo==1);
}

void *movimentacao_jogo(void *args){
joga *jo;
jo= (joga *) args;
int res;
int i;
int n;
int gg=((NDEFESAS+NAVANCADOS+1)*2);
int desc_tecla;
int nova_pos_x;/*define posicao iterada em x*/
int nova_pos_y;/*define posicao iterada em y*/
int resultado;
int res_t_lock;
int coincide;
char vito[50];
comand cc;
int eq;
int num_aleatorio;
int tem_bola;
//chule=open(FIFO_TECLAS,O_RDONLY);
int joga_existe;

//fprintf(stderr,"\npid: %d\n",jo->pid_cliente);

do{
//movimento aleatorio
if(jo->pid_cliente==0){
    //fprintf(stderr,"\nThread %d\n",jo->num_j);
    pthread_mutex_lock(&mutex);
    /**captura bola**/
    capturas_bola(jo->num_j,jo->eq);
    ultimo_adquire_a_bola(jo->num_j,jo->eq);

    fprintf(stderr,"\nA processar.\n");

    /**Movimentacao dos jogadores aleatorios**/
    jogador_aleatorio(jo->num_j,jo->eq);

        /**enviar novas posicoes do arrays aos usuarios que estao no jogo**/
        for(i=0;i<NUM_JOG;i++){
            if(jj[i].pid_cliente!=0){/*aos jogadores que estao a controlar um boneco, envio as novas posicoes*/
                sprintf(fifo_pid_cli,FIFO_CLIENTE,jj[i].pid_cliente);
                res=open(fifo_pid_cli,O_WRONLY);
                write(res,&jj,sizeof(jj));
                close(res);
            }

        }
         pthread_mutex_unlock(&mutex);
        usleep(1000000);/**Segundos aqui**/
}
else{
        sprintf(vito,FIFO_TECLAS,jo->pid_cliente);
        chule=open(vito,O_RDONLY);
        pid_cliente=0;
        n=read(chule,&cc,sizeof(comand));
        if(n>0){
            //fprintf(stderr,"\nThread desbloqueada: %d\n",jo->num_j);
            res_t_lock=pthread_mutex_lock(&mutex);
            //read(chule,&cc.pidss,sizeof(cc.pidss));
            //fprintf(stderr,"\nLi pid: %d\n",cc.pidss);
            if(cc.pidss==jo->pid_cliente){
                /*fprintf(stderr,"\nThread desbloqueada: %d\n",jo->num_j);
                fprintf(stderr,"\nThread bloqueada: %d\n",jo->num_j);
                fprintf(stderr,"\nPid recebido: %d\n",cc.pidss);
                fprintf(stderr,"\nJo pid: %d\n",jo->pid_cliente);
                fprintf(stderr,"\nJo num_j: %d\n",jo->num_j);*/
                //fprintf(stderr,"\nNum_j e eq: %d %d\n",jo->num_j,jo->eq);
                //fprintf(stderr,"\nPosicao: %d %d e possui %d\n",jo->c.x,jo->c.y,jo->b.possui);
                capturas_bola(jo->num_j,jo->eq);
                ultimo_adquire_a_bola(jo->num_j,jo->eq);
                switch(cc.key){
                    /**cima**/
                    case 'w':
                        nova_pos_x=jo->c.x-1;/*nova posicao em x mediante pedido do utilizador, que dps vai ser validada ou nao*/
                        nova_pos_y=jo->c.y;/*nova posicao em y mediante pedido do utilizador, que dps vai ser validada ou nao*/
                        /*verifica movimento*/
                        resultado=verifica_jogada(nova_pos_x,nova_pos_y,jo->num_j);
                        coincide=nao_colide_com_jogador(nova_pos_x,nova_pos_y);
                        if(resultado==1 && coincide==1){
                            tem_bola=tem_ou_nao_bola(jo->num_j,jo->eq);
                            if(tem_bola==0){
                                jo->c.x--;
                            }
                            else{/*tem bola na sua posse*/
                               jo->c.x--;/*Recuo posicao do jogador vertical*/
                               jo->b.co.x--;/*Recuo posicao da bola vertical*/
                            }
                        }
                    break;
                    /**direita**/
                    case 'd':
                        nova_pos_x=jo->c.x;/*nova posicao em x mediante pedido do utilizador, que dps vai ser validada ou nao*/
                        nova_pos_y=jo->c.y+1;/*nova posicao em y mediante pedido do utilizador, que dps vai ser validada ou nao*/
                        /*verifica movimento*/
                        resultado=verifica_jogada(nova_pos_x,nova_pos_y,jo->num_j);
                        coincide=nao_colide_com_jogador(nova_pos_x,nova_pos_y);
                        if(resultado==1 && coincide==1){
                            tem_bola=tem_ou_nao_bola(jo->num_j,jo->eq);
                            if(tem_bola==0){
                                jo->c.y++;
                            }
                            else{/*tem bola na sua posse*/
                               jo->c.y++;/*Avanco posicao do jogador na horizontal*/
                               jo->b.co.y++;/*Avanco posicao da bola na horizontal*/
                            }
                        }
                    break;
                    /**esquerda**/
                    case 'a':
                        nova_pos_x=jo->c.x;/*nova posicao em x mediante pedido do utilizador, que dps vai ser validada ou nao*/
                        nova_pos_y=jo->c.y-1;/*nova posicao em y mediante pedido do utilizador, que dps vai ser validada ou nao*/
                        /*verifica movimento*/
                        resultado=verifica_jogada(nova_pos_x,nova_pos_y,jo->num_j);
                        coincide=nao_colide_com_jogador(nova_pos_x,nova_pos_y);
                        if(resultado==1 && coincide==1){
                            tem_bola=tem_ou_nao_bola(jo->num_j,jo->eq);
                            if(tem_bola==0){
                                jo->c.y--;
                            }
                            else{/*tem bola na sua posse*/
                               jo->c.y--;/*Recuo posicao do jogador na horizontal*/
                               jo->b.co.y--;/*Recuo posicao da bola na horizontal*/
                            }
                        }
                    break;
                    /**baixo**/
                    case 's':
                        nova_pos_x=jo->c.x+1;/*nova posicao em x mediante pedido do utilizador, que dps vai ser validada ou nao*/
                        nova_pos_y=jo->c.y;/*nova posicao em y mediante pedido do utilizador, que dps vai ser validada ou nao*/
                        /*verifica movimento*/
                        resultado=verifica_jogada(nova_pos_x,nova_pos_y,jo->num_j);
                        coincide=nao_colide_com_jogador(nova_pos_x,nova_pos_y);
                        if(resultado==1 && coincide==1){
                           tem_bola=tem_ou_nao_bola(jo->num_j,jo->eq);
                            if(tem_bola==0){
                                jo->c.x++;
                            }
                            else{/*tem bola na sua posse*/
                               jo->c.x++;/*Avanco posicao do jogador na vertical*/
                               jo->b.co.x++;/*Avanco posicao da bola na vertical*/
                            }
                        }
                    break;
                    /**sair**/
                    case 'q':
                        jo->pid_cliente=0;
                    break;

                    /**mudar para redes da msm equipa**/
                    /*1 verificar qual a equipa
                    2 verificar se o redes nao esta a ser controlar por um user
                    3 se nao colocar pid do atual a 0 e colocar o pid do redes igual ao do user*/
                    case '0':
                        joga_existe=verifica_jogador_existe(0,jo->eq);
                        /**Passe do jogador que tem a bola para o guarda redes**/
                        if(jo->b.possui==1 && joga_existe==1 &&jo->num_j!=0){/*Defesa passa ao redes*/
                           /*Gerado numero aleatorio*/
                            num_aleatorio=rand()%101;
                            if( (jo->num_j>0 && jo->num_j<5 && num_aleatorio<80) || (jo->num_j>4 && jo->num_j<9 && num_aleatorio<60)){/*Depois mudar so para experimentar*/
                                passar_a_bola(jo->num_j,jo->eq,0);
                                break;
                              }
                            else{/*Se nao remate aleatorio*/
                                /**Chamar aqui o remate aleatorio**/
                                remate_aleatorio(jo->num_j,jo->eq);
                                break;
                            }
                        }
                        /**SE nao tiver a bola e o gurada redes nao tiver a ser controlado, passa o redes a ser controlado pelo humano**/
                        if(jo->b.possui==0 && joga_existe==1 && jo->num_j!=0){
                            for(i=0;i<gg;i++){
                                if(jj[i].eq==jo->eq && jj[i].num_j==0 && jj[i].pid_cliente==0){/*Se pertencer a mesma equipa e nao estiver a ser controlado por nenhum usuario*/
                                    jj[i].pid_cliente=cc.pidss;
                                    jo->pid_cliente=0;
                                }
                            }
                        }
                    break;

                    case '1':
                        joga_existe=verifica_jogador_existe(1,jo->eq);
                        if(jo->b.possui==1 && joga_existe==1 && jo->num_j!=1){/*Se o jogador possui a bola,e se existe colega de equipa e nao é o proprio jogador a pedir*/
                            num_aleatorio=rand()%101;/*Gero numero aleatorio*/
                            if((jo->num_j==0 && num_aleatorio<=25) || (jo->num_j>0 && jo->num_j<5 && num_aleatorio<80) || (jo->num_j>4 && jo->num_j<9 && num_aleatorio<=60)){
                                passar_a_bola(jo->num_j,jo->eq,1);
                                break;
                            }
                            else{
                                remate_aleatorio(jo->num_j,jo->eq);
                                break;
                            }
                        }
                        if(jo->b.possui==0 && joga_existe==1 && jo->num_j!=1){/*Se nao possuir a bola*/
                            for(i=0;i<gg;i++){
                                if(jj[i].num_j==1 && jj[i].eq==jo->eq && jj[i].pid_cliente==0){/*Se pertencer a mesma equipa e nao estiver a ser controlado por nenhum usuario*/
                                    jj[i].pid_cliente=cc.pidss;
                                    jo->pid_cliente=0;
                                }
                            }
                        }
                    break;

                    case '2':
                        joga_existe=verifica_jogador_existe(2,jo->eq);
                        if(jo->b.possui==1 && joga_existe==1 && jo->num_j!=2){/*Se o jogador possui a bola,e se existe colega de equipa e nao é o proprio jogador a pedir*/
                            num_aleatorio=rand()%101;/*Gero numero aleatorio*/
                            if((jo->num_j==0 && num_aleatorio<=25) || (jo->num_j>0 && jo->num_j<5 && num_aleatorio<80) || (jo->num_j>4 && jo->num_j<9 && num_aleatorio<=60)){
                                passar_a_bola(jo->num_j,jo->eq,2);
                                break;
                            }
                            else{
                                remate_aleatorio(jo->num_j,jo->eq);
                                break;
                            }
                        }
                        if(jo->b.possui==0 && joga_existe==1 && jo->num_j!=2){/*Se nao possuir a bola*/
                            for(i=0;i<gg;i++){
                                if(jj[i].num_j==2 && jj[i].eq==jo->eq && jj[i].pid_cliente==0){/*Se pertencer a mesma equipa e nao estiver a ser controlado por nenhum usuario*/
                                    jj[i].pid_cliente=cc.pidss;
                                    jo->pid_cliente=0;
                                }
                            }
                        }
                    break;

                    case '3':
                        joga_existe=verifica_jogador_existe(3,jo->eq);
                        if(jo->b.possui==1 && joga_existe==1 && jo->num_j!=3){/*Se o jogador possui a bola,e se existe colega de equipa e nao é o proprio jogador a pedir*/
                            num_aleatorio=rand()%101;/*Gero numero aleatorio*/
                            if((jo->num_j==0 && num_aleatorio<=25) || (jo->num_j>0 && jo->num_j<5 && num_aleatorio<80) || (jo->num_j>4 && jo->num_j<9 && num_aleatorio<=60)){
                                passar_a_bola(jo->num_j,jo->eq,3);
                                break;//para nao fazer o if de baixo
                            }
                            else{
                                remate_aleatorio(jo->num_j,jo->eq);
                                break;//para nao fazer o if de baixo
                            }
                        }
                        if(jo->b.possui==0 && joga_existe==1 && jo->num_j!=3){/*Se nao possuir a bola*/
                            for(i=0;i<gg;i++){
                                if(jj[i].num_j==3 && jj[i].eq==jo->eq && jj[i].pid_cliente==0){/*Se pertencer a mesma equipa e nao estiver a ser controlado por nenhum usuario*/
                                    jj[i].pid_cliente=cc.pidss;
                                    jo->pid_cliente=0;
                                }
                            }
                        }
                    break;

                    case '4':
                        joga_existe=verifica_jogador_existe(4,jo->eq);
                        if(jo->b.possui==1 && joga_existe==1 && jo->num_j!=4){/*Se o jogador possui a bola,e se existe colega de equipa e nao é o proprio jogador a pedir*/
                            num_aleatorio=rand()%101;/*Gero numero aleatorio*/
                            if((jo->num_j==0 && num_aleatorio<=25) || (jo->num_j>0 && jo->num_j<5 && num_aleatorio<80) || (jo->num_j>4 && jo->num_j<9 && num_aleatorio<=60)){
                                passar_a_bola(jo->num_j,jo->eq,4);
                                break;//para nao fazer o if de baixo
                            }
                            else{
                                remate_aleatorio(jo->num_j,jo->eq);
                                break;//para nao fazer o if de baixo
                            }
                        }
                        if(jo->b.possui==0 && joga_existe==1 && jo->num_j!=4){/*Se nao possuir a bola*/
                            for(i=0;i<gg;i++){
                                if(jj[i].num_j==4 && jj[i].eq==jo->eq && jj[i].pid_cliente==0){/*Se pertencer a mesma equipa e nao estiver a ser controlado por nenhum usuario*/
                                    jj[i].pid_cliente=cc.pidss;
                                    jo->pid_cliente=0;
                                }
                            }
                        }
                    break;

                    case '5':
                        joga_existe=verifica_jogador_existe(5,jo->eq);
                        if(jo->b.possui==1 && joga_existe==1 && jo->num_j!=5){/*Se o jogador possui a bola,e se existe colega de equipa e nao é o proprio jogador a pedir*/
                            num_aleatorio=rand()%101;/*Gero numero aleatorio*/
                            if((jo->num_j==0 && num_aleatorio<=25) || (jo->num_j>0 && jo->num_j<5 && num_aleatorio<80) || (jo->num_j>4 && jo->num_j<9 && num_aleatorio<=60)){
                                passar_a_bola(jo->num_j,jo->eq,5);
                                break;//para nao fazer o if de baixo
                            }
                            else{
                                remate_aleatorio(jo->num_j,jo->eq);
                                break;//para nao fazer o if de baixo
                            }
                        }
                        if(jo->b.possui==0 && joga_existe==1 && jo->num_j!=5){/*Se nao possuir a bola*/
                            for(i=0;i<gg;i++){
                                if(jj[i].num_j==5 && jj[i].eq==jo->eq && jj[i].pid_cliente==0){/*Se pertencer a mesma equipa e nao estiver a ser controlado por nenhum usuario*/
                                    jj[i].pid_cliente=cc.pidss;
                                    jo->pid_cliente=0;
                                }
                            }
                        }
                    break;

                    case '6':
                        joga_existe=verifica_jogador_existe(6,jo->eq);
                        if(jo->b.possui==1 && joga_existe==1 && jo->num_j!=6){/*Se o jogador possui a bola,e se existe colega de equipa e nao é o proprio jogador a pedir*/
                            num_aleatorio=rand()%101;/*Gero numero aleatorio*/
                            if((jo->num_j==0 && num_aleatorio<=25) || (jo->num_j>0 && jo->num_j<5 && num_aleatorio<80) || (jo->num_j>4 && jo->num_j<9 && num_aleatorio<=60)){
                                passar_a_bola(jo->num_j,jo->eq,6);
                                break;//para nao fazer o if de baixo
                            }
                            else{
                                remate_aleatorio(jo->num_j,jo->eq);
                                break;//para nao fazer o if de baixo
                            }
                        }
                        if(jo->b.possui==0 && joga_existe==1 && jo->num_j!=6){/*Se nao possuir a bola*/
                            for(i=0;i<gg;i++){
                                if(jj[i].num_j==6 && jj[i].eq==jo->eq && jj[i].pid_cliente==0){/*Se pertencer a mesma equipa e nao estiver a ser controlado por nenhum usuario*/
                                    jj[i].pid_cliente=cc.pidss;
                                    jo->pid_cliente=0;
                                }
                            }
                        }
                    break;

                    case '7':
                        joga_existe=verifica_jogador_existe(7,jo->eq);
                        if(jo->b.possui==1 && joga_existe==1 && jo->num_j!=7){/*Se o jogador possui a bola,e se existe colega de equipa e nao é o proprio jogador a pedir*/
                            num_aleatorio=rand()%101;/*Gero numero aleatorio*/
                            if((jo->num_j==0 && num_aleatorio<=25) || (jo->num_j>0 && jo->num_j<5 && num_aleatorio<80) || (jo->num_j>4 && jo->num_j<9 && num_aleatorio<=60)){
                                passar_a_bola(jo->num_j,jo->eq,7);
                                break;//para nao fazer o if de baixo
                            }
                            else{
                                remate_aleatorio(jo->num_j,jo->eq);
                                break;//para nao fazer o if de baixo
                            }
                        }
                        if(jo->b.possui==0 && joga_existe==1 && jo->num_j!=7){/*Se nao possuir a bola*/
                            for(i=0;i<gg;i++){
                                if(jj[i].num_j==7 && jj[i].eq==jo->eq && jj[i].pid_cliente==0){/*Se pertencer a mesma equipa e nao estiver a ser controlado por nenhum usuario*/
                                    jj[i].pid_cliente=cc.pidss;
                                    jo->pid_cliente=0;
                                }
                            }
                        }
                    break;

                    case '8':
                        joga_existe=verifica_jogador_existe(8,jo->eq);
                        if(jo->b.possui==1 && joga_existe==1 && jo->num_j!=8){/*Se o jogador possui a bola,e se existe colega de equipa e nao é o proprio jogador a pedir*/
                            num_aleatorio=rand()%101;/*Gero numero aleatorio*/
                            if((jo->num_j==0 && num_aleatorio<=25) || (jo->num_j>0 && jo->num_j<5 && num_aleatorio<80) || (jo->num_j>4 && jo->num_j<9 && num_aleatorio<=60)){
                                passar_a_bola(jo->num_j,jo->eq,8);
                                break;//para nao fazer o if de baixo
                            }
                            else{
                                remate_aleatorio(jo->num_j,jo->eq);
                                break;//para nao fazer o if de baixo
                            }
                        }
                        if(jo->b.possui==0 && joga_existe==1 && jo->num_j!=8){/*Se nao possuir a bola*/
                            for(i=0;i<gg;i++){
                                if(jj[i].num_j==8 && jj[i].eq==jo->eq && jj[i].pid_cliente==0){/*Se pertencer a mesma equipa e nao estiver a ser controlado por nenhum usuario*/
                                    jj[i].pid_cliente=cc.pidss;
                                    jo->pid_cliente=0;
                                }
                            }
                        }
                    break;

                    /**Remate para a baliza**/
                    case 'g':
                        if(jo->b.possui==1){
                            num_aleatorio=rand()%101;
                            if((jo->num_j==0 && num_aleatorio<25) || (jo->num_j>0 && jo->num_j<5 && num_aleatorio<20) || (jo->num_j>4 && jo->num_j<9 && num_aleatorio<60)){
                                remate_baliza(jo->num_j,jo->eq);
                            }
                            else{
                                remate_aleatorio(jo->num_j,jo->eq);
                            }
                        }
                    break;

            }

                /**Trata falta jogador**/
                if(falta==1 && player==jo->pid_cliente){
                    fprintf(stderr,"\nFalta Thread.\n");
                        usleep(5000000);
                        falta=0;
                }

                /**enviar novas posicoes do arrays aos usuarios que estao no jogo**/
        for(i=0;i<NUM_JOG;i++){
            if(jj[i].pid_cliente!=0){/*aos jogadores que estao a controlar um boneco, envio as novas posicoes*/
                sprintf(fifo_pid_cli,FIFO_CLIENTE,jj[i].pid_cliente);
                res=open(fifo_pid_cli,O_WRONLY);
                write(res,&jj,sizeof(jj));
                close(res);
            }

        }

         }
    }
                    pthread_mutex_unlock(&mutex);
                    if(jo->num_j==0 || jo->num_j<4){
                        usleep(300000);
                    }
                    else{
                        usleep(400000);
                    }
}

            //fprintf(stderr,"\nSai da thread %d\n",jo->num_j);
            //close(chule);

 }while(ha_jogo==1);
              //usleep(400000);

close(chule);
//pthread_cancel(jogadores_campo[i]);
return NULL;
}

void cria_threads(){
int g=(NDEFESAS+NAVANCADOS+1)*2;
joga jogadores[(NAVANCADOS+NDEFESAS+1)*2];
int i;
void *b;

/**Varivel verifica-->cria apenas uma vez as threads-->nao é necessaria**/
//criacao das threads
    if(ha_jogo==1 && verifica==0){/*Crio logo no start*/
            verifica++;
        for(i=0;i<g;i++){
            //fprintf(stderr,"\nnnnana\n");
                        pthread_create(&jogadores_campo[i],NULL,movimentacao_jogo,(void *) &jj[i]);
        }
        pthread_create(&golos,NULL,verifica_golos,NULL);
    }

        if(stops==1){
                for(i=0;i<g;i++){
                    pthread_join(jogadores_campo[i],&b);
                    fprintf(stderr,"\nSaiu\n");
                }
                pthread_join(golos,NULL);
            }
stops=0;

    return;
}


int main(int argc, char *argv[]){
int res;
char pedido[50];
char comando[50]="";
int k;
char *token;
int verifica=0;
char *comando2[50];//necessario para o user username password

int g=(NDEFESAS+NAVANCADOS+1)*2;
joga jogadores[(NAVANCADOS+NDEFESAS+1)*2];
int i;
pthread_t jogadores_campo[(NDEFESAS+NAVANCADOS+1)*2];

//verificacao se já existe um servidor aberto, se existir fecha este novo
if(access(FIFO_SERVIDOR,F_OK)==0){
    fprintf(stderr,"\nO servidor já se encontra a ser executado.\n");
    exit(EXIT_SUCCESS);
}

fprintf(stderr,"\nRes: Marc_esq Marc_dir: %d %d\n",marcador_esq,marcador_dir);

define_var_ambiente();
carrega_posicao_inicial_jogadores(jj);
//lista_players();

fich=argv[1];

    //implementacao da estrutura do signal recebido no login
    struct sigaction sas;
    sas.sa_flags=SA_SIGINFO;
    sigemptyset(&sas.sa_mask);//inicializacao da estrutura
    sas.sa_handler=SIG_DFL;//modificacao apos
    sas.sa_sigaction= &recebi_sinal;
    sigaction(SIGUSR2,&sas,NULL);
    sigaction(SIGUSR1,&sas,NULL);
    sigaction(SIGALRM,&sas,NULL);

    coloca_pid_servidor_fich("pid_servidor.txt");

    res=mkfifo(FIFO_SERVIDOR,0777);//dadas todas as permissoes
    if(res==-1){
        perror("\nErro ao criar o fifo do servidor.\n");
        exit(EXIT_FAILURE);
    }
    //se correu bem
    fprintf(stderr,"\nO fifo do servidor foi criado.\n");


    desc_serv=open(FIFO_SERVIDOR,O_RDWR);
    if(desc_serv==-1){
        perror("\nErro ao abrir o fifo do servidor.\n");
        //como nao consegui abrir, não faz sentido continuar no programa
        exit(EXIT_FAILURE);
    }
    //se consegui abrir o FIFO_SERVIDOR
    fprintf(stderr,"\nAberto o FIFO do servidor.\n");


    while(1){
    cria_threads();
    int x=0;
        printf("\n<\n");
        scanf(" %[^\n]",comando);
        //mudar isto aqui e utilizar o strtok-->ou seja no scanf utilizar uma string normal, e usando o strtok separo o que foi escrito e valido conforme o que foi escrito no 1 argumento user, users, etc
        token=strtok(comando," ");
                while(token!=NULL){
                    //printf("\n%s\n",token);
                    comando2[x]=token;
                    //printf("\n%s\n",comando2[x]);
                    x++;
                    token=strtok(NULL," ");
        }
            if(strcmp(comando2[0],"user")==0){
                //chamar uma funcao que verificar se um username já existe, se existir nao mete, se nao existir adiciona o user ao ficheiro
                //printf("\nEntrei aqui.\n");
                k=coloca_user_ficheiro(argv[1],comando2);
            }
            if(strcmp(comando2[0],"users")==0){
                //printf("\nEntrei.\n");
                verifica_logados();
            }
            if(strcmp(comando2[0],"start")==0){
                envia_sinal_entrar_jogo(u);
                /*
                Necessario tratar aqui do alarme do jogo
                */
                alarm(atoi(comando2[1]));
                inicializa_mutexes();
                ha_jogo=1;
            }
            if(strcmp(comando2[0],"shutdown")==0){
                if(ha_jogo==0){
                    //envio do sinal aos cli's a dizer que vou embora
                    envia_sinal_sair_jogo_aos_clis(u);
                    //recebo sinal SIGUSR1 e termino a execução apagando o FIFO do servidor
                    envia_sigusr1_terminar();
                }
            }
            if(strcmp(comando2[0],"stop")==0){
                if(ha_jogo==1){
                    stops++;/*Join das Threads*/
                    ha_jogo=0;/*Controlao jogo*/
                    verifica=0;/*Criacao das Threads*/
                    envia_join_stop();
                    if(marcador_esq==marcador_dir){
                        fprintf(stderr,"\nAcabaram empatados.\n");
                    }
                    if(marcador_esq>marcador_dir){
                        fprintf(stderr,"\nGanhou a equipa 0\n");
                    }
                    if(marcador_esq<marcador_dir){
                        fprintf(stderr,"\nGanhou a equipa 1\n");
                    }
                    alarm(0);
                }
                //pthread_exit(&jogadores_campo[0]);
                //continuar depois aqui
            }
            if(strcmp(comando2[0],"matriz")==0){
                lista_matriz(u);
            }
            if(strcmp(comando2[0],"red")==0){
                if(ha_jogo==1 && tenho_usuarios_no_jogo>0){
                    envia_sinal_cartao_vermelho(u,jj,comando2[1]);
                }
            }
            if(strcmp(comando2[0],"result")==0){
                if(ha_jogo==1){
                    printf("\nResultado: %d %d\n",marcador_esq,marcador_dir);
                    fflush(stdout);
                }
            }


        memset(comando,' ',sizeof(comando));//reset aos comandos definidos pelo user para nao executar sempre o mesmo até que se volte a inserir novos comandos
        //tenho de fazer memset do comando2
    }
pthread_mutex_destroy(&mutex);
close(desc_serv);
unlink(FIFO_SERVIDOR);

return 0;
}
