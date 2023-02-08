#include "geracao_particoes.h"
#include "funcionario.h"
//#include "intercalacao.h"

void classificacao_interna(char *nome_arquivo_entrada, Lista *nome_arquivos_saida, int M) {

    int fim = 0; //variável de controle para saber se arquivo de entrada terminou
    FILE *arq; //declara ponteiro para arquivo

    //abre arquivo para leitura
    if ((arq = fopen(nome_arquivo_entrada, "rb")) == NULL) {
        printf("Erro ao abrir arquivo de entrada\n");
    } else {

        //le primeiro cliente
        TFunc *cin = le(arq);

        while (!(fim)) {
            //le o arquivo e coloca no vetor
            TFunc *v[M];

            int i = 0;
            while (!feof(arq) && i < M) {
                v[i] = cin;
                cin = le(arq);
                i++;
            }

            //ajusta tamanho M caso arquivo de entrada tenha terminado antes do vetor
            if (i != M) {
                M = i;
            }

            //faz ordenacao
            for (int j = 1; j < M; j++) {
                TFunc *c = v[j];
                i = j - 1;
                while ((i >= 0) && (v[i]->cod > c->cod)) {
                    v[i + 1] = v[i];
                    i = i - 1;
                }
                v[i + 1] = c;
            }

            //cria arquivo de particao e faz gravacao
            char *nome_particao = nome_arquivos_saida->nome;
            nome_arquivos_saida = nome_arquivos_saida->prox;

            printf("\n%s\n", nome_particao);

            FILE *p;
            if ((p = fopen(nome_particao, "wb")) == NULL) {
                printf("Erro criar arquivo de saida\n");
            } else {
                for (int i = 0; i < M; i++) {
                    salva(v[i], p);
                    imprime(v[i]);
                }
                fclose(p);
            }
            if (feof(arq)) {
                fim = 1;
            }
        }
    }
}

void selecao_com_substituicao(char *nome_arquivo_entrada, Lista *nome_arquivos_saida, int M)
{
    return;
}

void selecao_com_substituicao(char * nome_arquivo_entrada, Lista * nome_arquivos_saida, int M) {
    FILE * arq; //pointer to input file
    int fim = 0;

    //open input file for reading
    if ((arq = fopen(nome_arquivo_entrada, "rb")) == NULL) {
        printf("Error opening input file\n");
    } else {
        //Ler primeiro funcionario
        TFunc *cin = le(arq);

        TFrozen *v[M]; //array to store records from input file
        for (int i = 0; i < M; i++) {
            v[i] = (TFrozen*)malloc(sizeof(TFrozen));
        }
        int i = 0;

        //Ler M registros do arquivo para a memória
        while (!feof(arq) && i < M) {
            v[i]->func = cin;
            cin = le(arq);
            i++;
        }

        //adjust M if input file ended before filling the array v
        if (i != M) {
            M = i;
        }

        int frozen = 0;
        TFunc *menor;
        //Loop enquanto ainda tem registro descongelados
        //open output partition for writing
        while (!fim) {
            char *nome_particao = nome_arquivos_saida->nome;
            nome_arquivos_saida = nome_arquivos_saida->prox;
            FILE *p;

            while (M - frozen > 0) {
                if ((p = fopen(nome_particao, "wb")) == NULL) {
                    printf("Error creating output partition\n");
                } else {
                    //find the record with the smallest key
                    int ind_min = 0;
                    for (int j = 1; j < M; j++) {
                        if (v[j]->func->cod < v[ind_min]->func->cod && !v[j]->frozen) {
                            ind_min = j;
                        }
                    }
                    menor = v[ind_min]->func;
                    //write the record with the smallest key to output partition
                    salva(menor, p);
                    
                    if (cin != NULL) {
                        v[ind_min] = le(arq);
                        cin = le_arq(arq);
                    }

                    //replace the record with the smallest key with the next record from input file
                    if (!feof(arq)) {
                        if (cin != NULL) {
                            if (v[ind_min]->cod < menor->cod) {
                                v[ind_min]->frozen = 1;
                                frozen++;
                            }
                        }

                        continue;
                    } else if (feof(arq) && M - frozen > 0) {
                        v[ind_min]->frozen = 1;
                        continue;
                    }
                    //close output partition
                    frozen++;
                    fclose(p);
                }
                //unfreeze frozen records
                for (i = 0; i < M; i++) {
                    v[i]->frozen = 0;
                }
            }
            if (feof(arq) && frozen < M) {
                fim = 1;
            }
        }
        for (i = 0; i < M; i++) {
            free(v[i]);
        }
    }
}
