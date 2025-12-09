#ifndef CONFIG_H
#define CONFIG_H

// Estrutura para parâmetros do Hill Climbing
typedef struct {
    int max_iteracoes;
    int usar_vizinhanca_2;
    int aceitar_iguais;
} ConfigHillClimbing;

// Estrutura para parâmetros do Algoritmo Evolutivo
typedef struct {
    int tamanho_populacao;
    int num_geracoes;
    double prob_cruzamento;
    double prob_mutacao;
    int tipo_selecao;        // 0=torneio, 1=roleta
    int tipo_cruzamento;     // 0=uniforme, 1=um_ponto
    int tipo_mutacao;        // 0=trocar, 1=embaralhar
    int tamanho_torneio;
} ConfigEvolutivo;

// Estrutura para parâmetros dos Híbridos
typedef struct {
    int hibrido1_iter_hc;    // Iterações HC após evolutivo
    int hibrido1_pop_ea;     // População do EA no híbrido 1
    int hibrido1_ger_ea;     // Gerações do EA no híbrido 1
    
    int hibrido2_iter_hc;    // Iterações HC inicial
    int hibrido2_pop_ea;     // População do EA no híbrido 2
    int hibrido2_ger_ea;     // Gerações do EA no híbrido 2
} ConfigHibridos;

// Estrutura principal de configuração
typedef struct {
    int num_execucoes;
    int seed_aleatoria;      // -1 para usar time(NULL)
    ConfigHillClimbing hc;
    ConfigEvolutivo ea;
    ConfigHibridos hib;
} Configuracao;

// Funções de configuração
Configuracao *carregar_configuracao(const char *nome_ficheiro);
void criar_ficheiro_configuracao_padrao(const char *nome_ficheiro);
void libertar_configuracao(Configuracao *config);
void mostrar_configuracao(Configuracao *config);

#endif