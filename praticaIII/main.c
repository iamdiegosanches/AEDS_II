#include <stdlib.h>
#include <string.h>


#include "funcionario.h"
#include "hash_exterior.h"
#include "compartimento.h"

#define TAM 27

void MSG_MENU() {
    printf("\n\t>>>>>>>>>>>>>>>>>>>>>>> OPCOES DE MENU <<<<<<<<<<<<<<<<<<<<<<<<");
    printf("\n\n\t1. INSERIR 1000 FUNCIONARIOS");
    printf("  \n\t2. INSERIR 1 FUNCIONARIO");
    printf("  \n\t3. PESQUISAR FUNCIONARIO");
    printf("  \n\t4. REMOVER 1 FUNCIONARIO");
    printf("  \n\t5. IMPRIMIR TABELA");
    printf("  \n\t0. SAIR");
}


int main() {
    FILE *arq = fopen("funcionarios.dat", "w+b");
    FILE *arq_hash = fopen("hash.dat", "w+b");
    cria_hash("hash.dat", TAM);
    int opt;
    TFunc *func_teste = funcionario(11, "A", 3000, -1);
    int codigo;
    char nome[50];
    float salario;

    do {
        MSG_MENU();
        printf("\n\nDigite uma opcao: ");
        fflush(stdin);
        scanf("%d", &opt);
        system("cls");
        switch (opt) {
            case 0:
                system("cls");
                printf("\n<<<<<<<<< SAINDO DO PROGRAMA >>>>>>>>>>\n");
                break;
            case 1:
                printf("\n<<<<<<<<< INSERINDO 1000 FUNCIONARIOS SEQUENCIALMENTE >>>>>>>>>>\n");
                for (int i = 1; i <= 1000; ++i) {
                    func_teste = funcionario(i, "Ana", 3000, -1);
                    insere(func_teste, arq_hash, "funcionarios.dat", TAM);
                }
                system("cls");
                break;
            case 2:
                printf("\n<<<<<<<<< INSERINDO 1 FUNCIONARIO >>>>>>>>>>\n");
                printf("Digite o codigo: ");
                fflush(stdin);
                scanf("%d", &codigo);

                printf("Digite o nome: ");
                fflush(stdin);
                fgets(nome, 50, stdin);
                nome[strcspn(nome, "\n")] = '\0';

                printf("Digite o salario: ");
                fflush(stdin);
                scanf("%f", &salario);

                insere(funcionario(codigo, nome, salario, -1), arq_hash, "funcionarios.dat", TAM);
                system("cls");
                break;
            case 3:
                printf("\n<<<<<<<<< PESQUISANDO 1 FUNCIONARIO >>>>>>>>>>\n");
                printf("Digite o codigo do funcionario a ser pesquisado: ");
                fflush(stdin);
                scanf("%d", &codigo);

                TFunc *busc = busca(codigo, arq_hash, "funcionarios.dat", TAM);

                if(busc->cod != -1) {
                    imprime(busc);
                } else {
                    printf("\nFuncionario NAO esta na tabela.\n");
                }

                system("PAUSE");
                system("cls");
                break;
            case 4:
                printf("\n<<<<<<<<< REMOVENDO 1 FUNCIONARIO >>>>>>>>>>\n");
                printf("Digite o codigo do funcionario a ser pesquisado: ");
                fflush(stdin);
                scanf("%d", &codigo);

                int r = exclui(codigo, arq_hash, "funcionarios.dat", TAM);

                if(r == 1) {
                    printf("\nFuncionario removido com sucesso.\n");
                } else {
                    printf("\nFuncionario nao foi removido.\n");
                }

                system("PAUSE");
                system("cls");
                break;
            case 5:
                //le o arquivo e coloca no vetor
                rewind(arq_hash); //posiciona cursor no inicio do arquivo
                CompartimentoHash *c = le_compartimento(arq_hash);
                int x = 0;
                while (!feof(arq_hash))
                {
                    printf("\n\n<<<<<<<<<<<<<<<<<<<<<<<<<<Compartimento %d>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n", x);
                    if (c->prox == -1) {
                        c = le_compartimento(arq_hash);
                        x++;
                        continue;
                    }
                    fseek(arq, tamanho_registro() * (c->prox), SEEK_SET);
                    func_teste = le(arq);
                    imprime(func_teste);
                    while(func_teste->prox != -1) {
                        fseek(arq, tamanho_registro() * (func_teste->prox), SEEK_SET);
                        func_teste = le(arq);
                        imprime(func_teste);
                    }
                    c = le_compartimento(arq_hash);
                    x++;
                }

                system("PAUSE");
                system("cls");
                break;
            default:
                printf("\t<<<<Digite uma opcao valida>>>>\n");
        }
    }while(opt != 0);

    free(func_teste);
    fclose(arq);
    fclose(arq_hash);
    return 0;
}
