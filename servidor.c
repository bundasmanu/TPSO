#include "utils.h"

user w;
userr v_din=NULL;//para ver isso dos logados-->comando "users"
userr novo_din=NULL;
int t=0;//-->controla insercoes
int tt=0;//-->controla os exit's
int desc_serv, desc_cli;//Descritores
int logs;
char *fich;
char fifo_pid_cli[50];
int result_res2;
char comando[3][50]={"","",""};//necessario para o user username password

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
int pid_servidor;
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

    printf("\nEntrei.\n");
    printf("\nMat1: %s Mat2: %s\n",mat[1],mat[2]);

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



void recebi_sinal(int signo, siginfo_t *info, void *extra){
int i=0,kk=0;//variavel que permite controlar o numero de users ate chegar a 20-->usado no comando "users"

printf("\nt: %d\n",t);
printf("\ntt: %d\n",tt);

    printf("\nRecebi pedido por parte de um cliente.\n");
    result_res2=read(desc_serv,&w,sizeof(user));
    if(result_res2 < sizeof(user)){
            fprintf(stderr,"\nResposta nao foi recebida na integra.\n");
            return;
    }
    fprintf(stderr,"\nPedido de Validacao entregue, vamos verificar.\n");
    if(strcmp(w.comando,"login")==0){
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
                        t++;//variavel aumenta-->esta variavel é responsavel por fazer o realloc

                        if(t==1){
                            //alocacao de memoria para definir novo user logado
                            v_din = (userr) malloc(sizeof(user)*1);
                        }

                        //já tenho a alocacao de memoria feita, é feita no inicio do programa, agora tenho de inserir os users logados
                     for(i=0;i<t;i++){
                        if(v_din[i].rr.logado!=1 && kk==0){//colocar apenas 1 vez(atraves do kk), na 1 posicao que encontrar o user logado
                            v_din[i].c.pids=w.c.pids;
                            v_din[i].rr.logado=1;
                            kk++;
                            printf("\nPosicao: %d",i);
                        }
                     }

                     //aloco logo espaco para mais uma nova entrada depois da insercao de uma pessoa
                     novo_din=realloc(v_din,1+t);
                     v_din=novo_din;

                        //ate chegar ao limite de 16 users logados-->para já nao tou a validar isso
                        //escreve na estrutura elementar o logado que depois é passado ao cliente-->no write
                        write(desc_cli,&w,sizeof(user));

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


                    write(desc_cli,&w,sizeof(user));

                    close(desc_cli);//fecho logo o descritor do cliente, dps de enviada a informacao
                    return;

            }
    }
    if(strcmp(w.comando,"exit")==0){
        //agora o que tenho de fazer é percorrer todos os clientes logados e ver onde ta aquele com o pid enviado
        //para esse cliente-->retiro-o dos logados-->ou atraves do memset ou outra forma qualquer
        for(i=0;i<t;i++){
            if(v_din[i].c.pids==w.c.pids){
                    //aloco logo espaco para mais uma nova entrada para apagar uma pessoa
                     novo_din=realloc(v_din,1+t);
                     v_din=novo_din;
                if((t-tt)!=1){
                    v_din[i]=v_din[i+1];
                    printf("\nEntrei diferente.\n");
                }
                if((t-tt)==1){
                    //alocacao de memoria para definir novo user logado
                    printf("\nEntrei igual.\n");
                    v_din = (userr) malloc(sizeof(user)*1);
                }
                //apaga-lo do panorama
                printf("\nEntrei na eliminacao do usuario.\n");
                tt++;//-->tem de se atualizar no fim de apagar para dar certo
        }

    }
    }
//free(novo_din);
//free(v_din);
}

void envia_threads_user(){


}

void verifica_logados(){
    int i;

printf("\nLogados:\n");
    for(i=0;i<t-tt;i++){//definir tamanho de jogadores-->os que inseri - apaguei = tamanho dos users
        //percorrer todos os jogadores
        if(v_din[i].rr.logado==1){
            fprintf(stderr,"\nPid: %d\n",v_din[i].c.pids);
        }

    }

return;
}

void envia_sinal_entrar_jogo(userr n){//recebe o vetor dinamico
int i;
union sigval sig;

//falta definir os inteiros nesta funcao e no cliente  verificar se da uniao recebeu 1 ou 2 e agir conforme o pedido

    for(i=0;i<t-tt;i++){
            sigqueue(n[i].c.pids,SIGUSR2,sig);
    }
return;
}

void envia_sinal_sair_jogo_aos_clis(userr nn){
//percorrer o vetor dinamico e enviar sinal a todos os users logados
int i;
union sigval sig;

//falta definir os inteiros nesta funcao

    for(i=0;i<t-tt;i++){
        sigqueue(nn[i].c.pids,SIGUSR2,sig);
    }


return;
}


int main(int argc, char *argv[]){
int res;
char pedido[50];
char *comando2[50];//necessario para o user username password
char comando[50]="";
int k;
char *token;

//verificacao se já existe um servidor aberto, se existir fecha este novo
if(access(FIFO_SERVIDOR,F_OK)==0){
    fprintf(stderr,"\nO servidor já se encontra a ser executado.\n");
    exit(EXIT_SUCCESS);
}


fich=argv[1];

    //implementacao da estrutura do signal recebido no login
    struct sigaction sas;
    sas.sa_flags=SA_SIGINFO;
    sigemptyset(&sas.sa_mask);//inicializacao da estrutura
    sas.sa_handler=SIG_DFL;//modificacao apos
    sas.sa_sigaction= &recebi_sinal;
    sigaction(SIGUSR2,&sas,NULL);

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
    int x=0;
        printf("\n<\n");
        scanf(" %[^\n]",comando);
        //mudar isto aqui e utilizar o strtok-->ou seja no scanf utilizar uma string normal, e usando o strtok separo o que foi escrito e valido conforme o que foi escrito no 1 argumento user, users, etc
        token=strtok(comando," ");
                while(token!=NULL){
                    printf("\n%s\n",token);
                    comando2[x]=token;
                    printf("\n%s\n",comando2[x]);
                    x++;
                    token=strtok(NULL," ");
        }
            if(strcmp(comando2[0],"user")==0){
                //chamar uma funcao que verificar se um username já existe, se existir nao mete, se nao existir adiciona o user ao ficheiro
                printf("\nEntrei aqui.\n");
                k=coloca_user_ficheiro(argv[1],comando2);
            }
            if(strcmp(comando2[0],"users")==0){
            printf("\nEntrei.\n");
                verifica_logados();
            }
            if(strcmp(comando2[0],"start")==0){
                envia_sinal_entrar_jogo(v_din);
            }
            if(strcmp(comando2[0],"shutdown")==0){
                //envio do sinal aos cli's a dizer que vou embora
                envia_sinal_sair_jogo_aos_clis(v_din);
                //recebo sinal SIGUSR1 e termino a execução apagando o FIFO do servidor
                /*
                //->fazer aqui chamada a funcao do SIGUSR1 para desligar
                */
            }


        memset(comando,' ',sizeof(comando));//reset aos comandos definidos pelo user para nao executar sempre o mesmo até que se volte a inserir novos comandos
        //tenho de fazer memset do comando2
    }
free(v_din);
close(desc_serv);
unlink(FIFO_SERVIDOR);

return 0;
}
