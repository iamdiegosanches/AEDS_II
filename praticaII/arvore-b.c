#include "arvore-b.h"

void deleta_arquivo(bTree *tree, FILE *fp) {
    free(tree);
    fclose(fp);
    if (remove("funcionarios.dat") == 0)
        printf("\nArquivo deletado com sucesso.\n");
    else
        printf("\nErro: Nao foi possivel deletar o arquivo\n");
}

// O calculate_offset serve para calcular a posição em ‘bytes’ de um determinado nó em um arquivo de índice de árvore
int calcula_posicao(int disk, int order) {

    // Calcula o tamanho do nó em bytes.
    int size_of_btNode = (sizeof(int) * 3) + (sizeof(funcionario) * (order - 1)) + (sizeof(int) * order);

    // Calcula a posição do nó no arquivo.
    // Multiplicamos o tamanho do nó pelo número do disco para obter a posição.
    return size_of_btNode * disk;

}

no_b ler_disco(int disk, int order, FILE *fp) {
    no_b read_node;

    int offset = calcula_posicao(disk, order);
    fseek(fp, offset, SEEK_SET);// define o ponteiro do arquivo para o offset calculado


    // lê as informações do arquivo para o nó
    fread(&read_node.numKeys, sizeof(read_node.numKeys), 1, fp);
    fread(&read_node.isLeaf, sizeof(read_node.isLeaf), 1, fp);
    fread(&read_node.pos_in_disk, sizeof(read_node.pos_in_disk), 1, fp);

    // aloca memória para as chaves e lê os elementos do arquivo
    read_node.keys = malloc(sizeof(funcionario) * (order - 1));
    fread(read_node.keys, sizeof(funcionario), order - 1, fp);

    // aloca memória para os filhos e lê os índices do arquivo
    read_node.filhos = malloc(sizeof(int) * order);
    fread(read_node.filhos, sizeof(int), order, fp);


    return read_node;
}

void escreve_em_disco(no_b node, int order, FILE *fp) {
    int offset = calcula_posicao(node.pos_in_disk, order);
    fseek(fp, offset, SEEK_SET);        // posiciona o ponteiro do arquivo na posição correta

    fwrite(&node.numKeys, sizeof(node.numKeys), 1, fp);   // escreve as informações no arquivo
    fwrite(&node.isLeaf, sizeof(node.isLeaf), 1, fp);
    fwrite(&node.pos_in_disk, sizeof(node.pos_in_disk), 1, fp);
    fwrite(node.keys, sizeof(funcionario), order - 1, fp);
    fwrite(node.filhos, sizeof(int), order, fp);
}

no_b novo_no(int order, int is_leaf) {
    no_b n;

    n.numKeys = 0;                                 // define o número de chaves como 0
    n.isLeaf = is_leaf;

    n.keys = malloc((order - 1) * sizeof(funcionario));  // aloca espaço para o array de chaves
    for (int i = 0; i < order - 1; i++) {                // inicializa as chaves do array
        n.keys[i].codigo = -1;
        n.keys[i].salario = -1;
    }

    n.filhos = malloc((order) * sizeof(int));        // aloca espaço para o array de filhos
    for (int i = 0; i < order; i++) {                  // inicializa os filhos do array
        n.filhos[i] = -1;
    }

    return n;
}

void imprime_funcionario(no_b node, int order) {
    printf("\n[");
    for (int i = 0; i < order - 1; i++) {
        if (node.keys[i].codigo != -1) {
            printf("\n--->Codigo: %d\n", node.keys[i].codigo);
            printf("--->Nome: Ana\n");
            printf("--->Salario: R$ %.2f\n", node.keys[i].salario);
        }
    }
    printf("] ");
}

