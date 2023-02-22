#ifndef LISTA_H_INCLUDED
#define LISTA_H_INCLUDED

typedef struct Lista {
    char *nome;
    struct Lista *prox;
} Lista;

// Imprime lista de nomes
void imprime_nomes(Lista *nomes);

// Aloca copia de str
char *cria_str(char *str);

// Cria lista encadeada de nomes. Lembrar de chamar libera_nomes(nomes)
Lista *cria_nomes(char *nome, Lista *prox);

// Desaloca lista encadeada de nomes
void libera_nomes(Lista *nomes);

// Retorna quantidade de nomes
int conta_nomes(Lista *nomes);

// Inverte lista encadeada de nomes
void inverte_nomes(Lista *nomes);

// Adiciona nome no final da lista
void adiciona_nomes(Lista *nomes, char *nome);

#endif // LISTA_H_INCLUDED
