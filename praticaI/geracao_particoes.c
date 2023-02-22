#include "geracao_particoes.h"
#include <stdio.h>
#include <time.h>

void selecao_com_substituicao(char * nome_arquivo_entrada, Lista * nome_arquivos_saida, int M) {
    int comparacoes = 0;
    FILE * arq; //pointer to input file
    int fim = 0;

    clock_t start, end;
    double time_spent = 0.0;
    start = clock();

    //open input file for reading
    if ((arq = fopen(nome_arquivo_entrada, "rb")) == NULL) {
        printf("Error opening input file\n");
    } else {
        int i = 0;
        //Ler primeiro funcionario
        TFunc *cin = le(arq);

        // Vetor para guardar os funcionarios do arquivo
        TFunc *v[M];
        int frozens[M];
        for (i = 0; i < M; i++) {
            frozens[i] = 0;
        }

        //Ler M registros do arquivo para a memória
        i = 0;
        while (!feof(arq) && i < M) {
            v[i] = cin;
            cin = le(arq);
            i++;
        }

        //adjust M if input file ended before filling the array v
        if (i != M) {
            M = i;
        }

        int n_frozen = 0;
        TFunc *menor;
        //open output partition for writing
        while (!fim) {
            char *nome_particao = nome_arquivos_saida->nome;
            nome_arquivos_saida = nome_arquivos_saida->prox;
            FILE *p;
                if ((p = fopen(nome_particao, "wb")) == NULL) {
                    printf("Error creating output partition\n");
                } else {
                while (M - n_frozen > 0) {
                    i = 0;
                    int ind_min = 0;
                    // Tratar exececao de encontrar algum funcionario NULL ou Funcinoario congelado nas primeiras posicoes
                    while (v[i] == NULL || frozens[i] == 1) {
                        ind_min++;
                        i++;
                    }
                    // Encontrar funcionario com mento codigo
                    for (int j = 0; j < M; j++) {
                        if(v[j] == NULL) {
                            continue;
                        }
                        if (v[j]->cod < v[ind_min]->cod && frozens[j] == 0) {
                            comparacoes++;
                            ind_min = j;
                        }
                    }
                    menor = v[ind_min];

                    //imprime(menor);
                    salva(menor, p);

                    // Substituir funcionario no vetor caso estamos no final do arquivo este sera nulo
                    if (cin != NULL) {
                        v[ind_min] = cin;
                        cin = le(arq);
                    } else {
                        v[ind_min] = NULL;
                        n_frozen++;
                    }

                    // Congelar funcionario
                    if (v[ind_min] != NULL) {
                        if (v[ind_min]->cod < menor->cod) {
                            frozens[ind_min] = 1;
                            n_frozen++;
                        }
                        continue;
                    }
                }
            }

            // Fechar particao
            fclose(p);
            // Descongelar
            for (i = 0; i < M; i++) {
                if(v[i] == NULL)
                    continue;
                frozens[i] = 0;
                n_frozen--;
            }

            // Se estiver no final do arquivo e tudo resolvido
            if (feof(arq) && n_frozen == M) {
                fim = 1;
            }
        }
    }
    // Calculando tempo e comparacoes
    end = clock();
    time_spent += (double)(end - start) / CLOCKS_PER_SEC;

    FILE *out = fopen("saida_selecao.txt", "w");
    fprintf(out, "Numero comparacoes: %d\n", comparacoes);
    fprintf(out, "Tempo gasto: %f", time_spent);
    fclose(out);

    printf("Comparacoes: %d\n", comparacoes);
    printf("Tempo de execucao: %f", time_spent);
}
