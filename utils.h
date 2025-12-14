/*
 * Módulo: utils.h
 * ------------------------------------------------------------
 * Declara funções utilitárias para:
 *  - Ler o problema a partir de ficheiro de texto;
 *  - Calcular o fitness de uma solução;
 *  - Criar, copiar e libertar soluções;
 *  - Calcular estatísticas sobre conjuntos de execuções.
 * As implementações encontram‑se em utils.c.
 */
#ifndef UTILS_H
#define UTILS_H

#include "estruturas.h"

// Funções de leitura e cálculo
// Lê um ficheiro de entrada e preenche a estrutura Problema. Retorna 1 em sucesso.
int ler_problema(const char *nome_ficheiro, Problema *prob);
void libertar_problema(Problema *prob);
// Calcula a média das distâncias entre todos os pares de pontos selecionados
double calcular_fitness(Solucao *sol, Problema *prob);

// Funções de manipulação de soluções
Solucao *criar_solucao_aleatoria(Problema *prob);
Solucao *copiar_solucao(Solucao *sol);
void libertar_solucao(Solucao *sol);

// Funções de estatísticas
// Preenche Estatisticas (melhor, pior, média, desvio padrão) para um vetor de valores
void calcular_estatisticas(double *valores, int n, Estatisticas *stats);

#endif