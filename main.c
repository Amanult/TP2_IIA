#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "estruturas.h"
#include "utils.h"
#include "excel.h"
#include "config.h"
#include "analise_profunda.h"

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
    printf("Exemplos:\n");
    printf("  ./TP2 tourism_5.txt\n");
    printf("  ./TP2 tourism_5.txt --config minha_config.txt\n");
    printf("  ./TP2 tourism_5.txt --analise-profunda\n\n");
    printf("Modo Normal:\n");
    printf("  - Le parametros de options.txt (ou ficheiro especificado)\n");
    printf("  - Executa cada algoritmo 10 vezes\n");
    printf("  - Gera resultados_<ficheiro>.xlsx\n\n");
    printf("Modo Analise Profunda:\n");
    printf("  - Testa TODAS as combinacoes de parametros\n");
    printf("  - Encontra a melhor configuracao para cada algoritmo\n");
    printf("  - Gera analise_profunda_<ficheiro>.xlsx\n");
    printf("  - AVISO: Pode demorar varios minutos!\n\n");
}

void ler_opcoes_interactivas(char *ficheiro_config, int *modo_analise_profunda) {
    char resposta[256];

    printf("\n========================================\n");
    printf("  CONFIGURACAO DE OPCOES\n");
    printf("========================================\n\n");

    // Opção de ficheiro de configuração
    printf("Ficheiro de configuracao [options.txt]: ");
    if (fgets(resposta, sizeof(resposta), stdin)) {
        size_t len = strlen(resposta);
        if (len > 0 && resposta[len - 1] == '\n')
            resposta[len - 1] = '\0';

        if (strlen(resposta) > 0) {
            strncpy(ficheiro_config, resposta, 255);
            ficheiro_config[255] = '\0';
        }
    }

    // Opção de análise profunda
    printf("Executar analise profunda? (s/n) [n]: ");
    if (fgets(resposta, sizeof(resposta), stdin)) {
        size_t len = strlen(resposta);
        if (len > 0 && resposta[len - 1] == '\n')
            resposta[len - 1] = '\0';

        if (strlen(resposta) > 0 && (resposta[0] == 's' || resposta[0] == 'S')) {
            *modo_analise_profunda = 1;
        }
    }

    printf("\n");
}

int main(int argc, char *argv[]) {
    char ficheiro_entrada[256] = "";
    char ficheiro_config[256] = "options.txt";
    int modo_analise_profunda = 0;

    // Processar argumentos
    if (argc < 2) {
        mostrar_ajuda();
        printf("Por favor introduza o nome do ficheiro de entrada: ");
        if (!fgets(ficheiro_entrada, sizeof(ficheiro_entrada), stdin)) {
            fprintf(stderr, "Erro a ler do stdin\n");
            return 1;
        }
        size_t len = strlen(ficheiro_entrada);
        if (len > 0 && ficheiro_entrada[len - 1] == '\n')
            ficheiro_entrada[len - 1] = '\0';

        // Ler opções de forma interactiva
        ler_opcoes_interactivas(ficheiro_config, &modo_analise_profunda);
    } else {
        strncpy(ficheiro_entrada, argv[1], sizeof(ficheiro_entrada) - 1);
        ficheiro_entrada[sizeof(ficheiro_entrada) - 1] = '\0';

        // Processar opções adicionais
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                mostrar_ajuda();
                return 0;
            } else if (strcmp(argv[i], "--analise-profunda") == 0) {
                modo_analise_profunda = 1;
            } else if (strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
                strncpy(ficheiro_config, argv[i + 1], sizeof(ficheiro_config) - 1);
                ficheiro_config[sizeof(ficheiro_config) - 1] = '\0';
                i++;  // Pular próximo argumento
            }
        }
    }

    // Configurar seed aleatória (padrão para análise profunda)
    srand(time(NULL));

    // Ler problema
    Problema prob;
    prob.nomes = NULL;
    prob.distancias = NULL;
    if (!ler_problema(ficheiro_entrada, &prob)) {
        printf("Erro ao ler ficheiro %s\n", ficheiro_entrada);
        return 1;
    }

    printf("========================================\n");
    printf("  PROBLEMA DE OTIMIZACAO TURISTICA\n");
    printf("========================================\n");
    printf("Ficheiro: %s\n", ficheiro_entrada);
    printf("Numero de candidatos: %d\n", prob.num_candidatos);
    printf("Numero a selecionar: %d\n", prob.num_selecionados);

    // Construir nome base do ficheiro
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

    if (modo_analise_profunda) {
        // MODO ANÁLISE PROFUNDA
        printf("Modo: ANALISE PROFUNDA\n");
        printf("========================================\n");

        char ficheiro_analise[256];
        snprintf(ficheiro_analise, sizeof(ficheiro_analise),
                 "analise_profunda_%s.xlsx", base);

        executar_analise_profunda(&prob, ficheiro_analise);

    } else {
        // MODO NORMAL
        printf("Modo: NORMAL\n");
        printf("========================================\n");

        // Carregar configuração
        printf("A carregar configuracao de: %s\n", ficheiro_config);
        Configuracao *config = carregar_configuracao(ficheiro_config);
        if (!config) {
            fprintf(stderr, "Erro ao carregar configuracao\n");
            libertar_problema(&prob);
            return 1;
        }

        // Mostrar configuração carregada
        mostrar_configuracao(config);

        // Configurar seed aleatória
        if (config->seed_aleatoria == -1) {
            srand(time(NULL));
            printf("Seed aleatoria: tempo atual (%ld)\n\n", (long)time(NULL));
        } else {
            srand(config->seed_aleatoria);
            printf("Seed aleatoria: %d\n\n", config->seed_aleatoria);
        }

        char ficheiro_saida[256];
        snprintf(ficheiro_saida, sizeof(ficheiro_saida),
                 "resultados_%s.xlsx", base);

        // Executar testes
        escrever_resultados_excel(ficheiro_saida, &prob, config);

        // Libertar configuração
        libertar_configuracao(config);
    }

    printf("\n========================================\n");
    printf("  EXECUCAO CONCLUIDA COM SUCESSO\n");
    printf("========================================\n");

    // Libertar memória
    libertar_problema(&prob);

    return 0;
}