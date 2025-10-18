#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ============================================================================
// --- Constantes Globais ---
// ============================================================================
#define MAX_NOME_SALA 50
#define MAX_TEXTO_PISTA 50 // Capacidade máxima para o texto da pista

// Determina o máximo de caracteres que o nome da sala pode ocupar no texto da pista.
// 50 (MAX_TEXTO_PISTA) - 16 (Tamanho de "Chave XX ()" e NULL) = 34
#define MAX_NOME_SALA_NA_PISTA 34

// ============================================================================
// --- Estrutura do Mapa (Árvore Binária) ---
// ============================================================================

// Struct que representa um cômodo (nó) na árvore binária
typedef struct Sala {
    char nome[MAX_NOME_SALA];
    bool temPista; 
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// ============================================================================
// --- Estrutura da Pista (Árvore de Busca Binária - BST) ---
// ============================================================================

// Struct que representa uma pista na BST
typedef struct Pista {
    char texto[MAX_TEXTO_PISTA];
    struct Pista *esquerda;
    struct Pista *direita;
} Pista;


// ============================================================================
// --- Protótipos das Funções ---
// ============================================================================

// Funções da Sala (Mapa)
Sala* criarSala(const char* nome, bool temPista); 
void explorarSalas(Sala *raiz_mapa, Pista **raiz_pistas); 
void liberarMapa(Sala *raiz);

// Funções da Pista (BST)
Pista* criarPista(const char* texto);
Pista* inserirPista(Pista *raiz, const char* texto);
void listarPistasEmOrdem(Pista *raiz);
void liberarPistas(Pista *raiz);

// Funções Utilitárias
void limpar_buffer();

// ============================================================================
// --- Função Principal (main) ---
// ============================================================================

/*
 * main()
 *
 * Monta o mapa inicial, inicializa a BST de pistas e inicia a exploração.
 */
int main() {
    Sala *hall_entrada = NULL;
    Pista *bst_pistas = NULL; 

    printf("--- Detective Quest: Organização de Pistas (Nível Aventureiro) ---\n");
    printf("Explore a mansão para encontrar pistas, que serão armazenadas em ordem alfabética.\n\n");

    // Montagem do Mapa (Árvore Binária) - Pistas adicionadas a salas específicas
    // Nível 0 (Raiz)
    hall_entrada = criarSala("Hall de Entrada", false);

    // Nível 1
    hall_entrada->esquerda = criarSala("Sala de Estar", true);   // PISTA 1
    hall_entrada->direita = criarSala("Cozinha", false);

    // Nível 2
    hall_entrada->esquerda->esquerda = criarSala("Biblioteca", false);
    hall_entrada->esquerda->direita = criarSala("Jardim de Inverno", true); // PISTA 2
    hall_entrada->direita->esquerda = criarSala("Despensa", true);       // PISTA 3
    // Cozinha->direita = NULL

    // Nível 3
    hall_entrada->esquerda->esquerda->esquerda = criarSala("Escritório Secreto", false); // Folha
    // Biblioteca->direita = NULL

    hall_entrada->esquerda->direita->direita = criarSala("Varanda", true); // PISTA 4 (Folha)
    // Jardim de Inverno->esquerda = NULL

    // Início da exploração
    explorarSalas(hall_entrada, &bst_pistas);

    // Listagem final das pistas em ordem alfabética (emOrdem)
    printf("\n\n======================================================\n");
    printf("             Dossiê de Pistas Encontradas\n");
    printf("======================================================\n");
    listarPistasEmOrdem(bst_pistas);
    printf("======================================================\n");

    // Liberação da memória
    liberarMapa(hall_entrada);
    liberarPistas(bst_pistas);

    return 0;
}

// ============================================================================
// --- Implementação das Funções da SALA (Mapa) ---
// ============================================================================

/*
 * criarSala()
 *
 * Aloca dinamicamente uma nova Sala, inicializa seu nome, ponteiros e a flag de pista.
 */
Sala* criarSala(const char* nome, bool temPista) {
    Sala *nova_sala = (Sala*)malloc(sizeof(Sala));

    if (nova_sala == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para a sala '%s'.\n", nome);
        exit(1);
    }

    strncpy(nova_sala->nome, nome, MAX_NOME_SALA - 1);
    nova_sala->nome[MAX_NOME_SALA - 1] = '\0';
    nova_sala->temPista = temPista;
    nova_sala->esquerda = NULL;
    nova_sala->direita = NULL;

    return nova_sala;
}

/*
 * liberarMapa()
 *
 * Libera a memória alocada dinamicamente para cada Sala (nó) da árvore.
 */
void liberarMapa(Sala *raiz) {
    if (raiz == NULL) return;
    liberarMapa(raiz->esquerda);
    liberarMapa(raiz->direita);
    free(raiz);
}

// ============================================================================
// --- Implementação das Funções da PISTA (BST) ---
// ============================================================================

/*
 * criarPista()
 *
 * Cria e aloca dinamicamente um novo nó de Pista.
 */
Pista* criarPista(const char* texto) {
    Pista *nova_pista = (Pista*)malloc(sizeof(Pista));

    if (nova_pista == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para a pista.\n");
        exit(1);
    }

    strncpy(nova_pista->texto, texto, MAX_TEXTO_PISTA - 1);
    nova_pista->texto[MAX_TEXTO_PISTA - 1] = '\0';
    nova_pista->esquerda = NULL;
    nova_pista->direita = NULL;
    return nova_pista;
}

/*
 * inserirPista()
 *
 * Insere uma nova pista na BST, mantendo a ordem alfabética.
 */
Pista* inserirPista(Pista *raiz, const char* texto) {
    if (raiz == NULL) {
        return criarPista(texto);
    }

    int comparacao = strcmp(texto, raiz->texto);

    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, texto);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, texto);
    }
    // Se for igual, ignora a inserção (pista duplicada)

    return raiz;
}

