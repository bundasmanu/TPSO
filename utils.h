#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<ncurses.h>
#include<pthread.h>
#include<time.h>

//fifo do servidor
#define FIFO_SERVIDOR "/tmp/fifo_servidor"

//fifo dos clientes, onde o %d--> vai ser atribuido ao pid associado a cada cliente
#define FIFO_CLIENTE "/tmp/fifo_%d_cliente"

//fifo relativo ao arbitro
#define FIFO_ARBITRO "/tmp/fifo_arbitro"

//fifo teclas
#define FIFO_TECLAS "/tmp/fifo_teclas_%d"

//NUMERO max de jogadores
#define NUM_JOG 18


int NAVANCADOS=2;
int NDEFESAS=2;

//typedef's das funcoes
typedef struct utilizador user, *userr;
typedef struct resposta resp;
typedef struct cliente cli;
typedef struct coordenadas coorde;
typedef struct jogador joga, *jogar;
typedef struct comandos comand;
typedef struct bol bola;

//estrutura para ver se um cliente digitou bem as credenciais
struct resposta{
    int logado;//1-->esta logado 0-->nao esta correta a senha
};

struct cliente{
    int pids;//contem o pid de cada cliente
};

//estrutura de referencia para o utilizador, que contem nome e pass do utilizador
struct utilizador{
    char nome[50];
    char pass[50];
    char comando[50];
    resp rr;
    cli c;
    int t;//verificar a existencia do jogo
};

struct coordenadas{
    int x;//referencia ao eixo x
    int y;//referencia ao eixo y
};

struct bol{
    coorde co;
    int possui;
};

struct jogador{
    int eq;//distincao de qual equipa é
    int num_j;//o numero de jogador que é na equipa
    coorde c;
    int pid_cliente;//pode ser jogador automatico ou nao se nao for tem inerente o pid do cliente que o está a controlar
    bola b;
};

struct comandos{
    int key;
    int pidss;
};

#endif // UTILS_H_INCLUDED
