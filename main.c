#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include "estruturas.h"
#include "utils.h"
#include "excel.h"
#include "config.h"

int main(int argc, char *argv[]) {
    char ficheiro_entrada[256];
    char ficheiro_config[256] = "options.txt";

    // Ler ficheiro de entrada
    if (argc < 2) {
        printf("Nenhum ficheiro especificado.\n");
        printf("Por favor introduza o nome do ficheiro de entrada: ");
        if (!fgets(ficheiro_entrada, sizeof(ficheiro_entrada), stdin)) {
            fprintf(stderr, "Erro a ler do stdin\n");
            return 1;
        }
        size_t len = strlen(ficheiro_entrada);
        if (len > 0 && ficheiro_entrada[len - 1] == '\n')
            ficheiro_entrada[len - 1] = '\0';

        printf("Por favor introduza o nome do ficheiro de parametros: ");
        if (!fgets(ficheiro_config, sizeof(ficheiro_config), stdin)) {
            fprintf(stderr, "Erro a ler do stdin\n");
            return 1;
        }
        len = strlen(ficheiro_config);
        if (len > 0 && ficheiro_config[len - 1] == '\n')
            ficheiro_config[len - 1] = '\0';

    } else {
        strncpy(ficheiro_entrada, argv[1], sizeof(ficheiro_entrada) - 1);
        ficheiro_entrada[sizeof(ficheiro_entrada) - 1] = '\0';
    }

    // Permitir especificar ficheiro de configuração diferente
    if (argc >= 3) {
        strncpy(ficheiro_config, argv[2], sizeof(ficheiro_config) - 1);
        ficheiro_config[sizeof(ficheiro_config) - 1] = '\0';
    }

    // Carregar configuração
    printf("A carregar configuracao de: %s\n", ficheiro_config);
    Configuracao *config = carregar_configuracao(ficheiro_config);
    if (!config) {
        fprintf(stderr, "Erro ao carregar configuracao\n");
        return 1;
    }

    // Mostrar configuração carregada
    mostrar_configuracao(config);

    // Configurar seed aleatória
    if (config->seed_aleatoria == -1) {
        // NOLINT(clang-diagnostic-unused-variable)
        unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)clock();  // NOLINT
        srand(seed);
        printf("Seed aleatoria: gerada automaticamente (%u)\n\n", seed);
    } else {
        srand((unsigned int)config->seed_aleatoria);
        printf("Seed aleatoria: %d\n\n", config->seed_aleatoria);
    }

    // Ler problema
    Problema prob;
    if (!ler_problema(ficheiro_entrada, &prob)) {
        printf("Erro ao ler ficheiro %s\n", ficheiro_entrada);
        libertar_configuracao(config);
        return 1;
    }

    printf("========================================\n");
    printf("  PROBLEMA DE OTIMIZACAO TURISTICA\n");
    printf("========================================\n");
    printf("Ficheiro: %s\n", ficheiro_entrada);
    printf("Numero de candidatos: %d\n", prob.num_candidatos);
    printf("Numero a selecionar: %d\n", prob.num_selecionados);
    printf("========================================\n");

    // Construir nome do ficheiro de saída
    char base[256];
    const char *p_slash = strrchr(ficheiro_entrada, '/');
    const char *p_bslash = strrchr(ficheiro_entrada, '\\');
    const char *fname = ficheiro_entrada;
    if (p_slash) fname = p_slash + 1;
    if (p_bslash && p_bslash + 1 > fname) fname = p_bslash + 1;
    strncpy(base, fname, sizeof(base) - 1);
    base[sizeof(base) - 1] = '\0';
    char *dot = strrchr(base, '.');
    if (dot) *dot = '\0';
    char ficheiro_saida[256];
    snprintf(ficheiro_saida, sizeof(ficheiro_saida), "resultados_%s.xlsx", base);

    // Executar testes
    escrever_resultados_excel(ficheiro_saida, &prob, config);

    printf("\n========================================\n");
    printf("  EXECUCAO CONCLUIDA COM SUCESSO\n");
    printf("========================================\n");

    // Libertar memória
    libertar_problema(&prob);
    libertar_configuracao(config);

    return 0;
}