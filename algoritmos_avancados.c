#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ============================================================================
// --- Constantes Globais ---
// ============================================================================
#define MAX_NOME_SALA 50

// ============================================================================
// --- Estrutura de Dados ---
// ============================================================================

// Struct que representa um cômodo (nó) na árvore binária
typedef struct Sala {
    char nome[MAX_NOME_SALA];
    struct Sala *esquerda; // Caminho para a sub-árvore da esquerda
    struct Sala *direita;  // Caminho para a sub-árvore da direita
} Sala;

// ============================================================================
// --- Protótipos das Funções ---
// ============================================================================

// Função de Documentação: criarSala()
Sala* criarSala(const char* nome);

// Função de Documentação: explorarSalas()
void explorarSalas(Sala *raiz);

// Função auxiliar para limpar a memória
void liberarMapa(Sala *raiz);

// Função auxiliar para limpar o buffer de entrada
void limpar_buffer();

// ============================================================================
// --- Função Principal (main) ---
// ============================================================================

/*
 * main()
 *
 * Monta o mapa inicial da mansão como uma árvore binária de forma manual no
 * código-fonte (criação automática) e dá início à exploração interativa.
 */
int main() {
    Sala *hall_entrada = NULL;

    printf("--- Detective Quest: Mapa da Mansão (Nível Novato) ---\n");
    printf("Bem-vindo(a) ao Hall de Entrada! Seu objetivo é explorar a mansão.\n\n");

    // Montagem manual da Árvore Binária (Mapa)
    // Nível 0 (Raiz)
    hall_entrada = criarSala("Hall de Entrada");

    // Nível 1
    hall_entrada->esquerda = criarSala("Sala de Estar");
    hall_entrada->direita = criarSala("Cozinha");

    // Nível 2
    hall_entrada->esquerda->esquerda = criarSala("Biblioteca");
    hall_entrada->esquerda->direita = criarSala("Jardim de Inverno");
    hall_entrada->direita->esquerda = criarSala("Despensa");
    // Cozinha->direita não tem caminhos (NULL)

    // Nível 3 (Nós-folha ou nós com apenas um caminho)
    hall_entrada->esquerda->esquerda->esquerda = criarSala("Escritório Secreto"); // Folha
    // Biblioteca->direita não tem caminhos (NULL)

    hall_entrada->esquerda->direita->direita = criarSala("Varanda"); // Folha
    // Jardim de Inverno->esquerda não tem caminhos (NULL)

    // Início da exploração
    explorarSalas(hall_entrada);

    // Liberação da memória alocada
    liberarMapa(hall_entrada);

    return 0;
}

// ============================================================================
// --- Implementação das Funções ---
// ============================================================================

/*
 * criarSala()
 *
 * Aloca dinamicamente uma nova Sala (nó), inicializa seu nome e define
 * os ponteiros esquerda e direita como NULL.
 * Retorna o ponteiro para a nova Sala criada.
 */
Sala* criarSala(const char* nome) {
    // 1. Aloca memória para a nova sala
    Sala *nova_sala = (Sala*)malloc(sizeof(Sala));

    // 2. Verifica se a alocação foi bem-sucedida
    if (nova_sala == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para a sala '%s'.\n", nome);
        exit(1);
    }

    // 3. Inicializa os campos
    strncpy(nova_sala->nome, nome, MAX_NOME_SALA - 1);
    nova_sala->nome[MAX_NOME_SALA - 1] = '\0'; // Garante terminação nula
    nova_sala->esquerda = NULL;
    nova_sala->direita = NULL;

    return nova_sala;
}

/*
 * explorarSalas()
 *
 * Permite a navegação interativa do jogador pela árvore, exibindo o nome
 * da sala atual e solicitando a escolha de um caminho (e/d/s). A exploração
 * continua até o jogador sair (s) ou chegar a um nó-folha.
 */
void explorarSalas(Sala *sala_atual) {
    char escolha[10];

    printf("\n--- INÍCIO DA EXPLORAÇÃO ---\n");
    printf("Você está no caminho: ");

    // Loop de exploração
    while (sala_atual != NULL) {
        printf(" -> %s", sala_atual->nome);

        // Verifica se a sala atual é um nó-folha (fim do caminho)
        if (sala_atual->esquerda == NULL && sala_atual->direita == NULL) {
            printf("\n\n*** FIM DO CAMINHO! ***\n");
            printf("Você chegou a um beco sem saída (%s) e encerrou a exploração.\n", sala_atual->nome);
            break;
        }

        printf("\n\nVocê está em: %s\n", sala_atual->nome);
        printf("Caminhos disponíveis:\n");

        if (sala_atual->esquerda != NULL) {
            printf("  [E]squerda: %s\n", sala_atual->esquerda->nome);
        }
        if (sala_atual->direita != NULL) {
            printf("  [D]ireita: %s\n", sala_atual->direita->nome);
        }
        printf("  [S]air da exploração\n");

        printf("Escolha seu próximo caminho (e/d/s): ");
        if (fgets(escolha, sizeof(escolha), stdin) == NULL) {
            break; // Erro de leitura
        }
        limpar_buffer();
        
        // Remove a quebra de linha do fgets
        escolha[strcspn(escolha, "\n")] = 0;

        // Processa a escolha do jogador
        if (strcmp(escolha, "s") == 0 || strcmp(escolha, "S") == 0) {
            printf("\nExploração encerrada pelo jogador.\n");
            break;
        } else if (strcmp(escolha, "e") == 0 || strcmp(escolha, "E") == 0) {
            if (sala_atual->esquerda != NULL) {
                sala_atual = sala_atual->esquerda;
            } else {
                printf("Caminho 'Esquerda' não existe a partir daqui. Tente outro.\n");
            }
        } else if (strcmp(escolha, "d") == 0 || strcmp(escolha, "D") == 0) {
            if (sala_atual->direita != NULL) {
                sala_atual = sala_atual->direita;
            } else {
                printf("Caminho 'Direita' não existe a partir daqui. Tente outro.\n");
            }
        } else {
            printf("Escolha inválida. Use 'e', 'd' ou 's'.\n");
        }
    }
}

/*
 * liberarMapa()
 *
 * Libera a memória alocada dinamicamente para cada Sala (nó) da árvore.
 * Usa um percurso pós-ordem (recursivo) para liberar os filhos antes do pai.
 */
void liberarMapa(Sala *raiz) {
    if (raiz == NULL) {
        return;
    }

    // 1. Libera o filho da esquerda
    liberarMapa(raiz->esquerda);

    // 2. Libera o filho da direita
    liberarMapa(raiz->direita);

    // 3. Libera a Sala atual (o pai)
    // printf("Liberando sala: %s\n", raiz->nome); // Opcional: para rastrear a liberação
    free(raiz);
}

/*
 * limpar_buffer()
 *
 * Utilitário para limpar o buffer de entrada (necessário após fgets e antes de outro fgets).
 */
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}