#ifndef EVOLUTIVO_H
#define EVOLUTIVO_H

#include "estruturas.h"

// Funções de população
Populacao *criar_populacao(Problema *prob, int tamanho_pop);
void libertar_populacao(Populacao *pop);

// Funções de seleção
Solucao *selecao_torneio(Populacao *pop, int tamanho_torneio);
Solucao *selecao_roleta(Populacao *pop);

// Funções de recombinação
Solucao *recombinacao_uniforme(Solucao *p1, Solucao *p2, Problema *prob);
Solucao *recombinacao_um_ponto(Solucao *p1, Solucao *p2, Problema *prob);

// Funções de mutação
void mutacao_trocar(Solucao *sol, Problema *prob);
void mutacao_embaralhar(Solucao *sol, Problema *prob);

// Algoritmo Evolutivo
Solucao *algoritmo_evolutivo(Problema *prob, int tamanho_pop, int geracoes,
                             double prob_cruzamento, double prob_mutacao,
                             int tipo_selecao, int tipo_cruzamento, int tipo_mutacao,
                             int tamanho_torneio);

#endif