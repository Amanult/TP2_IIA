#ifndef CONFIG_H
#define CONFIG_H

// Estruturas de configuração
typedef struct {
    int max_iteracoes;
    int usar_vizinhanca_2;
    int aceitar_iguais;
} ConfigHC;

typedef struct {
    int tamanho_populacao;
    int num_geracoes;
    double prob_cruzamento;
    double prob_mutacao;
    int tipo_selecao;
    int tipo_cruzamento;
    int tipo_mutacao;
    int tamanho_torneio;
} ConfigEA;

typedef struct {
    int hibrido1_iter_hc;
    int hibrido1_pop_ea;
    int hibrido1_ger_ea;
    int hibrido2_iter_hc;
    int hibrido2_pop_ea;
    int hibrido2_ger_ea;
} ConfigHibrido;

typedef struct {
    // Parâmetros Gerais
    int num_execucoes;    // Usado no modo normal e agora na análise profunda
    int seed_aleatoria;
    int n_threads;        // NOVO: Número de threads para análise

    ConfigHC hc;
    ConfigEA ea;
    ConfigHibrido hib;
} Configuracao;

Configuracao *carregar_configuracao(const char *nome_ficheiro);
void criar_ficheiro_configuracao_padrao(const char *nome_ficheiro);
void libertar_configuracao(Configuracao *config);
void mostrar_configuracao(Configuracao *config);

#endif