/*
 * listarPistasEmOrdem()
 *
 * Percorre a BST em ordem (InOrder) para listar as pistas em ordem alfabética.
 */
void listarPistasEmOrdem(Pista *raiz) {
    if (raiz == NULL) return;

    listarPistasEmOrdem(raiz->esquerda);
    printf("   - %s\n", raiz->texto);
    listarPistasEmOrdem(raiz->direita);
}

/*
 * liberarPistas()
 *
 * Libera a memória alocada para a BST de pistas.
 */
void liberarPistas(Pista *raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}


// ============================================================================
// --- Implementação das Funções de Exploração e Jogo ---
// ============================================================================

/*
 * explorarSalas()
 *
 * Permite a navegação interativa e adiciona pistas à BST ao encontrá-las.
 */
void explorarSalas(Sala *sala_atual, Pista **raiz_pistas) {
    char escolha[10];
    int pista_counter = 1;

    printf("\n--- INÍCIO DA EXPLORAÇÃO ---\n");
    printf("Caminho percorrido: ");

    while (sala_atual != NULL) {
        printf(" -> %s", sala_atual->nome);

        // Lógica de Encontrar Pista (Aventureiro)
        if (sala_atual->temPista) {
            char texto_pista[MAX_TEXTO_PISTA];

            // CORREÇÃO DO ERRO: Usa *.*s para limitar o comprimento da string 'nome'
            // Isso garante que o buffer de MAX_TEXTO_PISTA não seja estourado.
            snprintf(texto_pista, MAX_TEXTO_PISTA, "Chave %d (%.*s)", 
                     pista_counter++, 
                     MAX_NOME_SALA_NA_PISTA, // Largura máxima do nome no formato
                     sala_atual->nome);

            // Insere a pista na BST (inserir())
            *raiz_pistas = inserirPista(*raiz_pistas, texto_pista);

            printf("\n\n*** PISTA ENCONTRADA! ***\n");
            printf("A pista '%s' foi adicionada ao seu dossiê.\n", texto_pista);

            // Desativa a pista para que não seja encontrada novamente
            sala_atual->temPista = false;
        }

        // Verifica se é o fim do caminho
        if (sala_atual->esquerda == NULL && sala_atual->direita == NULL) {
            printf("\n\n*** FIM DO CAMINHO! ***\n");
            printf("Você chegou a um beco sem saída (%s) e encerrou esta exploração.\n", sala_atual->nome);
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
        printf("  [L]istar pistas (emOrdem)\n");
        printf("  [S]air da exploração\n");

        printf("Escolha seu próximo caminho (e/d/l/s): ");
        if (fgets(escolha, sizeof(escolha), stdin) == NULL) break;
        limpar_buffer();

        escolha[strcspn(escolha, "\n")] = 0;

        // Processa a escolha do jogador
        Sala *proxima_sala = NULL;
        if (strcmp(escolha, "s") == 0 || strcmp(escolha, "S") == 0) {
            printf("\nExploração encerrada pelo jogador.\n");
            break;
        } else if (strcmp(escolha, "l") == 0 || strcmp(escolha, "L") == 0) {
             printf("\n--- PISTAS ATUAIS (Ordem Alfabética) ---\n");
             listarPistasEmOrdem(*raiz_pistas);
             printf("-------------------------------------------\n");
        } else if (strcmp(escolha, "e") == 0 || strcmp(escolha, "E") == 0) {
            proxima_sala = sala_atual->esquerda;
        } else if (strcmp(escolha, "d") == 0 || strcmp(escolha, "D") == 0) {
            proxima_sala = sala_atual->direita;
        }

        if (proxima_sala != NULL) {
            sala_atual = proxima_sala;
        } else if (strcmp(escolha, "l") != 0 && strcmp(escolha, "L") != 0) {
            printf("Caminho inválido ou inexistente. Tente novamente.\n");
        }
    }
}

/*
 * limpar_buffer()
 *
 * Utilitário para limpar o buffer de entrada.
 */
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}