void particiona(no_b x, int pos, bTree *tree, FILE *fp, int split_root) {
    // Função que faz a divisão de um nó filho em dois quando o número máximo de chaves é atingido.
    no_b y = ler_disco(x.filhos[pos], tree->order, fp); // busca o nó filho na posição pos
    if (split_root == 1) {                                // caso especial quando é feita a divisão da raiz da árvore
        tree->node_count++;                               // incrementa o número total de nós
        y.pos_in_disk = tree->node_count;                 // atribui uma nova posição no arquivo
    }
    no_b z = novo_no(tree->order, y.isLeaf);         // cria um novo nó para ser o filho (pos+1)
    tree->node_count++;                                            // incrementa o número total de nós
    z.pos_in_disk = tree->node_count;                              // atribui uma nova posição no arquivo
    int t = (tree->order / 2);                                     // calcula o grau mínimo de ramificação

    if (tree->order % 2 == 0) {
        t--;
    }
    // define o número de chaves que o novo nó receberá
    z.numKeys = t;

    if (tree->order % 2 != 0) {
        t--;
    }
    for (int j = 0; j <= t && (j + t + 1) <= y.numKeys - 1; j++) { // move os elementos para o novo nó
        z.keys[j] = y.keys[j + t + 1];
        y.keys[j + t + 1].codigo = -1;                     // apaga o elemento do nó anterior
        y.keys[j + t + 1].salario = -1;
    }

    if (y.isLeaf == 0) {                                  // se o nó y não for folha
        for (int j = 0; j <= t; j++) {                    // move os filhos também
            z.filhos[j] = y.filhos[j + t + 1];
            y.filhos[j + t + 1] = -1;                     // apaga o elemento do nó anterior
        }
    }
    // atualiza o número de chaves que o nó tem após a divisão
    y.numKeys = t;

    // caso especial quando é feita a divisão da raiz da árvore
    if (split_root == 1) {
        x.filhos[pos] = y.pos_in_disk;
        x.filhos[pos + 1] = z.pos_in_disk;
    } else {
        int j, i, r;
        for (j = 0; j < tree->order; j++) {
            // abre espaço para o novo filho de x
            if (x.filhos[j] == y.pos_in_disk) {
                for (i = j + 1; i < tree->order; i += 2) {
                    if (i + 1 < tree->order)
                        x.filhos[i + 1] = x.filhos[i];
                }
                r = j;
            }
        }
        x.filhos[r + 1] = z.pos_in_disk;
    }

    for (int j = pos; j < tree->order - 2; j += 2) {
        x.keys[j + 1] = x.keys[j];
    }

    x.keys[pos] = y.keys[t];
    y.keys[t].codigo = -1;
    y.keys[t].salario = -1;
    x.numKeys++;

    // Atualizar as informações no arquivo
    escreve_em_disco(x, tree->order, fp);
    escreve_em_disco(y, tree->order, fp);
    escreve_em_disco(z, tree->order, fp);
}

no_b insere_nao_cheio(no_b node, funcionario key, bTree *tree, FILE *fp) {

    int pos = node.numKeys;

    // Se é uma folha insere o novo elemento
    if (node.isLeaf == 1) {
        int i = pos - 1;
        while (i >= 0 && key.codigo < node.keys[i].codigo) {// Encontrar a posicao
            node.keys[i + 1] = node.keys[i];
            node.keys[i].codigo = -1;
            node.keys[i].salario = -1;
            i--;
        }
        if (i + 1 != pos) {
            node.keys[i + 1] = key;
        } else {
            node.keys[pos] = key;
        }
        node.numKeys++;
        escreve_em_disco(node, tree->order, fp);
        return node;
    // senao, desce para o filho
    } else {
        int n_pd = node.pos_in_disk;
        int i = pos - 1;
        // consegue o filho
        while (key.codigo < node.keys[i].codigo && i >= 0) {
            i--;
            pos--;
        }

        no_b x = ler_disco(node.filhos[pos], tree->order, fp); // pega o no filho
        // se o filho estiver cheio
        if (x.numKeys == tree->order - 1) {
            particiona(node, pos, tree, fp, 0);
            no_b x1 = ler_disco(n_pd, tree->order, fp);
            if (key.codigo > x1.keys[pos].codigo)
                // ajusta aposição
                pos++;
        }
        no_b x1 = ler_disco(n_pd, tree->order, fp);
        no_b x2 = ler_disco(x1.filhos[pos], tree->order, fp);
        insere_nao_cheio(x2, key, tree, fp);
    }
}

