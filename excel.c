//
// Created by Gabriel on 07/12/2025.
//

// c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "xlsxwriter.h"
#include "excel.h"
#include "utils.h"
#include "hill_climbing.h"
#include "evolutivo.h"
#include "hibridos.h"

void escrever_resultados_excel(const char *nome_ficheiro, Problema *prob) {
    lxw_workbook *livro = workbook_new(nome_ficheiro);
    if (!livro) {
        fprintf(stderr, "Erro: impossivel criar livro %s\n", nome_ficheiro);
        fflush(stderr);
        return;
    }

    lxw_worksheet *folha = workbook_add_worksheet(livro, "Resultados");
    if (!folha) {
        fprintf(stderr, "Erro: impossivel criar folha no livro\n");
        workbook_close(livro);
        return;
    }

    int linha = 0;
    worksheet_write_string(folha, linha, 0, "Algoritmo", NULL);
    worksheet_write_string(folha, linha, 1, "Melhor", NULL);
    worksheet_write_string(folha, linha, 2, "Media", NULL);
    worksheet_write_string(folha, linha, 3, "Pior", NULL);
    worksheet_write_string(folha, linha, 4, "Desvio Padrao", NULL);
    linha++;

    printf("\n=== Executando testes ===\n");
    fflush(stdout);

    size_t n = (size_t)NUM_EXECUCOES;
    if (n == 0) {
        fprintf(stderr, "NUM_EXECUCOES = 0\n");
        workbook_close(livro);
        return;
    }

    double *resultados_hc = malloc(sizeof(double) * n);
    double *resultados_ea = malloc(sizeof(double) * n);
    double *resultados_h1 = malloc(sizeof(double) * n);
    double *resultados_h2 = malloc(sizeof(double) * n);

    if (!resultados_hc || !resultados_ea || !resultados_h1 || !resultados_h2) {
        fprintf(stderr, "Erro: sem memoria para resultados\n");
        free(resultados_hc); free(resultados_ea); free(resultados_h1); free(resultados_h2);
        workbook_close(livro);
        return;
    }

    // Hill Climbing
    printf("Hill Climbing...\n"); fflush(stdout);
    for (size_t i = 0; i < n; i++) {
        Solucao *sol = hill_climbing(prob, 5000, 0);
        if (!sol) {
            resultados_hc[i] = NAN;
            fprintf(stderr, "Aviso: hill_climbing devolveu NULL na execucao %zu\n", i + 1);
        } else {
            resultados_hc[i] = sol->fitness;
            libertar_solucao(sol);
        }
        printf("  Execucao %zu: %.2f\n", i + 1, resultados_hc[i]);
    }
    fflush(stdout);
    Estatisticas stats_hc;
    calcular_estatisticas(resultados_hc, (int)n, &stats_hc);

    worksheet_write_string(folha, linha, 0, "Hill Climbing", NULL);
    worksheet_write_number(folha, linha, 1, stats_hc.melhor, NULL);
    worksheet_write_number(folha, linha, 2, stats_hc.media, NULL);
    worksheet_write_number(folha, linha, 3, stats_hc.pior, NULL);
    worksheet_write_number(folha, linha, 4, stats_hc.desvio_padrao, NULL);
    linha++;

    // Algoritmo Evolutivo
    printf("\nAlgoritmo Evolutivo...\n"); fflush(stdout);
    for (size_t i = 0; i < n; i++) {
        Solucao *sol = algoritmo_evolutivo(prob, 50, 200, 0.8, 0.1, 0, 0, 0);
        if (!sol) {
            resultados_ea[i] = NAN;
            fprintf(stderr, "Aviso: algoritmo_evolutivo devolveu NULL na execucao %zu\n", i + 1);
        } else {
            resultados_ea[i] = sol->fitness;
            libertar_solucao(sol);
        }
        printf("  Execucao %zu: %.2f\n", i + 1, resultados_ea[i]);
    }
    fflush(stdout);
    Estatisticas stats_ea;
    calcular_estatisticas(resultados_ea, (int)n, &stats_ea);

    worksheet_write_string(folha, linha, 0, "Evolutivo", NULL);
    worksheet_write_number(folha, linha, 1, stats_ea.melhor, NULL);
    worksheet_write_number(folha, linha, 2, stats_ea.media, NULL);
    worksheet_write_number(folha, linha, 3, stats_ea.pior, NULL);
    worksheet_write_number(folha, linha, 4, stats_ea.desvio_padrao, NULL);
    linha++;

    // Hibrido 1
    printf("\nHibrido 1 (Evolutivo + HC)...\n"); fflush(stdout);
    for (size_t i = 0; i < n; i++) {
        Solucao *sol = hibrido1(prob);
        if (!sol) {
            resultados_h1[i] = NAN;
            fprintf(stderr, "Aviso: hibrido1 devolveu NULL na execucao %zu\n", i + 1);
        } else {
            resultados_h1[i] = sol->fitness;
            libertar_solucao(sol);
        }
        printf("  Execucao %zu: %.2f\n", i + 1, resultados_h1[i]);
    }
    fflush(stdout);
    Estatisticas stats_h1;
    calcular_estatisticas(resultados_h1, (int)n, &stats_h1);

    worksheet_write_string(folha, linha, 0, "Hibrido 1", NULL);
    worksheet_write_number(folha, linha, 1, stats_h1.melhor, NULL);
    worksheet_write_number(folha, linha, 2, stats_h1.media, NULL);
    worksheet_write_number(folha, linha, 3, stats_h1.pior, NULL);
    worksheet_write_number(folha, linha, 4, stats_h1.desvio_padrao, NULL);
    linha++;

    // Hibrido 2
    printf("\nHibrido 2 (HC + Evolutivo)...\n"); fflush(stdout);
    for (size_t i = 0; i < n; i++) {
        Solucao *sol = hibrido2(prob);
        if (!sol) {
            resultados_h2[i] = NAN;
            fprintf(stderr, "Aviso: hibrido2 devolveu NULL na execucao %zu\n", i + 1);
        } else {
            resultados_h2[i] = sol->fitness;
            libertar_solucao(sol);
        }
        printf("  Execucao %zu: %.2f\n", i + 1, resultados_h2[i]);
    }
    fflush(stdout);
    Estatisticas stats_h2;
    calcular_estatisticas(resultados_h2, (int)n, &stats_h2);

    worksheet_write_string(folha, linha, 0, "Hibrido 2", NULL);
    worksheet_write_number(folha, linha, 1, stats_h2.melhor, NULL);
    worksheet_write_number(folha, linha, 2, stats_h2.media, NULL);
    worksheet_write_number(folha, linha, 3, stats_h2.pior, NULL);
    worksheet_write_number(folha, linha, 4, stats_h2.desvio_padrao, NULL);

    free(resultados_hc);
    free(resultados_ea);
    free(resultados_h1);
    free(resultados_h2);

    workbook_close(livro);
    printf("\n=== Resultados salvos em %s ===\n", nome_ficheiro);
    fflush(stdout);
}
