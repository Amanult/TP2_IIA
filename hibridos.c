//
// Created by Gabriel on 07/12/2025.
//

#include <stdlib.h>
#include "hibridos.h"
#include "hill_climbing.h"
#include "evolutivo.h"
#include "utils.h"

// Híbrido 1: Evolutivo + Hill Climbing (refinamento)
Solucao *hibrido1(Problema *prob) {
    // Primeiro executar algoritmo evolutivo
    Solucao *sol_ea = algoritmo_evolutivo(prob, 50, 100, 0.8, 0.1, 0, 0, 0);

    // Depois refinar com hill climbing
    for (int i = 0; i < 500; i++) {
        Solucao *vizinho = vizinho_trocar_um(sol_ea, prob);
        if (vizinho->fitness > sol_ea->fitness) {
            libertar_solucao(sol_ea);
            sol_ea = vizinho;
        } else {
            libertar_solucao(vizinho);
        }
    }

    return sol_ea;
}

// Híbrido 2: Hill Climbing inicial + Evolutivo
Solucao *hibrido2(Problema *prob) {
    // Primeiro executar hill climbing
    Solucao *sol_hc = hill_climbing(prob, 1000, 0);

    // Criar população com a solução do hill climbing
    Populacao *pop = criar_populacao(prob, 30);
    libertar_solucao(pop->populacao[0]);
    pop->populacao[0] = sol_hc;

    Solucao *melhor = copiar_solucao(sol_hc);

    // Executar evolutivo
    for (int ger = 0; ger < 50; ger++) {
        Populacao *nova_pop = malloc(sizeof(Populacao));
        nova_pop->tamanho = 30;
        nova_pop->populacao = malloc(30 * sizeof(Solucao*));

        // Elitismo: manter o melhor
        nova_pop->populacao[0] = copiar_solucao(melhor);

        for (int i = 1; i < 30; i++) {
            Solucao *p1 = selecao_torneio(pop, 3);
            Solucao *p2 = selecao_torneio(pop, 3);
            Solucao *filho = recombinacao_uniforme(p1, p2, prob);

            if ((double)rand() / RAND_MAX < 0.1) {
                mutacao_trocar(filho, prob);
            }

            nova_pop->populacao[i] = filho;

            if (filho->fitness > melhor->fitness) {
                libertar_solucao(melhor);
                melhor = copiar_solucao(filho);
            }
        }

        libertar_populacao(pop);
        pop = nova_pop;
    }

    libertar_populacao(pop);
    return melhor;
}