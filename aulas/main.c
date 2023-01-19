#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "funcionario.c"

//Constante para o numero de funcionarios
#define NFUNC 1000

void insere_1000_funcionarios(FILE *out) {
    for(int i = 1; i <= NFUNC; i++) {
        TFunc *f1 = funcionario(i, "Ana", "000.000.000-00", "01/01/1980", 3000);
        salva(f1, out);
        free(f1);
    }
}

void sobrescreve_funcionario(FILE *in, int n, TFunc *f1) {
    //pula primeiros n registros para posicionar no início do quarto registro
    fseek(in, tamanho() * n, SEEK_SET);
    salva(f1, in);
}

// Fisher-yates algoritmo de aleatorizacao
void shuffle(FILE *in, int n) {
    // Valores diferentes todas as vezes
    srand (time(NULL));

    TFunc *f1 = NULL;
    TFunc *f2 = NULL;

    for (int i = n-1; i > 0; i--) {
        int j = rand() % (i+1);

        fseek(in, tamanho() * i, SEEK_SET);
        f1 = le(in);
        fseek(in, tamanho() * j, SEEK_SET);
        f2 = le(in);

        sobrescreve_funcionario(in, i, f2);
        sobrescreve_funcionario(in, j, f1);

        free(f1);
        free(f2);
    }

}

void le_funcionarios(FILE *in) {
    printf("\n\nLendo funcionários do arquivo...\n\n");
    rewind(in);
    TFunc *f;
    while ((f = le(in)) != NULL) {
        imprime(f);
        free(f);
    }
}

void le_segundo_funcionario(FILE *in) {
    printf("\n\nLendo segundo funcionário do arquivo...\n\n");
    //tamanho() indica quantos bytes vamos pular, o que aqui é igual ao tamanho de um registro
    //(vamos pular o primeiro e nos posicionar no início do segundo)
    //** ATENÇÃO: não usar sizeof(Funcionario), pois ele pode retornar valor maior que o tamanho ocupado em disco,
    //            devido a alinhamento automático (ver https://en.wikipedia.org/wiki/Data_structure_alignment))
    //SEEK_SET indica o início do arquivo
    //ao final, o cursor estará posicionado em 0 + tamanho() +1
    fseek(in, tamanho(), SEEK_SET);
    TFunc *f = le(in);
    if (f != NULL) {
        imprime(f);
        free(f);
    }
}

void adiciona_funcionario(FILE *in) {
    printf("\n\nAdicionando funcionário no final do arquivo...\n\n");
    //pula 5 registros para posicionar no início do final do arquivo
    fseek(in, tamanho() * 5, SEEK_SET);
    TFunc *f = funcionario(6, "Bruna", "666.666.666-66", "06/06/1980", 2500);
    salva(f, in);
    free(f);

    //lê funcionário que acabou de ser gravado
    //posiciona novamente o cursor no início desse registro
    fseek(in, tamanho() * 5, SEEK_SET);
    TFunc *f6 = le(in);
    if (f6 != NULL) {
        imprime(f6);
        free(f6);
    }
}

void sobrescreve_quarto_funcionario(FILE *in) {
    printf("\n\nSobrescrevendo quarto funcionário do arquivo...\n\n");
    //pula primeiros 3 registros para posicionar no início do quarto registro
    fseek(in, tamanho() * 3, SEEK_SET);
    TFunc *f4 = funcionario(7, "Catarina", "777.777.777-77", "07/07/1990", 5000);
    salva(f4, in);
    free(f4);

    //lê funcionário que acabou de ser gravado
    //posiciona novamente o cursor no início desse registro
    fseek(in, tamanho() * 3, SEEK_SET);
    TFunc *f = le(in);
    if (f != NULL) {
        imprime(f);
        free(f);
    }
}

int main(int argc, char** argv) {
    //declara ponteiro para arquivo
    FILE *out;
    //abre arquivo
    if ((out = fopen("funcionario.dat", "w+b")) == NULL) {
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }
    else {
        //insere funcionários
        insere_1000_funcionarios(out);
        //volta ao início do arquivo e lê os funcionários inseridos
        le_funcionarios(out);
        //volta ao início do arquivo e lê o segundo funcionário
        le_segundo_funcionario(out);
        //grava mais um funcionário no final do arquivo
        adiciona_funcionario(out);
        //sobrescreve quarto funcionário
        sobrescreve_quarto_funcionario(out);
        //lendo o arquivo todo novamente
        le_funcionarios(out);

        //fecha arquivo
        fclose(out);
    }
}
