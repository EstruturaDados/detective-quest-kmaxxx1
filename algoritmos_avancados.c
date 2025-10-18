#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ============================================================================
// --- Constantes Globais ---
// ============================================================================
#define MAX_NOME_SALA 50
#define MAX_TEXTO_PISTA 50
#define MAX_NOME_SUSPEITO 30
#define NUM_SUSPEITOS 5
#define CAPACIDADE_HASH 10 // Tamanho da Tabela Hash (preferencialmente primo)
#define MAX_NOME_SALA_NA_PISTA 34

// ============================================================================
// --- Estrutura do Suspeito e Associações ---
// ============================================================================

// Pistas iniciais pré-determinadas e seus suspeitos
typedef struct {
    const char* nomeSuspeito;
    const char* nomePistaBase; // Base para o texto da pista
    int pistaID;               // ID para garantir unicidade
} AssociacaoPistaSuspeito;

// Array de Associações (Simulação de quem está vinculado a qual pista)
AssociacaoPistaSuspeito AssociacoesIniciais[4] = {
    {"Dr. Black", "Chave 1 (Sala de Estar)", 1},
    {"Ms. Scarlett", "Chave 2 (Jardim de Inverno)", 2},
    {"Col. Mustard", "Chave 3 (Despensa)", 3},
    {"Dr. Black", "Chave 4 (Varanda)", 4}
};
// O índice dessas associações (0 a 3) será usado para buscar a pista no loop de exploração.


