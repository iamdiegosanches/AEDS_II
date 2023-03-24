#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hash_exterior.h"
#include "compartimento.h"
#include "funcionario.h"

int hash(int cod, int tam) {
    return cod % tam;
}

void cria_hash(char *nome_arquivo_hash, int tam) {
    FILE *arq;
    CompartimentoHash *Compartimento = (CompartimentoHash *) malloc(sizeof(CompartimentoHash));
    if ((arq = fopen(nome_arquivo_hash, "wb")) == NULL) {
        printf("Erro ao abrir arquivo!");
    } else {
        for (int i = 0; i < tam; i++) {
            Compartimento = compartimento_hash(-1);
            salva_compartimento(Compartimento, arq);
        }
    }
    fclose(arq);
}

int insere(TFunc *func, FILE *arq_hash, char *nome_arquivo_dados, int tam) {
    int h = hash(func->cod, tam);
    if(h == 0) {
        rewind(arq_hash);
    } else {
        fseek(arq_hash, tamanho_compartimento() * (h), SEEK_SET);
    }
    CompartimentoHash *comp = le_compartimento(arq_hash);
    // Compartimento vazio
    if(comp->prox == -1) {
        FILE *arq_dado = fopen(nome_arquivo_dados, "a+b");
        if (arq_dado == NULL) {
            printf("Erro ao abrir o arquivo");
            exit(1);
        }
        // Salva o funcionario no arquivo de dados
        salva(func, arq_dado);
        // Atualiza o compartimento com a localização do funcionario no arquivo de dados
        comp->prox = tamanho_arquivo(arq_dado) - 1;
        fseek(arq_hash, tamanho_compartimento() * (h), SEEK_SET);
        salva_compartimento(comp, arq_hash);
        fclose(arq_dado);
        free(comp);
        return 1;
    } else { // Compartimento já tem conteúdo
        FILE *arq_dado = fopen(nome_arquivo_dados, "r+b");
        if (arq_dado == NULL) {
            printf("Erro ao abrir o arquivo");
            exit(1);
        }
        if(comp->prox == 0) {
            rewind(arq_dado);
        } else {
            fseek(arq_dado, tamanho_registro() * comp->prox, SEEK_SET);
        }
        // Variavel para atualizar o prox do funcionario
        int pos = comp->prox;
        while (1) {
            TFunc *f = le(arq_dado);
            if (f->prox == -1) { // O proximo espaço no final da lista esta vazio
                fseek(arq_dado, 0, SEEK_END);
                salva(func, arq_dado);
                f->prox = tamanho_arquivo(arq_dado) - 1;
                if (pos == 0) {
                    rewind(arq_dado);
                } else {
                    fseek(arq_dado, tamanho_registro() * pos, SEEK_SET);
                }
                // Atualiza o prox do funcionario anterior
                salva(f, arq_dado);
                free(f);
                fclose(arq_dado);
                return 1;
            } else {
                // Continua buscando pelo proximo lugar na lista encadeada
                fseek(arq_dado, tamanho_registro() * f->prox, SEEK_SET);
                pos = f->prox;
            }
        }
    }
}