funcionario deletar_max(no_b node, int order, FILE *fp) {
    if (node.isLeaf == 1) {
        node.keys[node.numKeys - 1].codigo = -1;
        node.keys[node.numKeys - 1].salario = -1;
        node.numKeys--;
        escreve_em_disco(node, order, fp);
        return node.keys[node.numKeys - 1];
    } else {
        no_b x = ler_disco(node.filhos[node.numKeys], order, fp);
        deletar_max(x, order, fp);
    }
}

funcionario deletar_min(no_b node, int order, FILE *fp) {
    if (node.isLeaf == 1) {
        funcionario x = node.keys[0];
        for (int j = 0; j < node.numKeys; j++)
            node.keys[j] = node.keys[j + 1];
        node.numKeys--;
        escreve_em_disco(node, order, fp);
        return x;
    } else {

        no_b x = ler_disco(node.filhos[0], order, fp);
        deletar_min(x, order, fp);
    }
}

void merge_filho(no_b node, int pos, int order, FILE *fp) {
    int t = (order / 2);

    // Ler em disco os nós filhos na posição pos e pos+1
    no_b y = ler_disco(node.filhos[pos], order, fp);
    no_b z = ler_disco(node.filhos[pos + 1], order, fp);

    // pegar emprestado item da raiz
    y.keys[t - 1] = node.keys[pos];

    // A chave emprestada é removida do nó pai
    node.keys[pos].codigo = -1;
    node.keys[pos].salario = -1;

    // As chaves restantes do nó filho da direita são transferidas para o nó filho da esquerda
    for (int j = 0; j < t - 1; j++) {
        y.keys[t + j] = z.keys[j];
    }
    // Se os nós não são folhas, os filhos do nó filho da direita são transferidos para o nó filho da esquerda
    if (y.isLeaf == 0) {
        for (int j = 0; j < t; j++) {
            y.filhos[t + j] = z.filhos[j];
        }
    }
    y.numKeys = order - 1;
    // As chaves e filhos restantes do nó pai são movidos para a esquerda para preencher o
    // espaço vazio deixado pela chave emprestada
    for (int j = pos + 1; j < node.numKeys; j++) {
        node.keys[j - 1] = node.keys[j];
    }
    for (int j = pos + 2; j < node.numKeys + 1; j++) {
        node.filhos[j - 1] = node.filhos[j];
    }
    // O número de chaves do nó pai é atualizado
    node.numKeys--;
    // O nó filho da esquerda atualizado é escrito de volta no disco
    escreve_em_disco(y, order, fp);
    // O nó pai atualizado é escrito de volta no disco
    escreve_em_disco(node, order, fp);
}

/*
 * Realiza a operação de empréstimo de uma chave e/ou ponteiro de um irmão à esquerda de um nó.
 * Esse empréstimo é feito quando o nó não possui chaves e/ou ponteiros suficientes para continuar a busca.
 * */
void emprestado_irmao_esquerdo(no_b node, int pos, int order, FILE *fp) {
    int t = (order / 2);
    no_b y = ler_disco(node.filhos[pos], order, fp);
    no_b z = ler_disco(node.filhos[pos - 1], order, fp);

    for (int j = t - 1; j > 0; j--) {
        y.keys[j] = y.keys[j - 1];
    }

    y.keys[0] = node.keys[pos - 1];
    node.keys[pos - 1] = z.keys[z.numKeys - 1];

    z.keys[z.numKeys - 1].codigo = -1;
    z.keys[z.numKeys - 1].salario = -1;

    if (y.isLeaf == 0) {
        for (int j = t; j > 1; j--) {
            y.filhos[j + 1] = y.filhos[j];
        }
        y.filhos[1] = z.filhos[z.numKeys + 1];
    }
    y.numKeys = t;
    z.numKeys--;
    escreve_em_disco(z, order, fp);
    escreve_em_disco(y, order, fp);
    escreve_em_disco(node, order, fp);
}

