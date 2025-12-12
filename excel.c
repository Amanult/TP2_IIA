#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "xlsxwriter.h"
#include "excel.h"
#include "utils.h"
#include "hill_climbing.h"
#include "evolutivo.h"
#include "hibridos.h"
#include "config.h"

// Estrutura para armazenar o melhor fitness e a solucao correspondente
typedef struct {
    double fitness;
    Solucao *solucao;
} MelhorSolucao;

// Funcao auxiliar para escrever cabecalhos de folha de algoritmo
void escrever_cabecalho_algoritmo(lxw_worksheet *folha, const char *nome_algoritmo) {
    worksheet_write_string(folha, 0, 0, "Execucao", NULL);
    worksheet_write_string(folha, 0, 1, "Fitness", NULL);
    // Adicionar mais colunas se necessario para detalhes da solucao
}

// Funcao auxiliar para escrever resultados de uma execucao
void escrever_linha_algoritmo(lxw_worksheet *folha, int linha, int execucao_num, double fitness) {
    worksheet_write_number(folha, linha, 0, execucao_num, NULL);
    worksheet_write_number(folha, linha, 1, fitness, NULL);
}

void escrever_resultados_excel(const char *nome_ficheiro, Problema *prob, Configuracao *config) {
    lxw_workbook *livro = workbook_new(nome_ficheiro);
    if (!livro) {
        fprintf(stderr, "Erro: impossivel criar livro %s\n", nome_ficheiro);
        fflush(stderr);
        return;
    }

    // A folha de resumo deve ficar no fim, por isso vamos criá-la só depois das execuções
    lxw_worksheet *folha_resumo = NULL;
    int linha_resumo = 1;

    // Folhas para cada algoritmo
    lxw_worksheet *folha_hc = workbook_add_worksheet(livro, "Hill Climbing");
    lxw_worksheet *folha_ea = workbook_add_worksheet(livro, "Evolutivo");
    lxw_worksheet *folha_h1 = workbook_add_worksheet(livro, "Hibrido 1");
    lxw_worksheet *folha_h2 = workbook_add_worksheet(livro, "Hibrido 2");

    if (!folha_hc || !folha_ea || !folha_h1 || !folha_h2) {
        fprintf(stderr, "Erro: impossivel criar folhas de algoritmo no livro\n");
        workbook_close(livro);
        return;
    }

    escrever_cabecalho_algoritmo(folha_hc, "Hill Climbing");
    escrever_cabecalho_algoritmo(folha_ea, "Algoritmo Evolutivo");
    escrever_cabecalho_algoritmo(folha_h1, "Hibrido 1");
    escrever_cabecalho_algoritmo(folha_h2, "Hibrido 2");

    printf("\n=== Executando testes ===\n");
    fflush(stdout);

    size_t n = (size_t)config->num_execucoes;
    if (n == 0) {
        fprintf(stderr, "Erro: num_execucoes = 0\n");
        workbook_close(livro);
        return;
    }

    MelhorSolucao melhor_hc = { .fitness = -INFINITY, .solucao = NULL };
    MelhorSolucao melhor_ea = { .fitness = -INFINITY, .solucao = NULL };
    MelhorSolucao melhor_h1 = { .fitness = -INFINITY, .solucao = NULL };
    MelhorSolucao melhor_h2 = { .fitness = -INFINITY, .solucao = NULL };
    int melhor_exec_hc = -1;
    int melhor_exec_ea = -1;
    int melhor_exec_h1 = -1;
    int melhor_exec_h2 = -1;

    // Hill Climbing
    printf("\nHill Climbing...\n");
    printf("  Parametros: iter=%d, vizinhanca=%d, aceitar_iguais=%d\n", 
           config->hc.max_iteracoes, config->hc.usar_vizinhanca_2, config->hc.aceitar_iguais);
    fflush(stdout);
    
    for (size_t i = 0; i < n; i++) {
        Solucao *sol = hill_climbing(prob, config->hc.max_iteracoes, 
                                     config->hc.usar_vizinhanca_2, 
                                     config->hc.aceitar_iguais);
        if (!sol) {
            fprintf(stderr, "Aviso: hill_climbing devolveu NULL na execucao %zu\n", i + 1);
        } else {
            escrever_linha_algoritmo(folha_hc, (int)i + 1, (int)i + 1, sol->fitness);
            if (sol->fitness > melhor_hc.fitness) {
                if (melhor_hc.solucao) libertar_solucao(melhor_hc.solucao);
                melhor_hc.fitness = sol->fitness;
                melhor_hc.solucao = copiar_solucao(sol);
                melhor_exec_hc = (int)i + 1;
            }
            libertar_solucao(sol);
        }
        printf("  Execucao %zu: %.2f\n", i + 1, (sol ? sol->fitness : NAN));
    }
    fflush(stdout);

    // Algoritmo Evolutivo
    printf("\nAlgoritmo Evolutivo...\n");
    printf("  Parametros: pop=%d, ger=%d, cruz=%.2f, mut=%.2f, sel=%d, cruz_tipo=%d, mut_tipo=%d\n",
           config->ea.tamanho_populacao, config->ea.num_geracoes, 
           config->ea.prob_cruzamento, config->ea.prob_mutacao,
           config->ea.tipo_selecao, config->ea.tipo_cruzamento, config->ea.tipo_mutacao);
    fflush(stdout);
    
    for (size_t i = 0; i < n; i++) {
        Solucao *sol = algoritmo_evolutivo(prob, 
                                           config->ea.tamanho_populacao,
                                           config->ea.num_geracoes,
                                           config->ea.prob_cruzamento,
                                           config->ea.prob_mutacao,
                                           config->ea.tipo_selecao,
                                           config->ea.tipo_cruzamento,
                                           config->ea.tipo_mutacao,
                                           config->ea.tamanho_torneio);
        if (!sol) {
            fprintf(stderr, "Aviso: algoritmo_evolutivo devolveu NULL na execucao %zu\n", i + 1);
        } else {
            escrever_linha_algoritmo(folha_ea, (int)i + 1, (int)i + 1, sol->fitness);
            if (sol->fitness > melhor_ea.fitness) {
                if (melhor_ea.solucao) libertar_solucao(melhor_ea.solucao);
                melhor_ea.fitness = sol->fitness;
                melhor_ea.solucao = copiar_solucao(sol);
                melhor_exec_ea = (int)i + 1;
            }
            libertar_solucao(sol);
        }
        printf("  Execucao %zu: %.2f\n", i + 1, (sol ? sol->fitness : NAN));
    }
    fflush(stdout);

    // Hibrido 1
    printf("\nHibrido 1 (Evolutivo + HC)...\n");
    printf("  Parametros: EA(pop=%d, ger=%d) + HC(iter=%d)\n",
           config->hib.hibrido1_pop_ea, config->hib.hibrido1_ger_ea, config->hib.hibrido1_iter_hc);
    fflush(stdout);
    
    for (size_t i = 0; i < n; i++) {
        Solucao *sol = hibrido1(prob, config);
        if (!sol) {
            fprintf(stderr, "Aviso: hibrido1 devolveu NULL na execucao %zu\n", i + 1);
        } else {
            escrever_linha_algoritmo(folha_h1, (int)i + 1, (int)i + 1, sol->fitness);
            if (sol->fitness > melhor_h1.fitness) {
                if (melhor_h1.solucao) libertar_solucao(melhor_h1.solucao);
                melhor_h1.fitness = sol->fitness;
                melhor_h1.solucao = copiar_solucao(sol);
                melhor_exec_h1 = (int)i + 1;
            }
            libertar_solucao(sol);
        }
        printf("  Execucao %zu: %.2f\n", i + 1, (sol ? sol->fitness : NAN));
    }
    fflush(stdout);

    // Hibrido 2
    printf("\nHibrido 2 (HC + Evolutivo)...\n");
    printf("  Parametros: HC(iter=%d) + EA(pop=%d, ger=%d)\n",
           config->hib.hibrido2_iter_hc, config->hib.hibrido2_pop_ea, config->hib.hibrido2_ger_ea);
    fflush(stdout);
    
    for (size_t i = 0; i < n; i++) {
        Solucao *sol = hibrido2(prob, config);
        if (!sol) {
            fprintf(stderr, "Aviso: hibrido2 devolveu NULL na execucao %zu\n", i + 1);
        } else {
            escrever_linha_algoritmo(folha_h2, (int)i + 1, (int)i + 1, sol->fitness);
            if (sol->fitness > melhor_h2.fitness) {
                if (melhor_h2.solucao) libertar_solucao(melhor_h2.solucao);
                melhor_h2.fitness = sol->fitness;
                melhor_h2.solucao = copiar_solucao(sol);
                melhor_exec_h2 = (int)i + 1;
            }
            libertar_solucao(sol);
        }
        printf("  Execucao %zu: %.2f\n", i + 1, (sol ? sol->fitness : NAN));
    }
    fflush(stdout);

    // Criar a folha de resumo por último para que apareça no fim do ficheiro
    folha_resumo = workbook_add_worksheet(livro, "Resumo");
    if (!folha_resumo) {
        fprintf(stderr, "Erro: impossivel criar folha de resumo no livro\n");
        // Mesmo sem a folha de resumo, fechar o livro depois de libertar memórias
    } else {
        worksheet_write_string(folha_resumo, 0, 0, "Algoritmo", NULL);
        worksheet_write_string(folha_resumo, 0, 1, "Melhor Fitness", NULL);
        worksheet_write_string(folha_resumo, 0, 2, "Melhor Iteracao", NULL);

        // Escrever resumo
        worksheet_write_string(folha_resumo, linha_resumo, 0, "Hill Climbing", NULL);
        worksheet_write_number(folha_resumo, linha_resumo, 1, melhor_hc.fitness, NULL);
        worksheet_write_number(folha_resumo, linha_resumo, 2, melhor_exec_hc, NULL);
        linha_resumo++;

        worksheet_write_string(folha_resumo, linha_resumo, 0, "Evolutivo", NULL);
        worksheet_write_number(folha_resumo, linha_resumo, 1, melhor_ea.fitness, NULL);
        worksheet_write_number(folha_resumo, linha_resumo, 2, melhor_exec_ea, NULL);
        linha_resumo++;

        worksheet_write_string(folha_resumo, linha_resumo, 0, "Hibrido 1", NULL);
        worksheet_write_number(folha_resumo, linha_resumo, 1, melhor_h1.fitness, NULL);
        worksheet_write_number(folha_resumo, linha_resumo, 2, melhor_exec_h1, NULL);
        linha_resumo++;

        worksheet_write_string(folha_resumo, linha_resumo, 0, "Hibrido 2", NULL);
        worksheet_write_number(folha_resumo, linha_resumo, 1, melhor_h2.fitness, NULL);
        worksheet_write_number(folha_resumo, linha_resumo, 2, melhor_exec_h2, NULL);
        linha_resumo++;
    }

    // Libertar solucoes guardadas
    if (melhor_hc.solucao) libertar_solucao(melhor_hc.solucao);
    if (melhor_ea.solucao) libertar_solucao(melhor_ea.solucao);
    if (melhor_h1.solucao) libertar_solucao(melhor_h1.solucao);
    if (melhor_h2.solucao) libertar_solucao(melhor_h2.solucao);

    workbook_close(livro);
    printf("\n=== Resultados salvos em %s ===\n", nome_ficheiro);
    fflush(stdout);
}