TFunc *busca(int cod, FILE *arq_hash, char *nome_arquivo_dados, int tam){
    TFunc *f_aux = funcionario(-1, "Ana", 3000, -1);
    // Medir o tempo de execução e comparações
    int comparacoes = 0;
    clock_t start, end;
    double time_spent = 0.0;
    start = clock();
    // Caso em que o codigo é menor que 0
    if(cod < 0) {
        end = clock();
        time_spent += (double)(end - start) / CLOCKS_PER_SEC;
        FILE *out = fopen("saida_selecao.txt", "w");
        fprintf(out, "Numero comparacoes: %d\n", comparacoes);
        fprintf(out, "Tempo gasto: %f", time_spent);
        fclose(out);
        printf("Comparacoes: %d\n", comparacoes);
        printf("Tempo de execucao da busca: %f\n", time_spent);
        return f_aux;
    }
    // Calcula o hash
    int h = hash(cod, tam);
    if(h == 0) {
        rewind(arq_hash);
    } else {
        fseek(arq_hash, tamanho_compartimento() * (h), SEEK_SET);
    }
    CompartimentoHash *comp = le_compartimento(arq_hash);
    // Se o compartimento está vazio o funcionario não está lá
    if(comp->prox == -1) {
        end = clock();
        comparacoes++;
        time_spent += (double)(end - start) / CLOCKS_PER_SEC;
        FILE *out = fopen("saida_selecao.txt", "w");
        fprintf(out, "Numero comparacoes: %d\n", comparacoes);
        fprintf(out, "Tempo gasto: %f", time_spent);
        fclose(out);
        printf("Comparacoes: %d\n", comparacoes);
        printf("Tempo de execucao da busca: %f\n", time_spent);
        return f_aux;
    } else {
        FILE *arq_dado = fopen(nome_arquivo_dados, "r+b");
        if (arq_dado == NULL) {
            printf("Erro ao abrir o arquivo!\n");
            exit(1);
        }
        if(comp->prox == 0) {
            rewind(arq_dado);
        } else {
            fseek(arq_dado, tamanho_registro() * comp->prox, SEEK_SET);
        }
        while (1) {
            TFunc *f = le(arq_dado);
            // Se chegou no final da lista e este não é o funcionario a ser buscado termina a busca
            if (f->prox == -1 && f->cod != cod) {
                fclose(arq_dado);
                free(f);
                // Calcular o tempo de execução
                end = clock();
                comparacoes++;
                time_spent += (double)(end - start) / CLOCKS_PER_SEC;
                FILE *out = fopen("saida_selecao.txt", "w");
                fprintf(out, "Tempo gasto: %f", time_spent);
                fprintf(out, "Numero comparacoes: %d\n", comparacoes);
                fclose(out);
                printf("Comparacoes: %d\n", comparacoes);
                printf("Tempo de execucao da busca: %f\n", time_spent);

                return f_aux;
            }
            // Se encontra o funcionario
            comparacoes++;
            if (f->cod == cod) {
                fclose(arq_dado);

                end = clock();
                time_spent += (double)(end - start) / CLOCKS_PER_SEC;
                FILE *out = fopen("saida_selecao.txt", "w");
                fprintf(out, "Numero comparacoes: %d\n", comparacoes);
                fprintf(out, "Tempo gasto: %f\n", time_spent);
                fclose(out);
                printf("Comparacoes: %d\n", comparacoes);
                printf("Tempo de execucao: %f\n", time_spent);
                return f;
            } else { // Continua buscando o proximo da lista
                fseek(arq_dado, tamanho_registro() * f->prox, SEEK_SET);
            }
        }
    }
}

int exclui(int cod, FILE *arq_hash, char *nome_arquivo_dados, int tam) {
    if(cod < 0) {
        return -1;
    }
    int h = hash(cod, tam);
    if(h == 0) {
        rewind(arq_hash);
    } else {
        fseek(arq_hash, tamanho_compartimento() * (h), SEEK_SET);
    }
    CompartimentoHash *comp = le_compartimento(arq_hash);
    if(comp->prox == -1) {
        return -1;
    } else {
        FILE *arq_dado = fopen(nome_arquivo_dados, "r+b");
        if (arq_dado == NULL) {
            printf("Erro ao abrir o arquivo!\n");
            exit(1);
        }
        if(comp->prox == 0) {
            rewind(arq_dado);
        } else {
            fseek(arq_dado, tamanho_registro() * comp->prox, SEEK_SET);
        }
        int flag = 1;
        int pos = comp->prox;

        while (1) {
            TFunc *f = le(arq_dado);
            // Funcionario nao esta no arquivo
            if (f->prox == -1 && f->cod != cod) {
                fclose(arq_dado);
                free(f);
                return -1;
            }
            // primeiro caso: Funcionario primeiro da lista atualiza o compartimento
            if (f->cod == cod && flag == 1) {
                if(h == 0) {
                    rewind(arq_hash);
                } else {
                    fseek(arq_hash, tamanho_compartimento() * (h), SEEK_SET);
                }
                comp->prox = f->prox;
                salva_compartimento(comp, arq_hash);
                free(comp);
                free(f);
                fclose(arq_dado);
                return 1;
            }
            // segundo caso: Funcionario no meio ou no final da lista
            if (f->cod == cod) { // Se ele foi encontrado
                TFunc *f_aux = funcionario(f->cod, f->nome, f->salario, f->prox);
                if (pos == 0) {
                    rewind(arq_dado);
                    f = le(arq_dado);
                } else {
                    fseek(arq_dado, tamanho_registro() * pos, SEEK_SET);
                    f = le(arq_dado);
                }
                // Faz a ponte para o proximo elemento
                f->prox = f_aux->prox;
                if (pos == 0) {
                    rewind(arq_dado);
                } else {
                    fseek(arq_dado, tamanho_registro() * pos, SEEK_SET);
                }
                // Atualiza o elemento na lista
                salva(f, arq_dado);
                free(f);
                free(f_aux);
                fclose(arq_dado);
                return 1;
            } else { // Continua buscando o funcionario na lista mantendo o pos no funcionario anterior aquela a ser deletado
                fseek(arq_dado, tamanho_registro() * f->prox, SEEK_SET);
                // Vericar se pode passar o prox de funcionario
                TFunc *f_aux = le(arq_dado);
                int aux = pos;
                if (f_aux->cod != cod) {
                    if (f_aux != NULL) {
                        if(f_aux->cod != cod) {
                            pos = f->prox;
                        }
                    }
                    free(f_aux);
                }
                fseek(arq_dado, tamanho_registro() * f->prox, SEEK_SET);
            }
            flag = 0;
        }
    }
}