#include "arvore-b.h"
#include "queue.h"

void MSG_MENU() {
    printf("\n\t>>>>>>>>>>>>>>>>>>>>>>> OPCOES DE MENU <<<<<<<<<<<<<<<<<<<<<<<<");
    printf("\n\n\t1. PESQUISAR FUNCIONARIO");
    printf("  \n\t2. REMOVER 1 FUNCIONARIO");
    printf("  \n\t3. IMPRIMIR ARVORE");
    printf("  \n\t0. SAIR");
}

int main() {
    FILE *fp;
    fp = fopen("funcionarios.dat", "wb+");
    if(!fp) {
        printf("Nao foi possivel abrir o arquivo.\n");
        system("PAUSE");
        exit(1);
    }

    bTree *tree = cria_arvore(4);

    funcionario n0;
    for (int i = 1; i <= 1050; ++i) {
        n0.codigo = i;
        n0.salario = 3000;
        inserir(tree, n0, fp);
    }

    queue *q0 = cria_fila(1050);
    funcionario key;

    int opt;
    int p;
    int search;
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
                printf("<<<<<<< Pesquisando funcionario >>>>>>>");
                printf("\n\nDigite um funcionario pra pesquisar: ");
                fflush(stdin);
                scanf("%d", &p);
                key.codigo = p;
                search = buscar(tree->root, tree->order, key, fp);
                if(search != -1) {
                    printf("Funcionario %d esta na arvore.\n", search);
                } else {
                    printf("Funcionario \033[31mNAO\033[0m esta na arvore.\n");
                }
                system("PAUSE");
                system("cls");
                break;
            case 2:
                printf("\n\nDigite um funcionario para remover: ");
                fflush(stdin);
                scanf("%d", &p);
                key.codigo = p;
                deletar(tree, key, fp);
                printf("\nFuncionario removido com sucesso.\n");
                system("PAUSE");
                system("cls");
                break;
            case 3:
                imprime_arvore(tree, q0, fp);
                printf("\n");
                system("PAUSE");
                system("cls");
                break;
            default:
                printf("\t<<<<Digite uma opcao valida!!!>>>>\n");
        }
    } while(opt != 0);

    deleta_arquivo(tree, fp);
    system("PAUSE");
    return 0;
}
