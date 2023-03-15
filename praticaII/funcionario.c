#include "funcionario.h"

#include <string.h>
#include <math.h>

// Cria funcionario. Lembrar de usar free(func)
TFunc *funcionario(int cod, char *nome, double salario) {
    TFunc *func = (TFunc *) malloc(sizeof(TFunc));
    //inicializa espaço de memória com ZEROS
    if (func) memset(func, 0, sizeof(TFunc));
    //copia valores para os campos de func
    func->cod = cod;
    strcpy(func->nome, nome);
    func->salario = salario;
    return func;
}

// Salva funcionario no arquivo out, na posicao atual do cursor
void salva(TFunc *func, FILE *out) {
    fwrite(&func->cod, sizeof(int), 1, out);
    //func->nome ao invés de &func->nome, pois string já é ponteiro
    fwrite(func->nome, sizeof(char), sizeof(func->nome), out);
    fwrite(&func->salario, sizeof(double), 1, out);
}

// Le um funcionario do arquivo in na posicao atual do cursor
// Retorna um ponteiro para funcionario lido do arquivo
TFunc *le(FILE *in) {
    TFunc *func = (TFunc *) malloc(sizeof(TFunc));
    if (0 >= fread(&func->cod, sizeof(int), 1, in)) {
        free(func);
        return NULL;
    }
    fread(func->nome, sizeof(char), sizeof(func->nome), in);
    fread(&func->salario, sizeof(double), 1, in);
    return func;
}

int cmp_func(TFunc *f1, TFunc *f2) {
    if (f1 == NULL) {
        return (f2 == NULL);
    }
    if (f1->cod != f2->cod) {
        return 0;
    }
    if (strcmp(f1->nome, f2->nome) != 0) {
        return 0;
    }
    if (f1->salario != f2->salario) {
        return 0;
    }
    return 1;
}

// Retorna tamanho do funcionario em ‘bytes’
int tamanho_registro() {
    return sizeof(int)  //cod
           + sizeof(char) * 10 //nome
           + sizeof(double); //salario
}

int tamanho_arquivo(FILE *arq) {
    fseek(arq, 0, SEEK_END);
    int tam = trunc(ftell(arq) / tamanho_registro());
    return tam;
}

void insere_1000_funcionarios(FILE *out) {
    for (int i = 1; i <= 1000; i++) {
        TFunc *f1 = funcionario(i, "Ana", 3000 + i);
        salva(f1, out);
        free(f1);
    }
}


TFunc *busca_binaria(int chave, FILE *in, int inicio, int fim) {
    TFunc *f = NULL;
    int cod = -1;
    int comparacoes = 0;
    double tempo_gasto = 0.0;
    while (inicio <= fim && cod != chave) {
        int meio = trunc((inicio + fim) / 2);
        // printf("Inicio: %d; Fim: %d; Meio: %d\n", inicio, fim, meio);
        fseek(in, (meio - 1) * tamanho_registro(), SEEK_SET);
        f = le(in);
        cod = f->cod;
        comparacoes++;
        if (f) {
            if (cod > chave) {
                fim = meio - 1;
            } else {
                inicio = meio + 1;
            }
        }
    }
    if (cod == chave) {
        return f;
    } else return NULL;
}