void emprestado_irmao_direito(no_b node, int pos, int order, FILE *fp) {
    int t = (order / 2);
    no_b y = ler_disco(node.filhos[pos], order, fp);
    no_b z = ler_disco(node.filhos[pos + 1], order, fp);

    y.keys[y.numKeys] = node.keys[pos];
    node.keys[pos] = z.keys[0];

    for (int j = 0; j < z.numKeys; j++) {
        if (j + 1 == z.numKeys) {
            z.keys[j].codigo = -1;
            z.keys[j].salario = -1;
        } else
            z.keys[j] = z.keys[j + 1];
    }


    if (y.isLeaf == 0) {
        for (int j = t; j > 1; j--) {
            y.filhos[j + 1] = y.filhos[j];
        }
        y.filhos[1] = z.filhos[z.numKeys + 1];
    }
    y.numKeys = t;
    z.numKeys--;
    escreve_em_disco(z, order, fp);
    escreve_em_disco(y, order, fp);
    escreve_em_disco(node, order, fp);
}

void excluir_codigo(no_b node, funcionario codigo, int ordem, FILE *fp) {
    int g_min = (ordem / 2); // Grau minimo da arvore b
    int emprestado;
    int pos = 0;
    while (pos <= node.numKeys - 1 && codigo.codigo > node.keys[pos].codigo)
        pos++;
    if (pos <= node.numKeys && codigo.codigo == node.keys[pos].codigo) {
        // O nó é uma folha
        if (node.isLeaf == 1) {
            // Remove o funcionário da lista de chaves do nó
            for (int j = pos; j < node.numKeys; j++)
                node.keys[j] = node.keys[j + 1];
            if (pos == node.numKeys - 1) {
                node.keys[pos].codigo = -1;
                node.keys[pos].salario = -1;
            }
            // Atualiza o número de chaves do nó e grava no arquivo
            node.numKeys--;
            escreve_em_disco(node, ordem, fp);
        // O nó não é uma folha
        } else {
            // Lê o nó filho esquerdo e verifica se ele pode emprestar uma chave
            no_b y = ler_disco(node.filhos[pos], ordem, fp);
            if (y.numKeys > g_min - 1) {
                // O filho esquerdo pode emprestar uma chave
                node.keys[pos] = deletar_max(y, ordem, fp);
                escreve_em_disco(node, ordem, fp);

            } else {
                // Lê o nó filho direito e verifica se ele pode emprestar uma chave
                no_b f_direito = ler_disco(node.filhos[pos + 1], ordem, fp);
                if (f_direito.numKeys > g_min - 1) {
                    // O filho direito pode emprestar uma chave
                    node.keys[pos] = deletar_min(f_direito, ordem, fp);
                    escreve_em_disco(node, ordem, fp);
                } else {
                    // Ambos os filhos têm o tamanho mínimo
                    merge_filho(node, pos, ordem, fp);
                    no_b node_child = ler_disco(node.filhos[pos], ordem, fp);
                    excluir_codigo(node_child, codigo, ordem, fp);
                }
            }
        }

    // Caso em que o funcionário a ser excluído não é encontrado na página atual
    } else if (node.isLeaf == 0) {
        // Determina qual nó filho deve ser visitado em seguida
        int m = pos; //default
        no_b y = ler_disco(node.filhos[pos], ordem, fp);
        if (y.numKeys == g_min - 1) {
            // Caso em que o filho esquerdo precisa emprestar ou o direito precisa ser fundido
            emprestado = 0;
            // Se não for o primeiro filho, tenta emprestar do irmão esquerdo
            if (pos > 0) {
                no_b z = ler_disco(node.filhos[pos - 1], ordem, fp);
                // Se o irmão esquerdo tem pelo menos t chaves, empresta uma chave
                if (z.numKeys > g_min - 1) {
                    emprestado_irmao_esquerdo(node, pos, ordem, fp);
                    emprestado = 1;
                } else {// Caso contrário, marca m como o índice do irmão esquerdo
                    m = pos - 1;
                }
            }

            // Se não conseguiu emprestar do irmão esquerdo, tenta emprestar do irmão direito
            if (emprestado == 0 && pos <= node.numKeys && node.filhos[pos + 1] != -1) {
                no_b z = ler_disco(node.filhos[pos + 1], ordem, fp);
                // Se o irmão direito tem pelo menos t chaves, empresta uma chave
                if (z.numKeys > g_min - 1) {
                    emprestado_irmao_direito(node, pos, ordem, fp);
                    emprestado = 1;
                } else { // Caso contrário, marca m como o índice do nó atual
                    m = pos;
                }
            }
            // Se não conseguiu emprestar de nenhum irmão, une o filho com o irmão direito
            if (emprestado == 0) {
                merge_filho(node, m, ordem, fp);
                no_b x = ler_disco(node.filhos[m], ordem, fp);
                y = x;
            }
        }

        if (m != pos) { // Se o filho usado na recursão não for o mesmo que o filho verificado em y
            excluir_codigo(y, codigo, ordem, fp);
        } else { // Caso contrário, lê novamente o filho atual
            no_b new_y = ler_disco(node.filhos[pos], ordem, fp);
            excluir_codigo(new_y, codigo, ordem, fp);
        }
    }
}

