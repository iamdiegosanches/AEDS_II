#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "funcionario.c"
#include "intercalacao.h"
#include "geracao_particoes.h"
#include "lista.h"

//Constante para o numero de funcionarios
#define NFUNC 5000

void sobrescreve_funcionario(FILE *in, int n, TFunc *f1)
{
    //pula primeiros n registros para posicionar no início do quarto registro
    fseek(in, tamanho_registro() * n, SEEK_SET);
    salva(f1, in);
}

// Fisher-yates algoritmo de aleatorizacao
void shuffle(FILE *in, int n)
{
    // Valores diferentes todas as vezes
    srand (time(NULL));

    TFunc *f1 = NULL;
    TFunc *f2 = NULL;

    for (int i = n-1; i > 0; i--)
    {
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

void insere_5000_funcionarios(FILE *out)
{
    for(int i = 1; i <= NFUNC; i++)
    {
        TFunc *f1 = funcionario(i, "Ana", 3000+i);
        salva(f1, out);
        free(f1);
    }
    shuffle(out, NFUNC);
}

void imprime_arquivo(FILE *arq)
{
    //le o arquivo e coloca no vetor
    rewind(arq); //posiciona cursor no inicio do arquivo
    TFunc *f = le(arq);
    while (!feof(arq))
    {
        imprime(f);
        f = le(arq);
    }
}

void busca_sequencial(FILE *in, int target) {
    TFunc *f;
    int comparacoes = 0;
    double tempo_gasto = 0.0;
    clock_t inicio = clock();
    for (int i = 0; i < NFUNC; i++) {
        fseek(in, tamanho_registro() * i, SEEK_SET);
        f = le(in);
        comparacoes++;
        if (f != NULL && f->cod == target) {
            imprime(f);
            clock_t fim = clock();
            tempo_gasto = (double)(fim - inicio) / CLOCKS_PER_SEC;
            // Gravar em arquivo as infos do funcionario encontrado,
            // juntamente com a quantidade de comparações realizadas e o tempo gasto para localizar o funcionarios
            FILE *out = fopen("saida_sequencial.txt", "w");
            fwrite(f, sizeof(TFunc), 1, out);
            fwrite(&comparacoes, sizeof(int), 1, out);
            fwrite(&tempo_gasto, sizeof(double), 1, out);
            fclose(out);
            printf("Numero de Comparacoes: %d\n", comparacoes);
            printf("Tempo gasto na execucao: %f segundos\n", tempo_gasto);
            return;
        }
    }
    free(f);
}

TFunc *busca_binaria(int chave, FILE *in, int inicio, int fim)
{
    TFunc *f = NULL;
    int cod = -1;
    int comparacoes = 0;
    double tempo_gasto = 0.0;
    clock_t start = clock();
    while (inicio <= fim && cod != chave)
    {
        int meio = trunc((inicio + fim) / 2);
        // printf("Inicio: %d; Fim: %d; Meio: %d\n", inicio, fim, meio);
        fseek(in, (meio -1) * tamanho_registro(), SEEK_SET);
        f = le(in);
        cod = f->cod;
        comparacoes++;
        if (f) {
            if (cod > chave) {
                fim = meio - 1;
            }
            else {
                inicio = meio + 1;
            }
        }
    }
    clock_t end = clock();
    tempo_gasto += (double)(end - start) / CLOCKS_PER_SEC;
    if (cod == chave)
    {
        FILE *out = fopen("saida_binaria.txt", "w");
        fprintf(out, "Codigo: %d\n Nome: %s \n Salario: %f", f->cod, f->nome, f->salario);
        fprintf(out, "\nNumero comparacoes: %d\n", comparacoes);
        fprintf(out, "Tempo gasto: %f", tempo_gasto);

        fclose(out);
        imprime(f);
        printf("\nNumero de Comparacoes: %d\n", comparacoes);
        printf("Tempo gasto na execucao: %f segundos\n", tempo_gasto);
        return f;
    }
    else return NULL;
}

// Falta salvar em arquivo o numero de comparações e o tempo gasto
void insertion_sort_disco(FILE *arq, int tam)
{
    int i;
    int comparacoes = 0;
    clock_t start, end;
    double tempo_gasto = 0.0;

    start = clock();
    //faz o insertion sort
    for (int j = 2; j <= tam; j++)
    {
        //posiciona o arquivo no registro j
        fseek(arq, (j-1) * tamanho_registro(), SEEK_SET);
        TFunc *fj = le(arq);
        i = j - 1;
        //posiciona o cursor no registro i
        fseek(arq, (i-1) * tamanho_registro(), SEEK_SET);
        TFunc *fi = le(arq);
        while ((i > 0) && (fi->cod > fj->cod))
        {
            comparacoes++;
            //posiciona o cursor no registro i+1
            fseek(arq, i * tamanho_registro(), SEEK_SET);
            //printf("Salvando funcionario %d na posicao %d\n", fi->cod, i+1);
            salva(fi, arq);
            i = i - 1;
            //lÃª registro i
            fseek(arq, (i-1) * tamanho_registro(), SEEK_SET);
            fi = le(arq);
        }
        //posiciona cursor no registro i + 1
        fseek(arq, (i) * tamanho_registro(), SEEK_SET);
        //salva registro j na posiÃ§Ã£o i
        salva(fj, arq);
    }
    //descarrega o buffer para ter certeza que dados foram gravados
    fflush(arq);
    end = clock();
    tempo_gasto += (double)(end - start) / CLOCKS_PER_SEC;

    FILE *out = fopen("saida_insertion_sort.txt", "w");
    fprintf(out, "Numero comparacoes: %d\n", comparacoes);
    fprintf(out, "Tempo gasto: %f", tempo_gasto);
    fclose(out);
    printf("Numero de Comparacoes: %d\n", comparacoes);
    printf("Tempo gasto na execucao: %f segundos\n", tempo_gasto);

}

void insertion_sort(FILE *arq, int tam)
{
    TFunc *v[tam - 1];
    //le o arquivo e coloca no vetor
    rewind(arq); //posiciona cursor no inicio do arquivo
    TFunc *f = le(arq);
    int i = 0;
    while (!feof(arq))
    {
        v[i] = f;
        f = le(arq);
        i++;
    }
    //faz o insertion sort
    for (int j = 1; j < tam; j++)
    {
        TFunc *f = v[j];
        i = j - 1;
        while ((i >= 0) && (v[i]->cod > f->cod))
        {
            v[i + 1] = v[i];
            i = i - 1;
        }
        v[i+1] = f;
    }
    //grava vetor no arquivo, por cima do conteudo anterior
    rewind(arq);
    for (int i = 0; i < tam; i++)
    {
        salva(v[i], arq);
    }
    //descarrega o buffer para ter certeza que dados foram gravados
    fflush(arq);
}

void MSG_MENU()
{
    printf("\n\t>>>>>>>>>>>>>>>>>>>>>>> OPCOES DE MENU <<<<<<<<<<<<<<<<<<<<<<<<");
    printf("\n\n\t1. IMPRIMIR");
    printf("  \n\t2. INSERTION SORT DISCO");
    printf("  \n\t3. INSERTION SORT MEMORIA");
    printf("  \n\t4. BUSCA SEQUENCIAL");
    printf("  \n\t5. ORDENACAO EXTERNA");
    printf("  \n\t6. BUSCA BINARIA (SOMENTE QUANDO ARQUIVO ESTA ORGANIZADO)");
    printf("  \n\t0. SAIR");
}

void MENU(FILE *in)
{
    int opcao = -1;
    int cod = -1;
    do
    {
        MSG_MENU();
        printf("\n\nDigite uma opcao: ");
        fflush(stdin);
        scanf("%d", &opcao);
        switch(opcao)
        {
        case 1:
            imprime_arquivo(in);
            break;
        case 2:
            insertion_sort_disco(in, NFUNC);
            printf("Arquivo organizado com sucesso.\n");
            break;
        case 3:
            insertion_sort(in, NFUNC);
            break;
        case 4:
            printf("Digite o codigo do funcionario que deseja pesquisar: ");
            fflush(stdin);
            scanf("%d", &cod);
            busca_sequencial(in, cod);
            break;
        case 6:
            printf("Digite o codigo do funcionario que deseja pesquisar: ");
            fflush(stdin);
            scanf("%d", &cod);
            busca_binaria(cod, in, 1, NFUNC);
            break;
        case 5:
            fclose(in);
            Lista *lst = NULL;
            //lst = cria_nomes(cria_str("p1.dat"), cria_nomes(cria_str("p2.dat"),  NULL));
            lst = cria_nomes(cria_str("p1.dat"),
                             cria_nomes(cria_str("p2.dat"),
                                        cria_nomes(cria_str("p3.dat"),
                                                   cria_nomes(cria_str("p4.dat"),NULL))));

            selecao_com_substituicao("funcionario.dat", lst, 800);
            intercalacao_otima("funcionario.dat", 4, lst, 5);

            printf("\nArquivo ordenado com sucesso.");
            libera_nomes(lst);
            break;
        case 0:
            break;
        }
    }
    while(opcao != 0);
}

int main(int argc, char** argv)
{
    FILE *out;
    if ((out = fopen("funcionario.dat", "w+b")) == NULL)
    {
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }
    else
    {
        insere_5000_funcionarios(out);

        MENU(out);

        fclose(out);
    }
}
