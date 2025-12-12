#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <pthread.h>
#include "evolutivo.h"
#include "utils.h"

// Mutex global para proteger o RNG entre threads (definido em analise_profunda.c)
extern pthread_mutex_t mutex_rand;

// [Manter as funções criar_populacao, libertar_populacao, selecao_roleta,
//  recombinacao_uniforme, recombinacao_um_ponto, mutacao_trocar, mutacao_embaralhar
//  exatamente como estão no ficheiro original]

// Cria população inicial
Populacao *criar_populacao(Problema *prob, int tamanho_pop) {
    Populacao *pop = malloc(sizeof(Populacao));
    pop->tamanho = tamanho_pop;
    pop->populacao = malloc(tamanho_pop * sizeof(Solucao*));

    for (int i = 0; i < tamanho_pop; i++) {
        pop->populacao[i] = criar_solucao_aleatoria(prob);
    }

    return pop;
}

void libertar_populacao(Populacao *pop) {
    for (int i = 0; i < pop->tamanho; i++) {
        libertar_solucao(pop->populacao[i]);
    }
    free(pop->populacao);
    free(pop);
}

// Seleção por torneio com tamanho configurável
Solucao *selecao_torneio(Populacao *pop, int tamanho_torneio) {
    Solucao *melhor = NULL;
    for (int i = 0; i < tamanho_torneio; i++) {
        pthread_mutex_lock(&mutex_rand);
        Solucao *candidato = pop->populacao[rand() % pop->tamanho];
        pthread_mutex_unlock(&mutex_rand);
        if (!melhor || candidato->fitness > melhor->fitness) {
            melhor = candidato;
        }
    }
    return melhor;
}

// Seleção por roleta
Solucao *selecao_roleta(Populacao *pop) {
    double total = 0.0;
    double fitness_minimo = DBL_MAX;

    for (int i = 0; i < pop->tamanho; i++) {
        if (pop->populacao[i]->fitness < fitness_minimo) {
            fitness_minimo = pop->populacao[i]->fitness;
        }
    }

    for (int i = 0; i < pop->tamanho; i++) {
        total += pop->populacao[i]->fitness - fitness_minimo + 1.0;
    }

    pthread_mutex_lock(&mutex_rand);
    double r = ((double)rand() / RAND_MAX) * total;
    pthread_mutex_unlock(&mutex_rand);
    double soma = 0.0;

    for (int i = 0; i < pop->tamanho; i++) {
        soma += pop->populacao[i]->fitness - fitness_minimo + 1.0;
        if (soma >= r) return pop->populacao[i];
    }

    return pop->populacao[pop->tamanho - 1];
}