bTree *cria_arvore(int order) {
    bTree *tree;  // cria a cabeça da arvore
    if ((tree = malloc(sizeof(bTree))) == NULL)// alloca espaço para a arvore
        return NULL;

    no_b root = novo_no(order, true);  // cria a raiz da arvore
    root.pos_in_disk = 0;  // cria a posicao em disco para a raiz

    tree->order = order;  // ordem da árvore
    tree->root = root;  // a arvore é uma raiz
    tree->node_count = 0;  // A contagem de nos ainda é zero

    return tree;
}

void inserir(bTree *tree, funcionario chave, FILE *fp) {
    if (tree->node_count > 0)
        tree->root = ler_disco(0, tree->order, fp);// Atualiza a raiz da arvore
    no_b raiz = tree->root;   // Variavel para a raiz da arvore

    if (raiz.numKeys == tree->order - 1) {                  // Se a raiz esta cheia
        no_b s = novo_no(tree->order, 0);       // cria outra raiz
        s.filhos[0] = raiz.pos_in_disk;                     // Faz da antiga raiz o seu primeiro filho
        particiona(s, 0, tree, fp, 1);      // divide a raiz
        s = ler_disco(0, tree->order, fp);       // obter nova raiz
        tree->root = s;                                     // tornar a raiz
        insere_nao_cheio(s, chave, tree, fp);     // inserir novo elemento caso a raiz nao esta cheia
    } else {
        tree->root = insere_nao_cheio(raiz, chave, tree, fp);  // insere o elemento onde nao esta cheio
    }
}

int buscar(no_b node, int ordem, funcionario codigo, FILE *fp) {
    int pos = 0;
    while (pos < node.numKeys && codigo.codigo > node.keys[pos].codigo) {  // encontra a posicao percorrendo as chaves
        pos++;
    }
    if (pos <= node.numKeys && codigo.codigo == node.keys[pos].codigo) {   // item foi encontrado
        return node.keys[pos].codigo;
    } else if (node.isLeaf == 1) { // item nao encontrado
        return -1;
    } else {
        no_b x = ler_disco(node.filhos[pos], ordem, fp); // vai mais a fundo na arvore recursivamente
        return buscar(x, ordem, codigo, fp);
    }
}

