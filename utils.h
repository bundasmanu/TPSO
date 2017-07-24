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

//fifo do servidor
#define FIFO_SERVIDOR "/tmp/fifo_servidor"

//fifo dos clientes, onde o %d--> vai ser atribuido ao pid associado a cada cliente
#define FIFO_CLIENTE "/tmp/fifo_%d_cliente"

//NUMERO max de jogadores
#define NUM_JOG 20

//typedef's das funcoes
typedef struct utilizador user, *userr;
typedef struct resposta resp;
typedef struct cliente cli;

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
};

#endif // UTILS_H_INCLUDED
