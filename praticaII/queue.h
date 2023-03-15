#ifndef QUEUE_H
#define QUEUE_H

#include "arvore-b.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct TFunc TFunc;
typedef struct no_b no_b;
typedef struct bTree bTree;

typedef struct queue{
    int size;
    int front;
    int rear;
    int itemCount;
    no_b *list;
}queue;

queue *cria_fila(int size);

no_b peek(queue *q);

bool isEmpty(queue *q);

bool isFull(queue *q);

int size(queue *q);

void insere(queue *q , no_b data);

no_b removeData(queue *q);

#endif