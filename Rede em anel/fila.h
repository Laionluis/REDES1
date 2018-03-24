#ifndef _LIB_H_
#define _LIB_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100

struct mensagem
{   
	int ACK;
    char origem[20];
    char destino[140];
    char mensagem[140];    
    int prioridade;
int eh_bastao;
};

typedef struct {
    struct mensagem *message;
    int len;
    int size;
} heap_t;

//struct mensagem pri_que[MAX];


void push(heap_t *h, struct mensagem data);
struct mensagem *pop (heap_t *h);

#endif
