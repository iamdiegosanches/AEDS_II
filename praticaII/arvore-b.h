#ifndef BTREE_H
#define BTREE_H

#include <stdio.h>
#include <malloc.h>
#include "queue.h"

typedef struct funcionario{
    int codigo;
    float salario;
}funcionario;

typedef struct no_b{
    int numKeys;          // nº de chaves que o nó possui
    int isLeaf;           // é um nó folha? 1 = true, 0 = false
    int pos_in_disk;      // posição do nó no arquivo
    funcionario *keys;    // guarda as chaves do nó
    int *filhos;          // guarda os filhos do nó
}no_b;

typedef struct bTree {
    int order; // ordem da árvore B
    no_b root; // raiz da árvore B
    int node_count; // total de nós que a árvore possui
} bTree;

typedef struct queue queue;

// Cria uma nova árvore vazia
bTree *cria_arvore(int order);

// Retorna um valor diferente de zero se a chave está presente na árvore
int buscar(no_b node, int ordem, funcionario codigo, FILE *fp);

// Insere um novo elemento na árvore
void inserir(bTree *tree, funcionario chave, FILE *fp);

// Remove uma chave da árvore
void deletar(bTree *tree, funcionario codigo, FILE *fp);

// Imprime todas as chaves da árvore a partir da raiz
void imprime_arvore(bTree *tree, queue *q, FILE *fp);

// Imprime as chaves em ordem
void imprime_arvore_crescente(bTree *tree, queue *q, FILE *fp);

// Deletar o arquivo da arvore
void deleta_arquivo(bTree *tree, FILE *fp);

funcionario btfindMax(no_b node, int order, FILE *fp);
funcionario btfindMin(no_b node, int order, FILE *fp);

no_b ler_disco(int disk, int order, FILE *fp);

#endif