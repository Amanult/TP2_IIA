/*
 * Módulo: estruturas.h
 * ------------------------------------------------------------
 * Define todas as estruturas de dados centrais do projeto:
 *  - Problema: dados de entrada (número de candidatos e distâncias);
 *  - Solucao: representação de uma solução candidata e o seu fitness;
 *  - Populacao: coleção de soluções (usada no algoritmo evolutivo);
 *  - Estatisticas: resumo estatístico de execuções.
 * Estas estruturas são usadas por praticamente todos os módulos.
 */
#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#define MAX_CANDIDATOS 500
#define MAX_NOME 20
#define NUM_EXECUCOES 10

// Estrutura para armazenar informação do problema
typedef struct {
    int num_candidatos;  // C: número total de candidaturas recebidas
    int num_selecionados;    // M: número de pontos a selecionar
    char **nomes;  // Alocação dinâmica
    double **distancias;  // Alocação dinâmica para evitar stack overflow
} Problema;

// Estrutura para uma solução
typedef struct {
    int *selecionados;  // Array de índices selecionados
    int tamanho;        // Tamanho (deve ser m)
    double fitness;     // Distância média
} Solucao;

// Estrutura para população
typedef struct {
    Solucao **populacao;
    int tamanho;
} Populacao;

// Estrutura para resultados estatísticos
typedef struct {
    double melhor;
    double pior;
    double media;
    double desvio_padrao;
    double execucoes[NUM_EXECUCOES];
} Estatisticas;

#endif