// Recombinação uniforme
Solucao *recombinacao_uniforme(Solucao *p1, Solucao *p2, Problema *prob) {
    Solucao *filho = malloc(sizeof(Solucao));
    filho->tamanho = prob->num_selecionados;
    filho->selecionados = malloc(filho->tamanho * sizeof(int));

    int *todos = malloc((p1->tamanho + p2->tamanho) * sizeof(int));
    memcpy(todos, p1->selecionados, p1->tamanho * sizeof(int));
    memcpy(todos + p1->tamanho, p2->selecionados, p2->tamanho * sizeof(int));

    int contador_unicos = 0;
    for (int i = 0; i < p1->tamanho + p2->tamanho; i++) {
        int eh_unico = 1;
        for (int j = 0; j < contador_unicos; j++) {
            if (todos[j] == todos[i]) {
                eh_unico = 0;
                break;
            }
        }
        if (eh_unico) todos[contador_unicos++] = todos[i];
    }

    if (contador_unicos >= filho->tamanho) {
        for (int i = 0; i < filho->tamanho; i++) {
            pthread_mutex_lock(&mutex_rand);
            int idx = rand() % contador_unicos;
            pthread_mutex_unlock(&mutex_rand);
            filho->selecionados[i] = todos[idx];
            todos[idx] = todos[--contador_unicos];
        }
    } else {
        int contador = 0;
        for (int i = 0; i < contador_unicos; i++) {
            filho->selecionados[contador++] = todos[i];
        }

        while (contador < filho->tamanho) {
            pthread_mutex_lock(&mutex_rand);
            int candidato = rand() % prob->num_candidatos;
            pthread_mutex_unlock(&mutex_rand);
            int encontrado = 0;
            for (int i = 0; i < contador; i++) {
                if (filho->selecionados[i] == candidato) {
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) filho->selecionados[contador++] = candidato;
        }
    }

    free(todos);
    filho->fitness = calcular_fitness(filho, prob);
    return filho;
}

// Recombinação de um ponto
Solucao *recombinacao_um_ponto(Solucao *p1, Solucao *p2, Problema *prob) {
    Solucao *filho = malloc(sizeof(Solucao));
    filho->tamanho = prob->num_selecionados;
    filho->selecionados = malloc(filho->tamanho * sizeof(int));

    pthread_mutex_lock(&mutex_rand);
    int ponto = rand() % filho->tamanho;
    pthread_mutex_unlock(&mutex_rand);

    for (int i = 0; i < ponto; i++) {
        filho->selecionados[i] = p1->selecionados[i];
    }

    int contador = ponto;
    for (int i = 0; i < p2->tamanho && contador < filho->tamanho; i++) {
        int encontrado = 0;
        for (int j = 0; j < ponto; j++) {
            if (filho->selecionados[j] == p2->selecionados[i]) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) filho->selecionados[contador++] = p2->selecionados[i];
    }

    while (contador < filho->tamanho) {
        pthread_mutex_lock(&mutex_rand);
        int candidato = rand() % prob->num_candidatos;
        pthread_mutex_unlock(&mutex_rand);
        int encontrado = 0;
        for (int i = 0; i < contador; i++) {
            if (filho->selecionados[i] == candidato) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) filho->selecionados[contador++] = candidato;
    }

    filho->fitness = calcular_fitness(filho, prob);
    return filho;
}

// Mutação: trocar um gene
void mutacao_trocar(Solucao *sol, Problema *prob) {
    pthread_mutex_lock(&mutex_rand);
    int posicao = rand() % sol->tamanho;
    pthread_mutex_unlock(&mutex_rand);
    int novo_valor;

    do {
        pthread_mutex_lock(&mutex_rand);
        novo_valor = rand() % prob->num_candidatos;
        pthread_mutex_unlock(&mutex_rand);
        int encontrado = 0;
        for (int i = 0; i < sol->tamanho; i++) {
            if (i != posicao && sol->selecionados[i] == novo_valor) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) break;
    } while (1);

    sol->selecionados[posicao] = novo_valor;
    sol->fitness = calcular_fitness(sol, prob);
}

// Mutação: embaralhar
void mutacao_embaralhar(Solucao *sol, Problema *prob) {
    for (int i = sol->tamanho - 1; i > 0; i--) {
        pthread_mutex_lock(&mutex_rand);
        int j = rand() % (i + 1);
        pthread_mutex_unlock(&mutex_rand);
        int temp = sol->selecionados[i];
        sol->selecionados[i] = sol->selecionados[j];
        sol->selecionados[j] = temp;
    }
    sol->fitness = calcular_fitness(sol, prob);
}

// Algoritmo Evolutivo - ATUALIZADO com tamanho_torneio
Solucao *algoritmo_evolutivo(Problema *prob, int tamanho_pop, int geracoes,
                             double prob_cruzamento, double prob_mutacao,
                             int tipo_selecao, int tipo_cruzamento, int tipo_mutacao,
                             int tamanho_torneio) {
    Populacao *pop = criar_populacao(prob, tamanho_pop);
    Solucao *melhor = copiar_solucao(pop->populacao[0]);

    for (int i = 1; i < pop->tamanho; i++) {
        if (pop->populacao[i]->fitness > melhor->fitness) {
            libertar_solucao(melhor);
            melhor = copiar_solucao(pop->populacao[i]);
        }
    }

    for (int ger = 0; ger < geracoes; ger++) {
        Populacao *nova_pop = malloc(sizeof(Populacao));
        nova_pop->tamanho = tamanho_pop;
        nova_pop->populacao = malloc(tamanho_pop * sizeof(Solucao*));

        for (int i = 0; i < tamanho_pop; i++) {
            Solucao *p1, *p2;

            if (tipo_selecao == 0) {
                p1 = selecao_torneio(pop, tamanho_torneio);
                p2 = selecao_torneio(pop, tamanho_torneio);
            } else {
                p1 = selecao_roleta(pop);
                p2 = selecao_roleta(pop);
            }

            Solucao *filho;
            pthread_mutex_lock(&mutex_rand);
            double r_cruz = (double)rand() / RAND_MAX;
            pthread_mutex_unlock(&mutex_rand);
            if (r_cruz < prob_cruzamento) {
                if (tipo_cruzamento == 0) {
                    filho = recombinacao_uniforme(p1, p2, prob);
                } else {
                    filho = recombinacao_um_ponto(p1, p2, prob);
                }
            } else {
                filho = copiar_solucao(p1);
            }

            pthread_mutex_lock(&mutex_rand);
            double r_mut = (double)rand() / RAND_MAX;
            pthread_mutex_unlock(&mutex_rand);
            if (r_mut < prob_mutacao) {
                if (tipo_mutacao == 0) {
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