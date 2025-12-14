/*
 * Programa: Otimizador Turístico (TP2)
 * ------------------------------------------------------------
 * Ponto de entrada da aplicação. Este módulo:
 *  - Lê argumentos de linha de comandos (ou pergunta interativamente em falta);
 *  - Carrega o problema (cidades/locais e distâncias);
 *  - Carrega a configuração a partir de um ficheiro de opções;
 *  - Decide entre executar o modo normal (produz ficheiro Excel com resultados)
 *    ou o modo de análise profunda (varre várias configurações e produz Excel);
 *  - Garante a libertação correta de todos os recursos no fim.
 *  - Depende dos módulos: estruturas, utils, excel, config, analise_profunda.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "estruturas.h"
#include "utils.h"
#include "excel.h"
#include "config.h"
#include "analise_profunda.h"

// Mostra texto de ajuda sobre como executar o programa
void mostrar_ajuda() {
    printf("========================================\n");
    printf("  OTIMIZADOR TURISTICO\n");
    printf("========================================\n\n");
    printf("Uso:\n");
    printf("  ./TP2 <ficheiro_entrada> [opcoes]\n\n");
    printf("Opcoes:\n");
    printf("  --config <ficheiro>     Usar ficheiro de configuracao especifico\n");
    printf("  --analise-profunda      Executar analise profunda de parametros\n");
    printf("  --help                  Mostrar esta ajuda\n\n");
}

// Pergunta ao utilizador, de forma interativa, opções base quando não são
// passadas via linha de comandos (nome do ficheiro de configuração e se
// deseja executar a análise profunda)
void ler_opcoes_interactivas(char *ficheiro_config, int *modo_analise_profunda) {
    char resposta[256];
    printf("\nConfiguracao de Opcoes:\n");

    printf("Ficheiro de configuracao [options.txt]: ");
    if (fgets(resposta, sizeof(resposta), stdin)) {
        size_t len = strlen(resposta);
        if (len > 0 && resposta[len - 1] == '\n') resposta[len - 1] = '\0';
        if (strlen(resposta) > 0) strncpy(ficheiro_config, resposta, 255);
    }

    printf("Executar analise profunda? (s/n) [n]: ");
    if (fgets(resposta, sizeof(resposta), stdin)) {
        if (resposta[0] == 's' || resposta[0] == 'S') *modo_analise_profunda = 1;
    }
}

// Função principal
int main(int argc, char *argv[]) {
    char ficheiro_entrada[256] = "";
    char ficheiro_config[256] = "options.txt";
    int modo_analise_profunda = 0;

    if (argc < 2) {
        mostrar_ajuda();
        printf("Ficheiro de entrada: ");
        if (scanf("%255s", ficheiro_entrada) != 1) return 1;
        // Limpar buffer
        int c; while ((c = getchar()) != '\n' && c != EOF);
        ler_opcoes_interactivas(ficheiro_config, &modo_analise_profunda);
    } else {
        strncpy(ficheiro_entrada, argv[1], 255);
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--analise-profunda") == 0) modo_analise_profunda = 1;
            else if (strcmp(argv[i], "--config") == 0 && i + 1 < argc) strncpy(ficheiro_config, argv[++i], 255);
        }
    }

    // Carregar problema (distâncias e número de locais a selecionar)
    Problema prob;
    prob.nomes = NULL; prob.distancias = NULL;
    if (!ler_problema(ficheiro_entrada, &prob)) {
        printf("Erro ao ler %s\n", ficheiro_entrada);
        return 1;
    }

    // Preparar nome base para ficheiros de saída (apenas nome do ficheiro,
    // sem diretórios nem caracteres inválidos, e sem extensão)
    char base[256];
    strncpy(base, ficheiro_entrada, 255);
    base[255] = '\0';
    // Remover diretórios
    const char *slash = strrchr(base, '/');
    const char *bslash = strrchr(base, '\\');
    const char *name_part = base;
    if (slash && bslash) name_part = (slash > bslash) ? slash + 1 : bslash + 1;
    else if (slash) name_part = slash + 1;
    else if (bslash) name_part = bslash + 1;

    char base_clean[256];
    strncpy(base_clean, name_part, 255);
    base_clean[255] = '\0';
    // Remover extensão
    char *dot = strrchr(base_clean, '.');
    if (dot) *dot = '\0';
    // Substituir caracteres potencialmente inválidos em Windows (ex.: ':') por '_'
    for (char *p = base_clean; *p; ++p) {
        if (*p == ':' || *p == '"' || *p == '\\' || *p == '/' || *p == '|' || *p == '?' || *p == '*' || *p == '<' || *p == '>')
            *p = '_';
    }

    // Carregar configuração (sempre necessária). Caso não exista, a função
    // de utilidade cria um ficheiro com valores padrão.
    printf("A carregar configuracao de: %s\n", ficheiro_config);
    Configuracao *config = carregar_configuracao(ficheiro_config);
    if (!config) {
        fprintf(stderr, "Erro fatal: Configuracao nao carregada.\n");
        libertar_problema(&prob);
        return 1;
    }

    // Inicializar gerador de números aleatórios (seed). Se a seed for -1,
    // usa-se o tempo atual para obter uma seed variável.
    if (config->seed_aleatoria == -1) srand(time(NULL));
    else srand(config->seed_aleatoria);

    if (modo_analise_profunda) {
        if (config->n_threads == 0) {
            char resposta_threads[16];
            int num_threads_input = 0;
            printf("Numero de threads para a analise profunda [4]: ");
            if (fgets(resposta_threads, sizeof(resposta_threads), stdin) && strlen(resposta_threads) > 1) {
                num_threads_input = atoi(resposta_threads);
            }
            if (num_threads_input > 0) {
                config->n_threads = num_threads_input;
            } else {
                config->n_threads = 4; // Default se nada for inserido ou for invalido
            }
        }

        char ficheiro_analise[256];
        snprintf(ficheiro_analise, sizeof(ficheiro_analise), "analise_profunda_%s.xlsx", base_clean);

        // Modo de Análise Profunda: avalia várias combinações de parâmetros
        // e escreve um relatório completo em Excel
        executar_analise_profunda(&prob, config, ficheiro_analise);

    } else {
        // Modo Normal: executa o algoritmo com a configuração dada e
        // exporta métricas resumidas para Excel
        printf("Modo: NORMAL (Execucoes: %d)\n", config->num_execucoes);
        mostrar_configuracao(config);

        char ficheiro_saida[256];
        snprintf(ficheiro_saida, sizeof(ficheiro_saida), "resultados_%s.xlsx", base_clean);
        escrever_resultados_excel(ficheiro_saida, &prob, config);
    }

    libertar_configuracao(config);
    libertar_problema(&prob);
    return 0;
}