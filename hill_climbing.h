/*
 * Módulo: hill_climbing.h
 * ------------------------------------------------------------
 * Declara a API do algoritmo Hill Climbing e das funções de
 * geração de vizinhos usadas para explorar a vizinhança de
 * uma solução.
 */
#ifndef HILL_CLIMBING_H
#define HILL_CLIMBING_H

#include "estruturas.h"

// Funções de vizinhança
Solucao *vizinho_trocar_um(Solucao *sol, Problema *prob);
Solucao *vizinho_trocar_dois(Solucao *sol, Problema *prob);

// Algoritmo Hill Climbing
Solucao *hill_climbing(Problema *prob, int max_iteracoes, int usar_vizinho2, int aceitar_iguais);

#endif