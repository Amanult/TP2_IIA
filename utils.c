#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "utils.h"

// Mutex global para proteger o RNG entre threads (definido em analise_profunda.c)
extern pthread_mutex_t mutex_rand;

// Lê o ficheiro de entrada
int ler_problema(const char *nome_ficheiro, Problema *prob) {
    FILE *f = fopen(nome_ficheiro, "r");
    if (!f) return 0;

    fscanf(f, "%d %d", &prob->num_candidatos, &prob->num_selecionados);

    // Alocar memória dinamicamente para evitar stack overflow
    prob->nomes = (char **)malloc(prob->num_candidatos * sizeof(char *));
    for (int i = 0; i < prob->num_candidatos; i++) {
        prob->nomes[i] = (char *)malloc(MAX_NOME * sizeof(char));
        sprintf(prob->nomes[i], "e%d", i + 1);
    }

    // Alocar matriz de distâncias dinamicamente
    prob->distancias = (double **)malloc(prob->num_candidatos * sizeof(double *));
    for (int i = 0; i < prob->num_candidatos; i++) {
        prob->distancias[i] = (double *)malloc(prob->num_candidatos * sizeof(double));
        for (int j = 0; j < prob->num_candidatos; j++) {
            prob->distancias[i][j] = (i == j) ? 0.0 : -1.0;
        }
    }

    // Ler distâncias
    char nome1[MAX_NOME], nome2[MAX_NOME];
    double dist;
    while (fscanf(f, "%s %s %lf", nome1, nome2, &dist) == 3) {
        int i = atoi(nome1 + 1) - 1;
        int j = atoi(nome2 + 1) - 1;
        prob->distancias[i][j] = dist;
        prob->distancias[j][i] = dist;
    }

    fclose(f);
    return 1;
}

// Liberta memória do problema
void libertar_problema(Problema *prob) {
    if (prob->nomes) {
        for (int i = 0; i < prob->num_candidatos; i++) {
            free(prob->nomes[i]);
        }
        free(prob->nomes);
    }

    if (prob->distancias) {
        for (int i = 0; i < prob->num_candidatos; i++) {
            free(prob->distancias[i]);
        }
        free(prob->distancias);
    }
}

// Calcula a fitness (distância média)
double calcular_fitness(Solucao *sol, Problema *prob) {
    if (sol->tamanho != prob->num_selecionados) return -1.0;

    double soma = 0.0;
    int contador = 0;

    for (int i = 0; i < sol->tamanho; i++) {
        for (int j = i + 1; j < sol->tamanho; j++) {
            soma += prob->distancias[sol->selecionados[i]][sol->selecionados[j]];
            contador++;
        }
    }

    return contador > 0 ? soma / sol->tamanho : 0.0;
}

// Cria solução aleatória
Solucao *criar_solucao_aleatoria(Problema *prob) {
    Solucao *sol = malloc(sizeof(Solucao));
    sol->tamanho = prob->num_selecionados;
    sol->selecionados = malloc(sol->tamanho * sizeof(int));

    int *disponiveis = malloc(prob->num_candidatos * sizeof(int));
    for (int i = 0; i < prob->num_candidatos; i++) disponiveis[i] = i;

    for (int i = 0; i < sol->tamanho; i++) {
        pthread_mutex_lock(&mutex_rand);
        int idx = rand() % (prob->num_candidatos - i);
        pthread_mutex_unlock(&mutex_rand);
        sol->selecionados[i] = disponiveis[idx];
        disponiveis[idx] = disponiveis[prob->num_candidatos - i - 1];
    }

    free(disponiveis);
    sol->fitness = calcular_fitness(sol, prob);
    return sol;
}

// Copia solução
Solucao *copiar_solucao(Solucao *sol) {
    Solucao *copia = malloc(sizeof(Solucao));
    copia->tamanho = sol->tamanho;
    copia->fitness = sol->fitness;
    copia->selecionados = malloc(sol->tamanho * sizeof(int));
    memcpy(copia->selecionados, sol->selecionados, sol->tamanho * sizeof(int));
    return copia;
}

void libertar_solucao(Solucao *sol) {
    if (sol) {
        free(sol->selecionados);
        free(sol);
    }
}

// Calcula estatísticas (robusto para n > NUM_EXECUCOES)
void calcular_estatisticas(double *valores, int n, Estatisticas *stats) {
    if (n <= 0) {
        stats->melhor = 0.0;
        stats->pior = 0.0;
        stats->media = 0.0;
        stats->desvio_padrao = 0.0;
        return;
    }

    stats->melhor = valores[0];
    stats->pior = valores[0];
    stats->media = 0.0;

    // Copiar no máximo NUM_EXECUCOES valores para o array fixo, para evitar overflow
    int limite = n < NUM_EXECUCOES ? n : NUM_EXECUCOES;
    for (int i = 0; i < limite; i++) {
        stats->execucoes[i] = valores[i];
    }

    for (int i = 0; i < n; i++) {
        double v = valores[i];
        stats->media += v;
        if (v > stats->melhor) stats->melhor = v;
        if (v < stats->pior) stats->pior = v;
    }
    stats->media /= n;

    double variancia = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = valores[i] - stats->media;
        variancia += diff * diff;
    }
    stats->desvio_padrao = sqrt(variancia / n);
}