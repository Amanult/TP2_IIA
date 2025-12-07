#ifndef UTILS_H
#define UTILS_H

#include "estruturas.h"

// Funções de leitura e cálculo
int ler_problema(const char *nome_ficheiro, Problema *prob);
void libertar_problema(Problema *prob);
double calcular_fitness(Solucao *sol, Problema *prob);

// Funções de manipulação de soluções
Solucao *criar_solucao_aleatoria(Problema *prob);
Solucao *copiar_solucao(Solucao *sol);
void libertar_solucao(Solucao *sol);

// Funções de estatísticas
void calcular_estatisticas(double *valores, int n, Estatisticas *stats);

#endif