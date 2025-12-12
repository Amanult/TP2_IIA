#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

static void trim(char *str) {
    char *end;
    while (*str == ' ' || *str == '\t') str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) end--;
    *(end + 1) = 0;
}

Configuracao *carregar_configuracao(const char *nome_ficheiro) {
    FILE *f = fopen(nome_ficheiro, "r");
    if (!f) {
        printf("Aviso: Ficheiro %s nao encontrado. A criar ficheiro padrao...\n", nome_ficheiro);
        criar_ficheiro_configuracao_padrao(nome_ficheiro);
        f = fopen(nome_ficheiro, "r");
        if (!f) return NULL;
    }

    Configuracao *config = malloc(sizeof(Configuracao));

    // Valores padrão
    config->num_execucoes = 10;
    config->seed_aleatoria = -1;
    config->n_threads = 0; // Inicializar com 0 para indicar que não foi lido do ficheiro

    // Padrões HC
    config->hc.max_iteracoes = 5000;
    config->hc.usar_vizinhanca_2 = 0;
    config->hc.aceitar_iguais = 1;

    // Padrões EA
    config->ea.tamanho_populacao = 50;
    config->ea.num_geracoes = 200;
    config->ea.prob_cruzamento = 0.8;
    config->ea.prob_mutacao = 0.1;
    config->ea.tipo_selecao = 0;
    config->ea.tipo_cruzamento = 0;
    config->ea.tipo_mutacao = 0;
    config->ea.tamanho_torneio = 3;

    // Padrões Híbridos
    config->hib.hibrido1_iter_hc = 500;
    config->hib.hibrido1_pop_ea = 50;
    config->hib.hibrido1_ger_ea = 100;
    config->hib.hibrido2_iter_hc = 1000;
    config->hib.hibrido2_pop_ea = 30;
    config->hib.hibrido2_ger_ea = 50;

    char linha[256];
    while (fgets(linha, sizeof(linha), f)) {
        trim(linha);
        if (linha[0] == '#' || linha[0] == '\0') continue;

        char chave[128], valor[128];
        if (sscanf(linha, "%[^=]=%s", chave, valor) == 2) {
            trim(chave);
            trim(valor);

            if (strcmp(chave, "num_execucoes") == 0) config->num_execucoes = atoi(valor);
            else if (strcmp(chave, "seed_aleatoria") == 0) config->seed_aleatoria = atoi(valor);
            else if (strcmp(chave, "analise_n_threads") == 0) { // Leitura das threads
                config->n_threads = atoi(valor);
                if (config->n_threads < 1) config->n_threads = 1;
            }
            // HC
            else if (strcmp(chave, "hc_max_iteracoes") == 0) config->hc.max_iteracoes = atoi(valor);
            else if (strcmp(chave, "hc_vizinhanca") == 0) config->hc.usar_vizinhanca_2 = atoi(valor);
            else if (strcmp(chave, "hc_aceitar_iguais") == 0) config->hc.aceitar_iguais = atoi(valor);
            // EA
            else if (strcmp(chave, "ea_tamanho_populacao") == 0) config->ea.tamanho_populacao = atoi(valor);
            else if (strcmp(chave, "ea_num_geracoes") == 0) config->ea.num_geracoes = atoi(valor);
            else if (strcmp(chave, "ea_prob_cruzamento") == 0) config->ea.prob_cruzamento = atof(valor);
            else if (strcmp(chave, "ea_prob_mutacao") == 0) config->ea.prob_mutacao = atof(valor);
            else if (strcmp(chave, "ea_tipo_selecao") == 0) config->ea.tipo_selecao = atoi(valor);
            else if (strcmp(chave, "ea_tipo_cruzamento") == 0) config->ea.tipo_cruzamento = atoi(valor);
            else if (strcmp(chave, "ea_tipo_mutacao") == 0) config->ea.tipo_mutacao = atoi(valor);
            else if (strcmp(chave, "ea_tamanho_torneio") == 0) config->ea.tamanho_torneio = atoi(valor);
            // Híbridos
            else if (strcmp(chave, "h1_iter_hc") == 0) config->hib.hibrido1_iter_hc = atoi(valor);
            else if (strcmp(chave, "h1_pop_ea") == 0) config->hib.hibrido1_pop_ea = atoi(valor);
            else if (strcmp(chave, "h1_ger_ea") == 0) config->hib.hibrido1_ger_ea = atoi(valor);
            else if (strcmp(chave, "h2_iter_hc") == 0) config->hib.hibrido2_iter_hc = atoi(valor);
            else if (strcmp(chave, "h2_pop_ea") == 0) config->hib.hibrido2_pop_ea = atoi(valor);
            else if (strcmp(chave, "h2_ger_ea") == 0) config->hib.hibrido2_ger_ea = atoi(valor);
        }
    }

    fclose(f);
    return config;
}

void criar_ficheiro_configuracao_padrao(const char *nome_ficheiro) {
    FILE *f = fopen(nome_ficheiro, "w");
    if (!f) return;

    fprintf(f, "# --- GERAL ---\n");
    fprintf(f, "num_execucoes=10\n");
    fprintf(f, "seed_aleatoria=-1\n");
    fprintf(f, "analise_n_threads=4\n\n"); // Novo parametro no ficheiro

    fprintf(f, "# --- HILL CLIMBING ---\n");
    fprintf(f, "hc_max_iteracoes=5000\n");
    fprintf(f, "hc_vizinhanca=0\n");
    fprintf(f, "hc_aceitar_iguais=1\n\n");

    fprintf(f, "# --- ALGORITMO EVOLUTIVO ---\n");
    fprintf(f, "ea_tamanho_populacao=50\n");
    fprintf(f, "ea_num_geracoes=200\n");
    fprintf(f, "ea_prob_cruzamento=0.8\n");
    fprintf(f, "ea_prob_mutacao=0.1\n");
    fprintf(f, "ea_tipo_selecao=0\n");
    fprintf(f, "ea_tamanho_torneio=3\n");
    fprintf(f, "ea_tipo_cruzamento=0\n");
    fprintf(f, "ea_tipo_mutacao=0\n\n");

    fprintf(f, "# --- HIBRIDOS ---\n");
    fprintf(f, "h1_iter_hc=500\n");
    fprintf(f, "h1_pop_ea=50\n");
    fprintf(f, "h1_ger_ea=100\n");
    fprintf(f, "h2_iter_hc=1000\n");
    fprintf(f, "h2_pop_ea=30\n");
    fprintf(f, "h2_ger_ea=50\n");

    fclose(f);
}

void libertar_configuracao(Configuracao *config) {
    if (config) free(config);
}

void mostrar_configuracao(Configuracao *config) {
    printf("\n=== CONFIGURACAO ===\n");
    printf("Execucoes: %d\n", config->num_execucoes);
    printf("Threads Analise: %d\n", config->n_threads);
    // ... (pode adicionar mais prints se desejar)
    printf("====================\n\n");
}