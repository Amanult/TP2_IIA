//
// Created by Gabriel on 07/12/2025.
//

#include <stdlib.h>
#include "hill_climbing.h"
#include "utils.h"

// Vizinhança 1: Trocar um ponto selecionado por um não selecionado
Solucao *vizinho_trocar_um(Solucao *sol, Problema *prob) {
    Solucao *vizinho = copiar_solucao(sol);

    int *nao_selecionados = malloc(sizeof(int) * (prob->num_candidatos - sol->tamanho));
    int contador = 0;

    for (int i = 0; i < prob->num_candidatos; i++) {
        int encontrado = 0;
        for (int j = 0; j < sol->tamanho; j++) {
            if (sol->selecionados[j] == i) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) nao_selecionados[contador++] = i;
    }

    if (contador > 0) {
        // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
        int posicao = rand() % sol->tamanho;
        // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
        int novo_ponto = nao_selecionados[rand() % contador];
        vizinho->selecionados[posicao] = novo_ponto;
    }

    free(nao_selecionados);
    vizinho->fitness = calcular_fitness(vizinho, prob);
    return vizinho;
}

// Vizinhança 2: Trocar dois pontos selecionados por dois não selecionados
Solucao *vizinho_trocar_dois(Solucao *sol, Problema *prob) {
    Solucao *vizinho = copiar_solucao(sol);

    int *nao_selecionados = malloc(sizeof(int) * (prob->num_candidatos - sol->tamanho));
    int contador = 0;

    for (int i = 0; i < prob->num_candidatos; i++) {
        int encontrado = 0;
        for (int j = 0; j < sol->tamanho; j++) {
            if (sol->selecionados[j] == i) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) nao_selecionados[contador++] = i;
    }

    if (contador >= 2 && sol->tamanho >= 2) {
        // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
        int pos1 = rand() % sol->tamanho;
        // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
        int pos2 = rand() % sol->tamanho;
        // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
        while (pos2 == pos1) pos2 = rand() % sol->tamanho;

        // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
        int novo1 = nao_selecionados[rand() % contador];
        // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
        int novo2 = nao_selecionados[rand() % contador];
        // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
        while (novo2 == novo1) novo2 = nao_selecionados[rand() % contador];

        vizinho->selecionados[pos1] = novo1;
        vizinho->selecionados[pos2] = novo2;
    }

    free(nao_selecionados);
    vizinho->fitness = calcular_fitness(vizinho, prob);
    return vizinho;
}

// Hill Climbing
Solucao *hill_climbing(Problema *prob, int max_iteracoes, int usar_vizinho2, int aceitar_iguais) {
    Solucao *atual = criar_solucao_aleatoria(prob);
    Solucao *melhor = copiar_solucao(atual);

    for (int iter = 0; iter < max_iteracoes; iter++) {
        Solucao *vizinho = usar_vizinho2 ?
            vizinho_trocar_dois(atual, prob) : vizinho_trocar_um(atual, prob);

        // Aceita se melhor, ou se igual e aceitar_iguais está ativo
        int aceitar = 0;
        // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
        if (vizinho->fitness > atual->fitness) {
            aceitar = 1;
        } else if (aceitar_iguais && vizinho->fitness == atual->fitness) {
            aceitar = 1;
        }

        if (aceitar) {
            libertar_solucao(atual);
            atual = vizinho;

            if (atual->fitness > melhor->fitness) {
                libertar_solucao(melhor);
                melhor = copiar_solucao(atual);
            }
        } else {
            libertar_solucao(vizinho);
        }
    }

    libertar_solucao(atual);
    return melhor;
}