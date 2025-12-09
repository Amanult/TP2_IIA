#include <stdlib.h>
#include "hibridos.h"
#include "hill_climbing.h"
#include "evolutivo.h"
#include "utils.h"
#include "config.h"

// Híbrido 1: Evolutivo + Hill Climbing (refinamento)
Solucao *hibrido1(Problema *prob, Configuracao *config) {
    // Primeiro executar algoritmo evolutivo
    Solucao *sol_ea = algoritmo_evolutivo(prob,
                                          config->hib.hibrido1_pop_ea,
                                          config->hib.hibrido1_ger_ea,
                                          config->ea.prob_cruzamento,
                                          config->ea.prob_mutacao,
                                          config->ea.tipo_selecao,
                                          config->ea.tipo_cruzamento,
                                          config->ea.tipo_mutacao,
                                          config->ea.tamanho_torneio);

    // Depois refinar com hill climbing
    for (int i = 0; i < config->hib.hibrido1_iter_hc; i++) {
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
Solucao *hibrido2(Problema *prob, Configuracao *config) {
    // Primeiro executar hill climbing
    Solucao *sol_hc = hill_climbing(prob,
                                    config->hib.hibrido2_iter_hc,
                                    config->hc.usar_vizinhanca_2,
                                    config->hc.aceitar_iguais);

    // Criar população com a solução do hill climbing
    Populacao *pop = criar_populacao(prob, config->hib.hibrido2_pop_ea);
    libertar_solucao(pop->populacao[0]);
    pop->populacao[0] = sol_hc;

    Solucao *melhor = copiar_solucao(sol_hc);

    // Executar evolutivo
    // NOLINT
    for (int ger = 0; ger < config->hib.hibrido2_ger_ea; ger++) {
        Populacao *nova_pop = malloc(sizeof(Populacao));  // NOLINT
        nova_pop->tamanho = config->hib.hibrido2_pop_ea;
        nova_pop->populacao = malloc(config->hib.hibrido2_pop_ea * sizeof(Solucao*));  // NOLINT

        // Elitismo: manter o melhor
        nova_pop->populacao[0] = copiar_solucao(melhor);

        for (int i = 1; i < config->hib.hibrido2_pop_ea; i++) {
            Solucao *p1, *p2;

            if (config->ea.tipo_selecao == 0) {
                p1 = selecao_torneio(pop, config->ea.tamanho_torneio);
                p2 = selecao_torneio(pop, config->ea.tamanho_torneio);
            } else {
                p1 = selecao_roleta(pop);
                p2 = selecao_roleta(pop);
            }

            Solucao *filho;
            if ((double)rand() / RAND_MAX < config->ea.prob_cruzamento) {
                if (config->ea.tipo_cruzamento == 0) {
                    filho = recombinacao_uniforme(p1, p2, prob);
                } else {
                    filho = recombinacao_um_ponto(p1, p2, prob);
                }
            } else {
                filho = copiar_solucao(p1);
            }

            if ((double)rand() / RAND_MAX < config->ea.prob_mutacao) {
                if (config->ea.tipo_mutacao == 0) {
                    mutacao_trocar(filho, prob);
                } else {
                    mutacao_embaralhar(filho, prob);
                }
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