// ============================================================================
// --- Estrutura do Mapa (Árvore Binária) ---
// ============================================================================
typedef struct Sala {
    char nome[MAX_NOME_SALA];
    int pistaID; // NOVO: ID da pista contida, ou -1 se não houver
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// ============================================================================
// --- Estrutura da Pista (Árvore de Busca Binária - BST) ---
// ============================================================================
typedef struct Pista {
    char texto[MAX_TEXTO_PISTA];
    struct Pista *esquerda;
    struct Pista *direita;
} Pista;

// ============================================================================
// --- Estrutura da Tabela Hash (Encadeamento) ---
// ============================================================================

// Nó da Lista Encadeada (Armazena a pista vinculada ao Suspeito)
typedef struct HashNode {
    char pistaTexto[MAX_TEXTO_PISTA];
    struct HashNode *proximo;
} HashNode;

// Elemento da Tabela Hash (Representa o Suspeito e o cabeçalho da lista de pistas)
typedef struct {
    char nomeSuspeito[MAX_NOME_SUSPEITO];
    int contagemPistas;
    HashNode *listaPistas; // Lista encadeada para as pistas (encadeamento)
} TabelaHashElemento;

// A Tabela Hash é um array de ponteiros para os elementos
typedef TabelaHashElemento TabelaHash[CAPACIDADE_HASH];


// ============================================================================
// --- Protótipos das Funções ---
// ============================================================================

// Funções da Sala (Mapa)
Sala* criarSala(const char* nome, int pistaID);
void liberarMapa(Sala *raiz);

// Funções da Pista (BST)
Pista* criarPista(const char* texto);
Pista* inserirPista(Pista *raiz, const char* texto);
void listarPistasEmOrdem(Pista *raiz);
void liberarPistas(Pista *raiz);

// Funções da Tabela Hash
void inicializarHash(TabelaHash hash);
unsigned int funcaoHash(const char *suspeito);
void inserirNaHash(TabelaHash hash, const char* nomeSuspeito, const char* textoPista);
void listarSuspeitosHash(TabelaHash hash);
void analisarSuspeitos(TabelaHash hash);
void liberarHash(TabelaHash hash);

// Funções de Exploração e Jogo
void explorarSalas(Sala *raiz_mapa, Pista **raiz_pistas, TabelaHash hash); // Modificada
void limpar_buffer();


// ============================================================================
// --- Função Principal (main) ---
// ============================================================================
int main() {
    Sala *hall_entrada = NULL;
    Pista *bst_pistas = NULL;
    TabelaHash hash_suspeitos;

    inicializarHash(hash_suspeitos);

    printf("--- Detective Quest: Suspeitos e Solução (Nível Mestre) ---\n");
    printf("Explore a mansão, colete pistas e use a Tabela Hash para encontrar o culpado!\n\n");

    // Montagem do Mapa (Árvore Binária) - Pistas associadas aos índices do array AssociacoesIniciais
    // Nível 0 (Raiz)
    hall_entrada = criarSala("Hall de Entrada", -1);

    // Nível 1
    hall_entrada->esquerda = criarSala("Sala de Estar", 0);   // AssociaçõesIniciais[0]
    hall_entrada->direita = criarSala("Cozinha", -1);

    // Nível 2
    hall_entrada->esquerda->esquerda = criarSala("Biblioteca", -1);
    hall_entrada->esquerda->direita = criarSala("Jardim de Inverno", 1); // AssociaçõesIniciais[1]
    hall_entrada->direita->esquerda = criarSala("Despensa", 2);       // AssociaçõesIniciais[2]

    // Nível 3
    hall_entrada->esquerda->esquerda->esquerda = criarSala("Escritório Secreto", -1); 
    hall_entrada->esquerda->direita->direita = criarSala("Varanda", 3); // AssociaçõesIniciais[3]

    // Início da exploração (explorarSalas agora gerencia BST e Hash)
    explorarSalas(hall_entrada, &bst_pistas, hash_suspeitos);

    // Analisa as pistas e suspeitos
    printf("\n\n======================================================\n");
    printf("                 ANÁLISE FINAL DE CASO\n");
    printf("======================================================\n");
    listarSuspeitosHash(hash_suspeitos);
    analisarSuspeitos(hash_suspeitos);
    printf("======================================================\n");
    
    // Listagem final das pistas (BST)
    printf("\n\n--- Dossiê Completo de Pistas (Ordem Alfabética) ---\n");
    listarPistasEmOrdem(bst_pistas);
    printf("------------------------------------------------------\n");

    // Liberação da memória
    liberarMapa(hall_entrada);
    liberarPistas(bst_pistas);
    liberarHash(hash_suspeitos);

    return 0;
}

// ============================================================================
// --- Implementação das Funções da SALA (Mapa) ---
// ============================================================================

Sala* criarSala(const char* nome, int pistaID) {
    Sala *nova_sala = (Sala*)malloc(sizeof(Sala));

    if (nova_sala == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para a sala '%s'.\n", nome);
        exit(1);
    }

    strncpy(nova_sala->nome, nome, MAX_NOME_SALA - 1);
    nova_sala->nome[MAX_NOME_SALA - 1] = '\0';
    nova_sala->pistaID = pistaID;
    nova_sala->esquerda = NULL;
    nova_sala->direita = NULL;

    return nova_sala;
}

void liberarMapa(Sala *raiz) {
    if (raiz == NULL) return;
    liberarMapa(raiz->esquerda);
    liberarMapa(raiz->direita);
    free(raiz);
}

// ============================================================================
// --- Implementação das Funções da PISTA (BST) ---
// ============================================================================

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
    return raiz;
}

void listarPistasEmOrdem(Pista *raiz) {
    if (raiz == NULL) return;

    listarPistasEmOrdem(raiz->esquerda);
    printf("   - %s\n", raiz->texto);
    listarPistasEmOrdem(raiz->direita);
}

