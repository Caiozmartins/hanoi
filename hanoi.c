#include <stdio.h> //Bibliotecas utilizadas
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Disco {//Representa um disco da torre
    int tamanho;
    struct Disco *proximo;
} Disco;

typedef struct Torre {
    Disco *topo;
    char id;
} Torre;

typedef struct Historico {
    char nome[50];
    int movimentos;
    int discos;
    char dataHora[20];
    struct Historico *proximo;
} Historico;

Historico *listaHistorico = NULL;

void inicializarTorre(Torre *torre, char id) {
    torre->topo = NULL;
    torre->id = id;
}

void empilhar(Torre *torre, int tamanho) {
    Disco *novo = (Disco *)malloc(sizeof(Disco));
    novo->tamanho = tamanho;
    novo->proximo = torre->topo;
    torre->topo = novo;
}

int desempilhar(Torre *torre) {
    if (torre->topo == NULL) return -1;
    Disco *remover = torre->topo;
    int tamanho = remover->tamanho;
    torre->topo = remover->proximo;
    free(remover);
    return tamanho;
}

void mostrarTorre(Torre *torre) {
    printf("Torre %c: ", torre->id);
    Disco *atual = torre->topo;
    if (!atual) {
        printf("(vazia)");
    }
    while (atual) {
        printf("%d ", atual->tamanho);
        atual = atual->proximo;
    }
    printf("\n");
}

void mostrarTodasTorres(Torre *A, Torre *B, Torre *C) {
    mostrarTorre(A);
    mostrarTorre(B);
    mostrarTorre(C);
    printf("\n");
}

int topoTamanho(Torre *torre) {
    if (torre->topo == NULL) return 1000000; // valor alto para comparar
    return torre->topo->tamanho;
}

int movimentoValido(Torre *origem, Torre *destino) {
    if (origem->topo == NULL) return 0;
    if (topoTamanho(origem) > topoTamanho(destino)) return 0;
    return 1;
}

Torre* pegarTorrePorId(Torre *A, Torre *B, Torre *C, char id) {
    if (id == 'A' || id == 'a') return A;
    if (id == 'B' || id == 'b') return B;
    if (id == 'C' || id == 'c') return C;
    return NULL;
}

void salvarHistoricoArquivo(Historico *lista) {
    FILE *f = fopen("historico.txt", "w");
    if (!f) return;
    Historico *atual = lista;
    while (atual) {
        fprintf(f, "%s;%d;%d;%s\n", atual->nome, atual->movimentos, atual->discos, atual->dataHora);
        atual = atual->proximo;
    }
    fclose(f);
}

void carregarHistoricoArquivo() {
    FILE *f = fopen("historico.txt", "r");
    if (!f) return;
    char linha[200];
    while (fgets(linha, sizeof(linha), f)) {
        Historico *novo = (Historico *)malloc(sizeof(Historico));
        novo->proximo = NULL;
        char *token = strtok(linha, ";");
        if (token) strncpy(novo->nome, token, 50);
        token = strtok(NULL, ";");
        if (token) novo->movimentos = atoi(token);
        token = strtok(NULL, ";");
        if (token) novo->discos = atoi(token);
        token = strtok(NULL, ";\n");
        if (token) strncpy(novo->dataHora, token, 20);
        // inserir no fim da lista
        if (listaHistorico == NULL) {
            listaHistorico = novo;
        } else {
            Historico *tmp = listaHistorico;
            while (tmp->proximo) tmp = tmp->proximo;
            tmp->proximo = novo;
        }
    }
    fclose(f);
}

void adicionarHistorico(char *nome, int movimentos, int discos) {
    Historico *novo = (Historico *)malloc(sizeof(Historico));
    strncpy(novo->nome, nome, 50);
    novo->movimentos = movimentos;
    novo->discos = discos;
    time_t agora = time(NULL);
    struct tm *tm_info = localtime(&agora);
    strftime(novo->dataHora, 20, "%d/%m/%Y %H:%M:%S", tm_info);
    novo->proximo = NULL;

    if (listaHistorico == NULL) {
        listaHistorico = novo;
    } else {
        Historico *tmp = listaHistorico;
        while (tmp->proximo) tmp = tmp->proximo;
        tmp->proximo = novo;
    }
    salvarHistoricoArquivo(listaHistorico);
}

void mostrarHistoricos() {
    if (listaHistorico == NULL) {
        printf("Nenhum histórico disponível.\n");
        return;
    }
    Historico *atual = listaHistorico;
    printf("Histórico de partidas:\n");
    while (atual) {
        printf("Jogador: %s | Movimentos: %d | Discos: %d | Data: %s\n", atual->nome, atual->movimentos, atual->discos, atual->dataHora);
        atual = atual->proximo;
    }
}

