#include "intercalacao.h"
#include "lista.h"
#include "funcionario.h"
#include <limits.h>
#include <time.h>

typedef struct vetor{
    TFunc *func;
    FILE *f;
    int aux_p;
}TVet;


void intercalacao_basico(char *nome_arquivo_saida, int num_p, Lista *nome_particoes) {

    int fim = 0; //variavel que controla fim do procedimento
    FILE *out; //declara ponteiro para arquivo

    //abre arquivo de saida para escrita
    if ((out = fopen(nome_arquivo_saida, "wb")) == NULL) {
        printf("Erro ao abrir arquivo de sa?da\n");
    } else {
        //cria vetor de particoes
        TVet v[num_p];

        //abre arquivos das particoes, colocando variavel de arquivo no campo f do vetor
        //e primeiro funcionario do arquivo no campo func do vetor
        for (int i=0; i < num_p; i++) {
            v[i].f = fopen(nome_particoes->nome, "rb");
            v[i].aux_p = 0;
            if (v[i].f != NULL) {
                fseek(v[i].f, v[i].aux_p * tamanho_registro(), SEEK_SET);
                TFunc *f = le(v[i].f);
                if (f == NULL) {
                    //arquivo estava vazio
                    //coloca HIGH VALUE nessa posi??o do vetor
                    v[i].func = funcionario(INT_MAX, "",0);
                }
                else {
                    //conseguiu ler funcionario, coloca na posi??o atual do vetor
                    v[i].func = f;
                }
            }
            else {
                fim = 1;
            }
            nome_particoes = nome_particoes->prox;
        }

        int aux = 0;
        while (!(fim)) { //conseguiu abrir todos os arquivos
            int menor = INT_MAX;
            int pos_menor;
            //encontra o funcionario com menor chave no vetor
            for(int i = 0; i < num_p; i++){
                if(v[i].func->cod < menor){
                    menor = v[i].func->cod;
                    pos_menor = i;
                }
            }
            if (menor == INT_MAX) {
                fim = 1; //terminou processamento
            }
            else {
                //salva funcionario no arquivo de saÃ¬da
                fseek(out, aux * tamanho_registro(), SEEK_SET);
                salva(v[pos_menor].func, out);
                //atualiza posiÃ§Ã£o pos_menor do vetor com pr?ximo funcionario do arquivo
                v[pos_menor].aux_p++;
                fseek(v[pos_menor].f, v[pos_menor].aux_p * tamanho_registro(), SEEK_SET);
                TFunc *f = le(v[pos_menor].f);
                aux++;
                if (f == NULL) {
                    //arquivo estava vazio
                    //coloca HIGH VALUE nessa posiÃ§ao do vetor
                    v[pos_menor].func = funcionario(INT_MAX, "", 0.0);
                }
                else {
                    v[pos_menor].func = f;
                }

            }
        }

        //fecha arquivos das partiÃ‡Ãµes de entrada
        for(int i = 0; i < num_p; i++){
            fclose(v[i].f);
            //    free(v[i].func);
        }
        //fecha arquivo de saÃ¬da
        fclose(out);
    }
}

void intercalacao_otima(char *nome_arquivo_saida, int num_p, Lista *nome_particoes, int f) {
    int n = num_p;
    int i = num_p;
    char nome_arquivo[50];
    clock_t start, end;
    double tempo_gasto = 0.0;
    start = clock();
    Lista *temp = NULL;
    // retirar o primeiro nome da lista
    while(i > f-1) {
        Lista *aux = cria_nomes(cria_str("exception"), NULL);
        for(int j = 0; j < f-1; j++) {
            temp = nome_particoes->prox;
            adiciona_nomes(aux, nome_particoes->nome);
            free(nome_particoes->nome);
            free(nome_particoes);
            nome_particoes = temp;
            i--;
        }
        temp = aux->prox;
        free(aux->nome);
        free(aux);
        aux = temp;
        n++;
        sprintf(nome_arquivo, "p%d.dat", n);
        intercalacao_basico(nome_arquivo, f-1, aux);
        adiciona_nomes(nome_particoes, nome_arquivo);
        libera_nomes(aux);
        i++;
    }
    // Ordenação final
    intercalacao_basico(nome_arquivo_saida, i, nome_particoes);

    // Tempo de execucao em arquivo
    end = clock();
    tempo_gasto += (double)(end - start) / CLOCKS_PER_SEC;
    FILE *out = fopen("saida_intercalacao.txt", "w");
    fprintf(out, "Tempo gasto intercalacao: %f", tempo_gasto);
    fclose(out);
    printf("\nTempo gasto na execucao na intercalacao: %f segundos\n", tempo_gasto);
    // Apagar as particoes
}