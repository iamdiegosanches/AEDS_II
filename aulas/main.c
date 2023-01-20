#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "funcionario.c"

//Constante para o numero de funcionarios
#define NFUNC 100

void insere_1000_funcionarios(FILE *out) {
    for(int i = 1; i <= NFUNC; i++) {
        TFunc *f1 = funcionario(i, "Ana", 3000);
        salva(f1, out);
        free(f1);
    }
}

void sobrescreve_funcionario(FILE *in, int n, TFunc *f1) {
    //pula primeiros n registros para posicionar no início do quarto registro
    fseek(in, tamanho_registro() * n, SEEK_SET);
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

        fseek(in, tamanho_registro() * i, SEEK_SET);
        f1 = le(in);
        fseek(in, tamanho_registro() * j, SEEK_SET);
        f2 = le(in);

        sobrescreve_funcionario(in, i, f2);
        sobrescreve_funcionario(in, j, f1);

        free(f1);
        free(f2);
    }

}

void imprime_arquivo(FILE *arq) {
    //le o arquivo e coloca no vetor
    rewind(arq); //posiciona cursor no inicio do arquivo
    TFunc *f = le(arq);
    while (!feof(arq)) {
        imprime(f);
        f = le(arq);
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
    fseek(in, tamanho_registro(), SEEK_SET);
    TFunc *f = le(in);
    if (f != NULL) {
        imprime(f);
        free(f);
    }
}

void adiciona_funcionario(FILE *in) {
    printf("\n\nAdicionando funcionário no final do arquivo...\n\n");
    //pula 5 registros para posicionar no início do final do arquivo
    fseek(in, tamanho_registro() * NFUNC, SEEK_SET);
    TFunc *f = funcionario(6, "Bruna", 2500);
    salva(f, in);
    free(f);

    //lê funcionário que acabou de ser gravado
    //posiciona novamente o cursor no início desse registro
    fseek(in, tamanho_registro() * 5, SEEK_SET);
    TFunc *f6 = le(in);
    if (f6 != NULL) {
        imprime(f6);
        free(f6);
    }
}

void sobrescreve_quarto_funcionario(FILE *in) {
    printf("\n\nSobrescrevendo quarto funcionário do arquivo...\n\n");
    //pula primeiros 3 registros para posicionar no início do quarto registro
    fseek(in, tamanho_registro() * 3, SEEK_SET);
    TFunc *f4 = funcionario(7, "Catarina", 5000);
    salva(f4, in);
    free(f4);

    //lê funcionário que acabou de ser gravado
    //posiciona novamente o cursor no início desse registro
    fseek(in, tamanho_registro() * 3, SEEK_SET);
    TFunc *f = le(in);
    if (f != NULL) {
        imprime(f);
        free(f);
    }
}

TFunc *busca_binaria(int chave, FILE *in, int inicio, int fim) {
    TFunc *f = NULL;
    int cod = -1;
    while (inicio <= fim && cod != chave) {
        int meio = trunc((inicio + fim) / 2);
        printf("Inicio: %d; Fim: %d; Meio: %d\n", inicio, fim, meio);
        fseek(in, (meio -1) * tamanho_registro(), SEEK_SET);
        f = le(in);
        cod = f->cod;
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
    }
    else return NULL;
}

void insertion_sort_disco(FILE *arq, int tam) {
    int i;
    //faz o insertion sort
    for (int j = 2; j <= tam; j++) {
        //posiciona o arquivo no registro j
        fseek(arq, (j-1) * tamanho_registro(), SEEK_SET);
        TFunc *fj = le(arq);
        printf("\n********* Funcionario atual: %d\n", fj->cod);
        i = j - 1;
        //posiciona o cursor no registro i
        fseek(arq, (i-1) * tamanho_registro(), SEEK_SET);
        TFunc *fi = le(arq);
        printf("fi = %d\n", fi->cod);
        while ((i > 0) && (fi->cod > fj->cod)) {
            //posiciona o cursor no registro i+1
            fseek(arq, i * tamanho_registro(), SEEK_SET);
            printf("Salvando funcionario %d na posicao %d\n", fi->cod, i+1);
            salva(fi, arq);
            i = i - 1;
            //lÃª registro i
            fseek(arq, (i-1) * tamanho_registro(), SEEK_SET);
            fi = le(arq);
            printf("fi = %d; i = %d\n", fi->cod, i);
        }
        //posiciona cursor no registro i + 1
        fseek(arq, (i) * tamanho_registro(), SEEK_SET);
        printf("*** Salvando funcionario %d na posicao %d\n", fj->cod, i+1);
        //salva registro j na posiÃ§Ã£o i
        salva(fj, arq);
    }
    //descarrega o buffer para ter certeza que dados foram gravados
    fflush(arq);
}

void insertion_sort(FILE *arq, int tam) {
    TFunc *v[tam - 1];
    //le o arquivo e coloca no vetor
    rewind(arq); //posiciona cursor no inicio do arquivo
    TFunc *f = le(arq);
    int i = 0;
    while (!feof(arq)) {
        v[i] = f;
        f = le(arq);
        i++;
    }
    //faz o insertion sort
    for (int j = 1; j < tam; j++) {
        TFunc *f = v[j];
        i = j - 1;
        while ((i >= 0) && (v[i]->cod > f->cod)) {
            v[i + 1] = v[i];
            i = i - 1;
        }
        v[i+1] = f;
    }
    //grava vetor no arquivo, por cima do conteÃºdo anterior
    rewind(arq);
    for (int i = 0; i < tam; i++) {
        salva(v[i], arq);
    }
    //descarrega o buffer para ter certeza que dados foram gravados
    fflush(arq);

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

        shuffle(out, NFUNC);

        //volta ao início do arquivo e lê os funcionários inseridos
        imprime_arquivo(out);

        //lendo o arquivo todo novamente
        //le_funcionarios(out);

        //fecha arquivo
        fclose(out);
    }
}

void MSG_MENU()
{
    system("clear");
    printf("\n\n\t>>>>>>>>>>>>>>>>>>>>>>> OPCOES DE MENU <<<<<<<<<<<<<<<<<<<<<<<<");
    printf("\n\n\t1. ENFILEIRAR");
    printf("  \n\t2. PESQUISAR");
    printf("  \n\t3. DESENFILEIRAR");
    printf("  \n\t4. IMPRIMIR");
    printf("  \n\t5. SAIR");
}

void MENU(FILE *in)
{
    int opcao=0;
    MSG_MENU();


}