void buscarHistorico() {
    char busca[50];
    printf("Digite nome do jogador ou data para buscar: ");//busca historico do jogo
    fgets(busca, sizeof(busca), stdin);
    busca[strcspn(busca, "\n")] = 0; // remove \n

    int achou = 0;
    Historico *atual = listaHistorico;
    while (atual) {
        if (strstr(atual->nome, busca) != NULL || strstr(atual->dataHora, busca) != NULL) {
            printf("Jogador: %s | Movimentos: %d | Discos: %d | Data: %s\n", atual->nome, atual->movimentos, atual->discos, atual->dataHora);
            achou = 1;
        }
        atual = atual->proximo;
    }
    if (!achou) {
        printf("Nenhum registro encontrado para '%s'.\n", busca);
    }
}

void reiniciarJogo(Torre *A, Torre *B, Torre *C, int discos) {
    // Limpar torres
    while (A->topo) desempilhar(A);
    while (B->topo) desempilhar(B);
    while (C->topo) desempilhar(C);
    // Empilhar discos na torre A
    for (int i = discos; i >= 1; i--) {
        empilhar(A, i);
    }
}

void jogarManual(Torre *A, Torre *B, Torre *C, int discos, char *nome) {
    int movimentos = 0;
    reiniciarJogo(A, B, C, discos);
    mostrarTodasTorres(A, B, C);

    while (1) {
        char origem, destino;
        printf("Digite movimento (origem destino) ou 0 0 para sair: ");
        int res = scanf(" %c %c", &origem, &destino);
        while(getchar() != '\n'); // limpar buffer
        if (res != 2) {
            printf("Entrada invalida.\n");
            continue;
        }
        if ((origem == '0' && destino == '0')) {
            printf("Saindo do jogo...\n");
            break;
        }
        Torre *torreOrigem = pegarTorrePorId(A, B, C, origem);
        Torre *torreDestino = pegarTorrePorId(A, B, C, destino);
        if (!torreOrigem || !torreDestino) {
            printf("Torres invalidas. Use A, B ou C.\n");
            continue;
        }
        if (!movimentoValido(torreOrigem, torreDestino)) {
            printf("Movimento invalido: nao e permitido colocar disco maior sobre menor ou torre origem vazia.\n");
            continue;
        } else {
            int disco = desempilhar(torreOrigem);
            empilhar(torreDestino, disco);
        }
        movimentos++;
        mostrarTodasTorres(A, B, C);

        // Verificar vitoria: todos discos em torre C
        int count = 0;
        Disco *d = C->topo;
        while (d) {
            count++;
            d = d->proximo;
        }
        if (count == discos) {
            printf("Parabens %s! Voce venceu em %d movimentos.\n", nome, movimentos);
            adicionarHistorico(nome, movimentos, discos);
            break;
        }
    }
}

int main() {
    Torre A, B, C;
    inicializarTorre(&A, 'A');
    inicializarTorre(&B, 'B');
    inicializarTorre(&C, 'C');
    carregarHistoricoArquivo();

    char nome[50];
    int discos = 0;
    int jogoAtivo = 0;

    while (1) {
        printf("\nMenu:\n");//Menu do game
        printf("1. Jogar Torre de Hanoi (movimentacao manual)\n");
        printf("2. Reiniciar jogo\n");
        printf("3. Ver historico das partidas\n");
        printf("4. Buscar por jogador/data\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");

        int opcao;
        if (scanf("%d", &opcao) != 1) {
            while(getchar() != '\n'); // limpar buffer
            printf("Opcao indisponivel.\n");
            continue;
        }
        while(getchar() != '\n'); // limpar buffer

        switch (opcao) {
            case 1:
                printf("Digite seu nome: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = 0; // remover \n
                if (strlen(nome) == 0) {
                    printf("Nome invalido.\n");
                    break;
                }
                printf("Coloque a quantidade de Discos (3 a 10): ");
                if (scanf("%d", &discos) != 1 || discos < 3 || discos > 10) {
                    while(getchar() != '\n');
                    printf("Numero invalido de discos.\n");
                    break;
                }
                while(getchar() != '\n');
                jogoAtivo = 1;
                jogarManual(&A, &B, &C, discos, nome);
                break;
            case 2:
                if (!jogoAtivo) {
                    printf("Nenhum jogo ativo para reiniciar.\n");
                    break;
                }
                reiniciarJogo(&A, &B, &C, discos);
                printf("Jogo reiniciado.\n");
                mostrarTodasTorres(&A, &B, &C);
                break;
            case 3:
                mostrarHistoricos();
                break;
            case 4:
                buscarHistorico();
                break;
            case 5:
                printf("Saindo do programa...\n");
                // liberar memória dos históricos
                while (listaHistorico) {
                    Historico *tmp = listaHistorico;
                    listaHistorico = listaHistorico->proximo;
                    free(tmp);
                }
                // liberar discos das torres
                while (A.topo) desempilhar(&A);
                while (B.topo) desempilhar(&B);
                while (C.topo) desempilhar(&C);
                return 0;
            default:
                printf("Opcao indisponivel.\n");
        }
    }
    return 0;
}
