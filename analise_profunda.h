#ifndef ANALISE_PROFUNDA_H
#define ANALISE_PROFUNDA_H

#include "estruturas.h"
#include "config.h"
#include <pthread.h> // Para multithreading

typedef struct {
    char algoritmo[50];
    int config_id;
    int parametro1;
    int parametro2;
    double parametro3;
    double parametro4;
    char descricao_params[200];

    double melhor;
    double media;
    double pior;
    double desvio_padrao;
    double tempo_execucao;
} ResultadoTeste;

typedef struct {
    ResultadoTeste *resultados;
    int num_resultados;
    int capacidade;
} ConjuntoResultados;

extern pthread_mutex_t mutex_rand;

// Alterado para receber a configuração completa
void executar_analise_profunda(Problema *prob, Configuracao *config, const char *ficheiro_saida);

ConjuntoResultados *criar_conjunto_resultados();
void libertar_conjunto_resultados(ConjuntoResultados *conj);
void escrever_analise_excel(const char *ficheiro, ConjuntoResultados *resultados);
void mostrar_melhores_configuracoes(ConjuntoResultados *resultados);
void adicionar_resultado(ConjuntoResultados *conj, ResultadoTeste resultado);

#endif