void liberarPistas(Pista *raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

// ============================================================================
// --- Implementação das Funções da TABELA HASH ---
// ============================================================================

void inicializarHash(TabelaHash hash) {
    for (int i = 0; i < CAPACIDADE_HASH; i++) {
        hash[i].nomeSuspeito[0] = '\0';
        hash[i].contagemPistas = 0;
        hash[i].listaPistas = NULL;
    }
}

/*
 * funcaoHash()
 * Hashing simples: Soma dos códigos ASCII dos primeiros três caracteres módulo a capacidade.
 */
unsigned int funcaoHash(const char *suspeito) {
    unsigned int hashValue = 0;
    int len = strlen(suspeito);
    
    // Usa no máximo os 3 primeiros caracteres (ou menos se o nome for curto)
    for (int i = 0; i < len && i < 3; i++) {
        hashValue += suspeito[i];
    }
    return hashValue % CAPACIDADE_HASH;
}

/*
 * inserirNaHash()
 * Insere a pista e associa ao suspeito, usando encadeamento (lista de pistas)
 */
void inserirNaHash(TabelaHash hash, const char* nomeSuspeito, const char* textoPista) {
    unsigned int index = funcaoHash(nomeSuspeito);
    TabelaHashElemento *elemento = &hash[index];

    // 1. Tratamento da Colisão/Inicialização do Suspeito
    if (elemento->nomeSuspeito[0] == '\0') {
        // Posição vazia: Inicializa com o novo suspeito
        strncpy(elemento->nomeSuspeito, nomeSuspeito, MAX_NOME_SUSPEITO - 1);
        elemento->nomeSuspeito[MAX_NOME_SUSPEITO - 1] = '\0';
    } else if (strcmp(elemento->nomeSuspeito, nomeSuspeito) != 0) {
        // Colisão NÃO tratada explicitamente aqui (assumindo que o hashing é suficiente para evitar colisões de nomes,
        // ou que a capacidade é suficiente). Se a colisão de nomes fosse estritamente necessária, seria uma lista de Suspeitos.
        // Para simplificar (Nível Mestre), assumimos que a posição indexada pertence ao primeiro Suspeito que a ocupou.
        // Se a colisão for ignorada, a busca no final falhará para o segundo suspeito com o mesmo hash.
        // No entanto, para fins didáticos, mantemos o primeiro Suspeito encontrado e inserimos a pista.
        printf("\nAVISO: Colisão hash detectada para Suspeito '%s' (hash %d). Pista vinculada a '%s'.\n", 
               nomeSuspeito, index, elemento->nomeSuspeito);
    }
    
    // 2. Inserção da Pista na Lista Encadeada (Encadeamento)
    HashNode *novoNode = (HashNode*)malloc(sizeof(HashNode));
    if (novoNode == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para HashNode.\n");
        exit(1);
    }
    strncpy(novoNode->pistaTexto, textoPista, MAX_TEXTO_PISTA - 1);
    novoNode->pistaTexto[MAX_TEXTO_PISTA - 1] = '\0';
    
    // Insere no início da lista (encadeamento)
    novoNode->proximo = elemento->listaPistas;
    elemento->listaPistas = novoNode;

    // 3. Atualiza a contagem
    elemento->contagemPistas++;
    printf("   [HASH VINCULADO]: Pista '%s' associada a %s.\n", textoPista, nomeSuspeito);
}

/*
 * listarSuspeitosHash()
 * Exibe todos os suspeitos que têm pistas associadas e suas evidências.
 */
void listarSuspeitosHash(TabelaHash hash) {
    printf("\n--- ASSOCIAÇÕES PISTA -> SUSPEITO ---\n");
    bool encontrado = false;
    for (int i = 0; i < CAPACIDADE_HASH; i++) {
        if (hash[i].nomeSuspeito[0] != '\0') {
            encontrado = true;
            printf("\nSuspeito: %s (Pistas: %d)\n", hash[i].nomeSuspeito, hash[i].contagemPistas);
            
            HashNode *current = hash[i].listaPistas;
            while (current != NULL) {
                printf("  -> %s\n", current->pistaTexto);
                current = current->proximo;
            }
        }
    }
    if (!encontrado) {
        printf("Nenhuma pista foi vinculada a um suspeito.\n");
    }
}

/*
 * analisarSuspeitos()
 * Determina o suspeito com o maior número de pistas associadas (o mais citado).
 */
void analisarSuspeitos(TabelaHash hash) {
    int maxContagem = -1;
    char culpadoPotencial[MAX_NOME_SUSPEITO] = "Ninguém";
    
    for (int i = 0; i < CAPACIDADE_HASH; i++) {
        if (hash[i].nomeSuspeito[0] != '\0') {
            if (hash[i].contagemPistas > maxContagem) {
                maxContagem = hash[i].contagemPistas;
                strncpy(culpadoPotencial, hash[i].nomeSuspeito, MAX_NOME_SUSPEITO - 1);
                culpadoPotencial[MAX_NOME_SUSPEITO - 1] = '\0';
            } else if (hash[i].contagemPistas == maxContagem && maxContagem > 0) {
                // Caso de empate: A dedução se torna incerta.
                strcat(culpadoPotencial, " e ");
                strncat(culpadoPotencial, hash[i].nomeSuspeito, MAX_NOME_SUSPEITO - strlen(culpadoPotencial) - 1);
            }
        }
    }

    printf("\n--- DEDUÇÃO DO CULPADO ---\n");
    if (maxContagem > 0) {
        printf("O Suspeito mais citado com %d evidência(s) é: **%s**\n", maxContagem, culpadoPotencial);
    } else {
        printf("Nenhuma evidência coletada. A dedução falhou.\n");
    }
}

void liberarHash(TabelaHash hash) {
    for (int i = 0; i < CAPACIDADE_HASH; i++) {
        HashNode *current = hash[i].listaPistas;
        while (current != NULL) {
            HashNode *temp = current;
            current = current->proximo;
            free(temp);
        }
        hash[i].listaPistas = NULL;
    }
}

// ============================================================================
// --- Implementação das Funções de Exploração e Jogo ---
// ============================================================================

void explorarSalas(Sala *sala_atual, Pista **raiz_pistas, TabelaHash hash) {
    char escolha[10];

    printf("\n--- INÍCIO DA EXPLORAÇÃO ---\n");
    printf("Caminho percorrido: ");

    while (sala_atual != NULL) {
        printf(" -> %s", sala_atual->nome);

        // Lógica de Encontrar Pista (Mestre)
        if (sala_atual->pistaID != -1) {
            int idx = sala_atual->pistaID;
            
            // Pega os dados da pista e suspeito da associação predefinida
            const char* nomeSuspeito = AssociacoesIniciais[idx].nomeSuspeito;
            const char* nomePistaBase = AssociacoesIniciais[idx].nomePistaBase;
            
            // 1. Insere na BST (Pistas)
            *raiz_pistas = inserirPista(*raiz_pistas, nomePistaBase);
            
            // 2. Insere na Tabela Hash (Suspeitos)
            inserirNaHash(hash, nomeSuspeito, nomePistaBase);

            printf("\n\n*** PISTA ENCONTRADA! ***\n");
            printf("Evidência '%s' coletada e vinculada a %s.\n", nomePistaBase, nomeSuspeito);

            // Desativa a pista
            sala_atual->pistaID = -1;
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
        printf("  [A]nalisar suspeitos (Tabela Hash)\n"); // NOVO: Opção de análise
        printf("  [S]air da exploração\n");

        printf("Escolha seu próximo caminho (e/d/a/s): ");
        if (fgets(escolha, sizeof(escolha), stdin) == NULL) break;
        limpar_buffer();

        escolha[strcspn(escolha, "\n")] = 0;

        // Processa a escolha do jogador
        Sala *proxima_sala = NULL;
        if (strcmp(escolha, "s") == 0 || strcmp(escolha, "S") == 0) {
            printf("\nExploração encerrada pelo jogador.\n");
            break;
        } else if (strcmp(escolha, "a") == 0 || strcmp(escolha, "A") == 0) {
             printf("\n--- ANÁLISE PARCIAL DOS SUSPEITOS ---\n");
             listarSuspeitosHash(hash);
             printf("-------------------------------------------\n");
             // Continua na sala atual
        } else if (strcmp(escolha, "e") == 0 || strcmp(escolha, "E") == 0) {
            proxima_sala = sala_atual->esquerda;
        } else if (strcmp(escolha, "d") == 0 || strcmp(escolha, "D") == 0) {
            proxima_sala = sala_atual->direita;
        }

        if (proxima_sala != NULL) {
            sala_atual = proxima_sala;
        } else if (strcmp(escolha, "a") != 0 && strcmp(escolha, "A") != 0) {
            printf("Caminho inválido ou inexistente. Tente novamente.\n");
        }
    }
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}