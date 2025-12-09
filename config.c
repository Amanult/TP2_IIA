#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

// Remove espaços em branco no início e fim
static void trim(char *str) {
    char *end;
    while (*str == ' ' || *str == '\t') str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) end--;
    *(end + 1) = 0;
}

// Carrega configuração do ficheiro
Configuracao *carregar_configuracao(const char *nome_ficheiro) {
    FILE *f = fopen(nome_ficheiro, "r");
    if (!f) {
        printf("Aviso: Ficheiro %s nao encontrado. A criar ficheiro padrao...\n", nome_ficheiro);
        criar_ficheiro_configuracao_padrao(nome_ficheiro);
        f = fopen(nome_ficheiro, "r");
        if (!f) {
            fprintf(stderr, "Erro: Nao foi possivel criar ficheiro de configuracao\n");
            return NULL;
        }
    }
    
    Configuracao *config = malloc(sizeof(Configuracao));
    
    // Valores padrão
    config->num_execucoes = 10;
    config->seed_aleatoria = -1;
    
    config->hc.max_iteracoes = 5000;
    config->hc.usar_vizinhanca_2 = 0;
    config->hc.aceitar_iguais = 1;
    
    config->ea.tamanho_populacao = 50;
    config->ea.num_geracoes = 200;
    config->ea.prob_cruzamento = 0.8;
    config->ea.prob_mutacao = 0.1;
    config->ea.tipo_selecao = 0;
    config->ea.tipo_cruzamento = 0;
    config->ea.tipo_mutacao = 0;
    config->ea.tamanho_torneio = 3;
    
    config->hib.hibrido1_iter_hc = 500;
    config->hib.hibrido1_pop_ea = 50;
    config->hib.hibrido1_ger_ea = 100;
    config->hib.hibrido2_iter_hc = 1000;
    config->hib.hibrido2_pop_ea = 30;
    config->hib.hibrido2_ger_ea = 50;
    
    char linha[256];
    while (fgets(linha, sizeof(linha), f)) {
        trim(linha);
        
        // Ignorar comentários e linhas vazias
        if (linha[0] == '#' || linha[0] == '\0') continue;
        
        char chave[128], valor[128];
        if (sscanf(linha, "%[^=]=%s", chave, valor) == 2) {
            trim(chave);
            trim(valor);
            
            // Parâmetros gerais
            if (strcmp(chave, "num_execucoes") == 0) {
                config->num_execucoes = atoi(valor);
            } else if (strcmp(chave, "seed_aleatoria") == 0) {
                config->seed_aleatoria = atoi(valor);
            }
            // Hill Climbing
            else if (strcmp(chave, "hc_max_iteracoes") == 0) {
                config->hc.max_iteracoes = atoi(valor);
            } else if (strcmp(chave, "hc_vizinhanca") == 0) {
                config->hc.usar_vizinhanca_2 = atoi(valor);
            } else if (strcmp(chave, "hc_aceitar_iguais") == 0) {
                config->hc.aceitar_iguais = atoi(valor);
            }
            // Evolutivo
            else if (strcmp(chave, "ea_tamanho_populacao") == 0) {
                config->ea.tamanho_populacao = atoi(valor);
            } else if (strcmp(chave, "ea_num_geracoes") == 0) {
                config->ea.num_geracoes = atoi(valor);
            } else if (strcmp(chave, "ea_prob_cruzamento") == 0) {
                config->ea.prob_cruzamento = atof(valor);
            } else if (strcmp(chave, "ea_prob_mutacao") == 0) {
                config->ea.prob_mutacao = atof(valor);
            } else if (strcmp(chave, "ea_tipo_selecao") == 0) {
                config->ea.tipo_selecao = atoi(valor);
            } else if (strcmp(chave, "ea_tipo_cruzamento") == 0) {
                config->ea.tipo_cruzamento = atoi(valor);
            } else if (strcmp(chave, "ea_tipo_mutacao") == 0) {
                config->ea.tipo_mutacao = atoi(valor);
            } else if (strcmp(chave, "ea_tamanho_torneio") == 0) {
                config->ea.tamanho_torneio = atoi(valor);
            }
            // Híbridos
            else if (strcmp(chave, "h1_iter_hc") == 0) {
                config->hib.hibrido1_iter_hc = atoi(valor);
            } else if (strcmp(chave, "h1_pop_ea") == 0) {
                config->hib.hibrido1_pop_ea = atoi(valor);
            } else if (strcmp(chave, "h1_ger_ea") == 0) {
                config->hib.hibrido1_ger_ea = atoi(valor);
            } else if (strcmp(chave, "h2_iter_hc") == 0) {
                config->hib.hibrido2_iter_hc = atoi(valor);
            } else if (strcmp(chave, "h2_pop_ea") == 0) {
                config->hib.hibrido2_pop_ea = atoi(valor);
            } else if (strcmp(chave, "h2_ger_ea") == 0) {
                config->hib.hibrido2_ger_ea = atoi(valor);
            }
        }
    }
    
    fclose(f);
    return config;
}

