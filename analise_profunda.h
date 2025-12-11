#ifndef ANALISE_PROFUNDA_H
#define ANALISE_PROFUNDA_H

#include "estruturas.h"
#include "config.h"

// Estrutura para armazenar resultado de uma configuração testada
typedef struct {
    // Identificação
    char algoritmo[50];
    int config_id;

    // Parâmetros testados
    int parametro1;
    int parametro2;
    double parametro3;
    double parametro4;
    char descricao_params[200];

    // Resultados
    double melhor;
    double media;
    double pior;
    double desvio_padrao;
    double tempo_execucao;  // em segundos

} ResultadoTeste;

// Estrutura para armazenar todos os resultados
typedef struct {
    ResultadoTeste *resultados;
    int num_resultados;
    int capacidade;
} ConjuntoResultados;

// Funções principais
void executar_analise_profunda(Problema *prob, const char *ficheiro_saida);
void executar_analise_hill_climbing(Problema *prob, ConjuntoResultados *resultados);
void executar_analise_evolutivo(Problema *prob, ConjuntoResultados *resultados);
void executar_analise_hibridos(Problema *prob, ConjuntoResultados *resultados);

// Funções auxiliares
ConjuntoResultados *criar_conjunto_resultados();
void adicionar_resultado(ConjuntoResultados *conj, ResultadoTeste resultado);
void libertar_conjunto_resultados(ConjuntoResultados *conj);
void escrever_analise_excel(const char *ficheiro, ConjuntoResultados *resultados);
void mostrar_melhores_configuracoes(ConjuntoResultados *resultados);

#endif