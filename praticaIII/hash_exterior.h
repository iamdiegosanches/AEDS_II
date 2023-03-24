#ifndef ENCADEAMENTO_EXTERIOR_H
#define ENCADEAMENTO_EXTERIOR_H

#include "funcionario.h"

// Cria uma tabela hash vazia de tamanho tam, e salva no arquivo nome_arquivo_hash
// Compartimento que nao tem lista encadeada associada deve ter valor igual a -1
// nome_arquivo_hash: nome do arquivo hash a ser criado
// tam: tamanho da tabela hash a ser criada
void cria_hash(char *nome_arquivo_hash, int tam);

// Retorna o funcinoario pesquisado
TFunc *busca(int cod, FILE *arq_hash, char *nome_arquivo_dados, int tam);

int insere(TFunc *func, FILE *arq_hash, char *nome_arquivo_dados, int tam);

// Retorna o endereco do cliente que foi excluido, ou -1 se cliente nao existe
int exclui(int cod, FILE *arq_hash, char *nome_arquivo_dados, int tam);

#endif