// Cria ficheiro de configuração padrão
void criar_ficheiro_configuracao_padrao(const char *nome_ficheiro) {
    FILE *f = fopen(nome_ficheiro, "w");
    if (!f) return;
    
    fprintf(f, "# ========================================\n");
    fprintf(f, "# CONFIGURACAO DO OTIMIZADOR TURISTICO\n");
    fprintf(f, "# ========================================\n\n");
    
    fprintf(f, "# --- PARAMETROS GERAIS ---\n");
    fprintf(f, "# Numero de execucoes de cada algoritmo\n");
    fprintf(f, "num_execucoes=10\n\n");
    fprintf(f, "# Seed aleatoria (-1 para usar tempo atual)\n");
    fprintf(f, "seed_aleatoria=-1\n\n");
    
    fprintf(f, "# --- HILL CLIMBING ---\n");
    fprintf(f, "# Numero maximo de iteracoes\n");
    fprintf(f, "hc_max_iteracoes=5000\n\n");
    fprintf(f, "# Tipo de vizinhanca (0=trocar_um, 1=trocar_dois)\n");
    fprintf(f, "hc_vizinhanca=0\n\n");
    fprintf(f, "# Aceitar solucoes de fitness igual (0=nao, 1=sim)\n");
    fprintf(f, "hc_aceitar_iguais=1\n\n");
    
    fprintf(f, "# --- ALGORITMO EVOLUTIVO ---\n");
    fprintf(f, "# Tamanho da populacao\n");
    fprintf(f, "ea_tamanho_populacao=50\n\n");
    fprintf(f, "# Numero de geracoes\n");
    fprintf(f, "ea_num_geracoes=200\n\n");
    fprintf(f, "# Probabilidade de cruzamento (0.0 a 1.0)\n");
    fprintf(f, "ea_prob_cruzamento=0.8\n\n");
    fprintf(f, "# Probabilidade de mutacao (0.0 a 1.0)\n");
    fprintf(f, "ea_prob_mutacao=0.1\n\n");
    fprintf(f, "# Tipo de selecao (0=torneio, 1=roleta)\n");
    fprintf(f, "ea_tipo_selecao=0\n\n");
    fprintf(f, "# Tamanho do torneio (se tipo_selecao=0)\n");
    fprintf(f, "ea_tamanho_torneio=3\n\n");
    fprintf(f, "# Tipo de cruzamento (0=uniforme, 1=um_ponto)\n");
    fprintf(f, "ea_tipo_cruzamento=0\n\n");
    fprintf(f, "# Tipo de mutacao (0=trocar, 1=embaralhar)\n");
    fprintf(f, "ea_tipo_mutacao=0\n\n");
    
    fprintf(f, "# --- HIBRIDO 1 (Evolutivo + Hill Climbing) ---\n");
    fprintf(f, "# Iteracoes do Hill Climbing apos o Evolutivo\n");
    fprintf(f, "h1_iter_hc=500\n\n");
    fprintf(f, "# Tamanho da populacao do Evolutivo\n");
    fprintf(f, "h1_pop_ea=50\n\n");
    fprintf(f, "# Numero de geracoes do Evolutivo\n");
    fprintf(f, "h1_ger_ea=100\n\n");
    
    fprintf(f, "# --- HIBRIDO 2 (Hill Climbing + Evolutivo) ---\n");
    fprintf(f, "# Iteracoes do Hill Climbing inicial\n");
    fprintf(f, "h2_iter_hc=1000\n\n");
    fprintf(f, "# Tamanho da populacao do Evolutivo\n");
    fprintf(f, "h2_pop_ea=30\n\n");
    fprintf(f, "# Numero de geracoes do Evolutivo\n");
    fprintf(f, "h2_ger_ea=50\n");
    
    fclose(f);
    printf("Ficheiro de configuracao criado: %s\n", nome_ficheiro);
}

// Liberta memória da configuração
void libertar_configuracao(Configuracao *config) {
    if (config) free(config);
}

// Mostra configuração atual
void mostrar_configuracao(Configuracao *config) {
    printf("\n========================================\n");
    printf("  CONFIGURACAO CARREGADA\n");
    printf("========================================\n\n");
    
    printf("PARAMETROS GERAIS:\n");
    printf("  Numero de execucoes: %d\n", config->num_execucoes);
    printf("  Seed aleatoria: %d%s\n\n", config->seed_aleatoria, 
           config->seed_aleatoria == -1 ? " (tempo atual)" : "");
    
    printf("HILL CLIMBING:\n");
    printf("  Iteracoes maximas: %d\n", config->hc.max_iteracoes);
    printf("  Vizinhanca: %s\n", config->hc.usar_vizinhanca_2 ? "Trocar Dois" : "Trocar Um");
    printf("  Aceitar iguais: %s\n\n", config->hc.aceitar_iguais ? "Sim" : "Nao");
    
    printf("ALGORITMO EVOLUTIVO:\n");
    printf("  Tamanho populacao: %d\n", config->ea.tamanho_populacao);
    printf("  Numero geracoes: %d\n", config->ea.num_geracoes);
    printf("  Prob. cruzamento: %.2f\n", config->ea.prob_cruzamento);
    printf("  Prob. mutacao: %.2f\n", config->ea.prob_mutacao);
    printf("  Tipo selecao: %s\n", config->ea.tipo_selecao == 0 ? "Torneio" : "Roleta");
    if (config->ea.tipo_selecao == 0) {
        printf("  Tamanho torneio: %d\n", config->ea.tamanho_torneio);
    }
    printf("  Tipo cruzamento: %s\n", config->ea.tipo_cruzamento == 0 ? "Uniforme" : "Um Ponto");
    printf("  Tipo mutacao: %s\n\n", config->ea.tipo_mutacao == 0 ? "Trocar" : "Embaralhar");
    
    printf("HIBRIDO 1 (Evolutivo + HC):\n");
    printf("  Pop. EA: %d, Ger. EA: %d, Iter. HC: %d\n\n", 
           config->hib.hibrido1_pop_ea, config->hib.hibrido1_ger_ea, config->hib.hibrido1_iter_hc);
    
    printf("HIBRIDO 2 (HC + Evolutivo):\n");
    printf("  Iter. HC: %d, Pop. EA: %d, Ger. EA: %d\n", 
           config->hib.hibrido2_iter_hc, config->hib.hibrido2_pop_ea, config->hib.hibrido2_ger_ea);
    
    printf("========================================\n\n");
}