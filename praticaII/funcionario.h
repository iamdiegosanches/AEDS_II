#ifndef PRATICAII_FUNCIONARIO_H
#define PRATICAII_FUNCIONARIO_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Funcionario {
    int cod;
    char nome[50];
    double salario;
} TFunc;

// Cria funcionario. Lembrar de usar free(funcionario)
TFunc *funcionario(int cod, char *nome, double salario);

// Salva funcionario no arquivo out, na posicao atual do cursor
void salva(TFunc *func, FILE *out);

// Le um funcionario do arquivo in na posicao atual do cursor
// Retorna um ponteiro para funcionario lido do arquivo
TFunc *le(FILE *in);

// Retorna tamanho do funcionario em ‘bytes’
int tamanho_registro();

int cmp_func(TFunc *f1, TFunc *f2);

// Retorna o tamanho do arquivo em ‘bytes’
int tamanho_arquivo(FILE *arq);

void insere_1000_funcionarios(FILE *out);

void imprime_arquivo(FILE *arq);

TFunc *busca_binaria(int chave, FILE *in, int inicio, int fim);

#endif //PRATICAII_FUNCIONARIO_H