void deletar(bTree *tree, funcionario codigo, FILE *fp) {
    // Variavel obtendo a raiz da arvore
    no_b root = tree->root;
    excluir_codigo(root, codigo, tree->order, fp); // Apaga o item
    no_b new_root = ler_disco(0, tree->order, fp); // armazena a raiz da arvore
    // se a raiz é vazia e nao for folha substitui pela subarvore
    if (new_root.numKeys == 0 && (new_root.isLeaf == 0)) {
        no_b x = ler_disco(new_root.filhos[0], tree->order, fp); // pegar o primeiro filho da subarvora
        x.pos_in_disk = 0;                                       // sobscreve a informaçao da raiz anterior
        escreve_em_disco(x, tree->order, fp);         // escreve no arquivo a informaçao
        tree->root = x;                                          // transforma o filho na raiz
    } else {
        tree->root = new_root; // mantem a raiz normalmente
    }
}

// Achar o menor do arquivo (nao sera usada)
funcionario btfindMax(no_b node, int order, FILE *fp) {
    if (node.isLeaf == 1) {
        return node.keys[node.numKeys - 1];
    } else {
        no_b x = ler_disco(node.filhos[node.numKeys], order, fp);
        btfindMax(x, order, fp);
    }
}

funcionario btfindMin(no_b node, int order, FILE *fp) {
    if (node.isLeaf == 1) {
        return node.keys[0];
    } else {
        no_b x = ler_disco(node.filhos[0], order, fp);
        btfindMin(x, order, fp);
    }
}

/*
A função btPrintTree é responsável por imprimir a estrutura da árvore B em ordem de nível.
Ela recebe como parâmetros um ponteiro para a estrutura bTree que representa a árvore B,
uma fila queue para armazenar os nós que serão visitados, e um ponteiro para o arquivo fp
onde a árvore está armazenada. A função começa inserindo o nó raiz na fila e continua imprimindo os nós em ordem de nível,
inserindo os nós filhos na fila à medida que os nós são visitados.
A função também utiliza um marcador para indicar o final de cada nível da árvore.
*/
void imprime_arvore(bTree *tree, queue *q, FILE *fp) {
    if (tree->root.numKeys == 0) {
        printf("\nA arvore B esta vazia\n");
    } else {
        no_b end = {.numKeys = -1};                   // cria um marcador para saber quando uma camada da árvore acaba
        insere(q, tree->root);                      // insere a raiz na fila
        int item_count = 1;                               // enquanto a fila não estiver vazia
        int level_count = 0;
        printf("<<<<Nivel %d: >>>>", level_count);                // imprime o número do nível atual
        while (!isEmpty(q)) {
            no_b current = removeData(q);               // remove o primeiro item da fila e retorna esse nó
            if (current.numKeys == -1) {                    // foi encontrado um marcador?
                printf("\n");
                level_count++;                             // incrementa a contagem do nível
                if (!isEmpty(q)) {
                    printf("<<<<Nivel %d: >>>>", level_count);    // imprime o número do próximo nível
                    insere(q, end);
                }
//                insere(q, end);
                if (item_count == 0)                       // para evitar um loop infinito de marcadores
                    break;                                // quando a árvore já foi impressa
            } else {
                item_count--;
                imprime_funcionario(current, tree->order);
                if (current.pos_in_disk == 0)              // caso especial
                    insere(q, end);
                for (int i = 0; i < tree->order; i++) {     // Insere os filhos para o proximo no
                    if (current.filhos[i] != -1) {
                        no_b x = ler_disco(current.filhos[i], tree->order, fp); // obtem o filho
                        insere(q, x);
                        item_count++;
                    }
                }
            }
        }
    }
}
