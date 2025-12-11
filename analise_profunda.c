#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "analise_profunda.h"
#include "hill_climbing.h"
#include "evolutivo.h"
#include "hibridos.h"
#include "utils.h"
#include "xlsxwriter.h"

#define NUM_TESTES_POR_CONFIG 10

// Criar conjunto de resultados
ConjuntoResultados *criar_conjunto_resultados() {
    ConjuntoResultados *conj = malloc(sizeof(ConjuntoResultados));
    conj->capacidade = 1000;
    conj->num_resultados = 0;
    conj->resultados = malloc(conj->capacidade * sizeof(ResultadoTeste));
    return conj;
}

// Adicionar resultado
void adicionar_resultado(ConjuntoResultados *conj, ResultadoTeste resultado) {
    if (conj->num_resultados >= conj->capacidade) {
        conj->capacidade *= 2;
        conj->resultados = realloc(conj->resultados, 
                                   conj->capacidade * sizeof(ResultadoTeste));
    }
    conj->resultados[conj->num_resultados++] = resultado;
}

// Libertar conjunto
void libertar_conjunto_resultados(ConjuntoResultados *conj) {
    if (conj) {
        free(conj->resultados);
        free(conj);
    }
}

// Testar uma configuração específica
ResultadoTeste testar_configuracao_hc(Problema *prob, int iteracoes, 
                                      int vizinhanca, int aceitar_iguais) {
    ResultadoTeste res;
    sprintf(res.algoritmo, "Hill Climbing");
    sprintf(res.descricao_params, "iter=%d, viz=%d, aceitar=%d", 
            iteracoes, vizinhanca, aceitar_iguais);
    
    res.parametro1 = iteracoes;
    res.parametro2 = vizinhanca;
    res.parametro3 = (double)aceitar_iguais;
    res.parametro4 = 0.0;
    
    double valores[NUM_TESTES_POR_CONFIG];
    clock_t inicio = clock();
    
    for (int i = 0; i < NUM_TESTES_POR_CONFIG; i++) {
        Solucao *sol = hill_climbing(prob, iteracoes, vizinhanca, aceitar_iguais);
        valores[i] = sol->fitness;
        libertar_solucao(sol);
    }
    
    clock_t fim = clock();
    res.tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    
    // Calcular estatísticas
    res.melhor = valores[0];
    res.pior = valores[0];
    res.media = 0.0;
    
    for (int i = 0; i < NUM_TESTES_POR_CONFIG; i++) {
        if (valores[i] > res.melhor) res.melhor = valores[i];
        if (valores[i] < res.pior) res.pior = valores[i];
        res.media += valores[i];
    }
    res.media /= NUM_TESTES_POR_CONFIG;
    
    double variancia = 0.0;
    for (int i = 0; i < NUM_TESTES_POR_CONFIG; i++) {
        double diff = valores[i] - res.media;
        variancia += diff * diff;
    }
    res.desvio_padrao = sqrt(variancia / NUM_TESTES_POR_CONFIG);
    
    return res;
}

// Análise Hill Climbing
void executar_analise_hill_climbing(Problema *prob, ConjuntoResultados *resultados) {
    printf("\n=== ANALISE HILL CLIMBING ===\n");
    
    int iteracoes_valores[] = {1000, 2000, 5000, 10000, 20000};
    int vizinhanca_valores[] = {0, 1};
    int aceitar_valores[] = {0, 1};
    
    int total = sizeof(iteracoes_valores)/sizeof(int) * 
                sizeof(vizinhanca_valores)/sizeof(int) * 
                sizeof(aceitar_valores)/sizeof(int);
    int atual = 0;
    
    for (int i = 0; i < sizeof(iteracoes_valores)/sizeof(int); i++) {
        for (int v = 0; v < sizeof(vizinhanca_valores)/sizeof(int); v++) {
            for (int a = 0; a < sizeof(aceitar_valores)/sizeof(int); a++) {
                atual++;
                printf("Configuracao HC %d/%d: iter=%d, viz=%d, aceitar=%d\n", 
                       atual, total,
                       iteracoes_valores[i], 
                       vizinhanca_valores[v], 
                       aceitar_valores[a]);
                
                ResultadoTeste res = testar_configuracao_hc(
                    prob,
                    iteracoes_valores[i],
                    vizinhanca_valores[v],
                    aceitar_valores[a]
                );
                
                res.config_id = atual;
                adicionar_resultado(resultados, res);
                
                printf("  Melhor: %.2f, Media: %.2f, Tempo: %.2fs\n", 
                       res.melhor, res.media, res.tempo_execucao);
            }
        }
    }
}

// Testar configuração Evolutivo
ResultadoTeste testar_configuracao_ea(Problema *prob, int pop, int ger,
                                      double prob_cruz, double prob_mut,
                                      int tipo_sel, int tipo_cruz, 
                                      int tipo_mut, int tam_torneio) {
    ResultadoTeste res;
    sprintf(res.algoritmo, "Evolutivo");
    sprintf(res.descricao_params, 
            "pop=%d, ger=%d, cruz=%.2f, mut=%.2f, sel=%d, tc=%d, tm=%d, tt=%d", 
            pop, ger, prob_cruz, prob_mut, tipo_sel, tipo_cruz, tipo_mut, tam_torneio);
    
    res.parametro1 = pop;
    res.parametro2 = ger;
    res.parametro3 = prob_cruz;
    res.parametro4 = prob_mut;
    
    double valores[NUM_TESTES_POR_CONFIG];
    clock_t inicio = clock();
    
    for (int i = 0; i < NUM_TESTES_POR_CONFIG; i++) {
        Solucao *sol = algoritmo_evolutivo(prob, pop, ger, prob_cruz, prob_mut,
                                           tipo_sel, tipo_cruz, tipo_mut, tam_torneio);
        valores[i] = sol->fitness;
        libertar_solucao(sol);
    }
    
    clock_t fim = clock();
    res.tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    
    // Calcular estatísticas
    res.melhor = valores[0];
    res.pior = valores[0];
    res.media = 0.0;
    
    for (int i = 0; i < NUM_TESTES_POR_CONFIG; i++) {
        if (valores[i] > res.melhor) res.melhor = valores[i];
        if (valores[i] < res.pior) res.pior = valores[i];
        res.media += valores[i];
    }
    res.media /= NUM_TESTES_POR_CONFIG;
    
    double variancia = 0.0;
    for (int i = 0; i < NUM_TESTES_POR_CONFIG; i++) {
        double diff = valores[i] - res.media;
        variancia += diff * diff;
    }
    res.desvio_padrao = sqrt(variancia / NUM_TESTES_POR_CONFIG);
    
    return res;
}

// Análise Evolutivo
void executar_analise_evolutivo(Problema *prob, ConjuntoResultados *resultados) {
    printf("\n=== ANALISE EVOLUTIVO ===\n");
    
    int pop_valores[] = {20, 50, 100};
    int ger_valores[] = {50, 100, 200, 500};
    double cruz_valores[] = {0.6, 0.8, 0.9};
    double mut_valores[] = {0.05, 0.1, 0.2};
    int sel_valores[] = {0, 1};  // 0=torneio, 1=roleta
    int cruz_tipo_valores[] = {0, 1};  // 0=uniforme, 1=um_ponto
    int mut_tipo_valores[] = {0, 1};  // 0=trocar, 1=embaralhar
    
    int config_id = 0;
    
    // Testar combinações principais
    for (int p = 0; p < sizeof(pop_valores)/sizeof(int); p++) {
        for (int g = 0; g < sizeof(ger_valores)/sizeof(int); g++) {
            for (int c = 0; c < sizeof(cruz_valores)/sizeof(double); c++) {
                for (int m = 0; m < sizeof(mut_valores)/sizeof(double); m++) {
                    for (int s = 0; s < sizeof(sel_valores)/sizeof(int); s++) {
                        for (int tc = 0; tc < sizeof(cruz_tipo_valores)/sizeof(int); tc++) {
                            for (int tm = 0; tm < sizeof(mut_tipo_valores)/sizeof(int); tm++) {
                                config_id++;
                                
                                // Limitar número de testes (seria muito!)
                                // Testar apenas combinações interessantes
                                if (config_id % 4 != 0) continue;  // Testar 1 em cada 4
                                
                                int tam_torneio = 3;  // Fixo por simplicidade
                                
                                printf("Configuracao EA %d: pop=%d, ger=%d, cruz=%.2f, mut=%.2f\n",
                                       config_id, pop_valores[p], ger_valores[g], 
                                       cruz_valores[c], mut_valores[m]);
                                
                                ResultadoTeste res = testar_configuracao_ea(
                                    prob, 
                                    pop_valores[p], 
                                    ger_valores[g],
                                    cruz_valores[c],
                                    mut_valores[m],
                                    sel_valores[s],
                                    cruz_tipo_valores[tc],
                                    mut_tipo_valores[tm],
                                    tam_torneio
                                );
                                
                                res.config_id = config_id;
                                adicionar_resultado(resultados, res);
                                
                                printf("  Melhor: %.2f, Media: %.2f, Tempo: %.2fs\n",
                                       res.melhor, res.media, res.tempo_execucao);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Testar configuração Híbrido
ResultadoTeste testar_configuracao_hibrido(Problema *prob, int tipo_hibrido,
                                          int param1, int param2, int param3) {
    ResultadoTeste res;
    
    if (tipo_hibrido == 1) {
        sprintf(res.algoritmo, "Hibrido 1");
        sprintf(res.descricao_params, "pop_ea=%d, ger_ea=%d, iter_hc=%d",
                param1, param2, param3);
    } else {
        sprintf(res.algoritmo, "Hibrido 2");
        sprintf(res.descricao_params, "iter_hc=%d, pop_ea=%d, ger_ea=%d",
                param1, param2, param3);
    }
    
    res.parametro1 = param1;
    res.parametro2 = param2;
    res.parametro3 = (double)param3;
    res.parametro4 = 0.0;
    
    // Criar configuração temporária
    Configuracao config;
    config.ea.prob_cruzamento = 0.8;
    config.ea.prob_mutacao = 0.1;
    config.ea.tipo_selecao = 0;
    config.ea.tipo_cruzamento = 0;
    config.ea.tipo_mutacao = 0;
    config.ea.tamanho_torneio = 3;
    config.hc.usar_vizinhanca_2 = 0;
    config.hc.aceitar_iguais = 1;
    
    if (tipo_hibrido == 1) {
        config.hib.hibrido1_pop_ea = param1;
        config.hib.hibrido1_ger_ea = param2;
        config.hib.hibrido1_iter_hc = param3;
    } else {
        config.hib.hibrido2_iter_hc = param1;
        config.hib.hibrido2_pop_ea = param2;
        config.hib.hibrido2_ger_ea = param3;
    }
    
    double valores[NUM_TESTES_POR_CONFIG];
    clock_t inicio = clock();
    
    for (int i = 0; i < NUM_TESTES_POR_CONFIG; i++) {
        Solucao *sol;
        if (tipo_hibrido == 1) {
            sol = hibrido1(prob, &config);
        } else {
            sol = hibrido2(prob, &config);
        }
        valores[i] = sol->fitness;
        libertar_solucao(sol);
    }
    
    clock_t fim = clock();
    res.tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    
    // Calcular estatísticas
    res.melhor = valores[0];
    res.pior = valores[0];
    res.media = 0.0;
    
    for (int i = 0; i < NUM_TESTES_POR_CONFIG; i++) {
        if (valores[i] > res.melhor) res.melhor = valores[i];
        if (valores[i] < res.pior) res.pior = valores[i];
        res.media += valores[i];
    }
    res.media /= NUM_TESTES_POR_CONFIG;
    
    double variancia = 0.0;
    for (int i = 0; i < NUM_TESTES_POR_CONFIG; i++) {
        double diff = valores[i] - res.media;
        variancia += diff * diff;
    }
    res.desvio_padrao = sqrt(variancia / NUM_TESTES_POR_CONFIG);
    
    return res;
}

// Análise Híbridos
void executar_analise_hibridos(Problema *prob, ConjuntoResultados *resultados) {
    printf("\n=== ANALISE HIBRIDOS ===\n");
    
    // Híbrido 1: Evolutivo + HC
    int h1_pop_valores[] = {30, 50, 100};
    int h1_ger_valores[] = {50, 100, 200};
    int h1_iter_valores[] = {200, 500, 1000};
    
    int config_id = 0;
    
    printf("\n--- Hibrido 1 (Evolutivo + HC) ---\n");
    for (int p = 0; p < sizeof(h1_pop_valores)/sizeof(int); p++) {
        for (int g = 0; g < sizeof(h1_ger_valores)/sizeof(int); g++) {
            for (int i = 0; i < sizeof(h1_iter_valores)/sizeof(int); i++) {
                config_id++;
                printf("Configuracao H1 %d: pop=%d, ger=%d, iter_hc=%d\n",
                       config_id, h1_pop_valores[p], h1_ger_valores[g], h1_iter_valores[i]);
                
                ResultadoTeste res = testar_configuracao_hibrido(
                    prob, 1,
                    h1_pop_valores[p],
                    h1_ger_valores[g],
                    h1_iter_valores[i]
                );
                
                res.config_id = config_id;
                adicionar_resultado(resultados, res);
                
                printf("  Melhor: %.2f, Media: %.2f, Tempo: %.2fs\n",
                       res.melhor, res.media, res.tempo_execucao);
            }
        }
    }
    
    // Híbrido 2: HC + Evolutivo
    int h2_iter_valores[] = {500, 1000, 2000};
    int h2_pop_valores[] = {20, 30, 50};
    int h2_ger_valores[] = {30, 50, 100};
    
    config_id = 0;
    
    printf("\n--- Hibrido 2 (HC + Evolutivo) ---\n");
    for (int i = 0; i < sizeof(h2_iter_valores)/sizeof(int); i++) {
        for (int p = 0; p < sizeof(h2_pop_valores)/sizeof(int); p++) {
            for (int g = 0; g < sizeof(h2_ger_valores)/sizeof(int); g++) {
                config_id++;
                printf("Configuracao H2 %d: iter_hc=%d, pop=%d, ger=%d\n",
                       config_id, h2_iter_valores[i], h2_pop_valores[p], h2_ger_valores[g]);
                
                ResultadoTeste res = testar_configuracao_hibrido(
                    prob, 2,
                    h2_iter_valores[i],
                    h2_pop_valores[p],
                    h2_ger_valores[g]
                );
                
                res.config_id = config_id;
                adicionar_resultado(resultados, res);
                
                printf("  Melhor: %.2f, Media: %.2f, Tempo: %.2fs\n",
                       res.melhor, res.media, res.tempo_execucao);
            }
        }
    }
}

// Escrever resultados em Excel
void escrever_analise_excel(const char *ficheiro, ConjuntoResultados *resultados) {
    lxw_workbook *livro = workbook_new(ficheiro);
    if (!livro) {
        fprintf(stderr, "Erro ao criar ficheiro Excel\n");
        return;
    }
    
    // Criar folhas por algoritmo
    lxw_worksheet *folha_hc = workbook_add_worksheet(livro, "Hill Climbing");
    lxw_worksheet *folha_ea = workbook_add_worksheet(livro, "Evolutivo");
    lxw_worksheet *folha_h1 = workbook_add_worksheet(livro, "Hibrido 1");
    lxw_worksheet *folha_h2 = workbook_add_worksheet(livro, "Hibrido 2");
    lxw_worksheet *folha_resumo = workbook_add_worksheet(livro, "Resumo");
    
    // Formatos
    lxw_format *fmt_cabecalho = workbook_add_format(livro);
    format_set_bold(fmt_cabecalho);
    format_set_bg_color(fmt_cabecalho, 0xCCCCCC);
    
    lxw_format *fmt_melhor = workbook_add_format(livro);
    format_set_bold(fmt_melhor);
    format_set_bg_color(fmt_melhor, 0x90EE90);
    
    // Cabeçalhos
    const char *headers[] = {
        "Config ID", "Parametros", "Melhor", "Media", "Pior", 
        "Desvio Padrao", "Tempo (s)"
    };
    
    lxw_worksheet *folhas[] = {folha_hc, folha_ea, folha_h1, folha_h2};
    const char *nomes_alg[] = {"Hill Climbing", "Evolutivo", "Hibrido 1", "Hibrido 2"};
    
    for (int f = 0; f < 4; f++) {
        for (int col = 0; col < 7; col++) {
            worksheet_write_string(folhas[f], 0, col, headers[col], fmt_cabecalho);
        }
        worksheet_set_column(folhas[f], 1, 1, 60, NULL);  // Coluna parâmetros larga
    }
    
    // Escrever dados
    int linhas[4] = {1, 1, 1, 1};
    double melhor_global[4] = {-1, -1, -1, -1};
    int linha_melhor[4] = {-1, -1, -1, -1};
    
    for (int i = 0; i < resultados->num_resultados; i++) {
        ResultadoTeste *res = &resultados->resultados[i];
        
        int idx_folha = -1;
        if (strcmp(res->algoritmo, "Hill Climbing") == 0) idx_folha = 0;
        else if (strcmp(res->algoritmo, "Evolutivo") == 0) idx_folha = 1;
        else if (strcmp(res->algoritmo, "Hibrido 1") == 0) idx_folha = 2;
        else if (strcmp(res->algoritmo, "Hibrido 2") == 0) idx_folha = 3;
        
        if (idx_folha == -1) continue;
        
        int linha = linhas[idx_folha]++;
        lxw_worksheet *folha = folhas[idx_folha];
        
        worksheet_write_number(folha, linha, 0, res->config_id, NULL);
        worksheet_write_string(folha, linha, 1, res->descricao_params, NULL);
        worksheet_write_number(folha, linha, 2, res->melhor, NULL);
        worksheet_write_number(folha, linha, 3, res->media, NULL);
        worksheet_write_number(folha, linha, 4, res->pior, NULL);
        worksheet_write_number(folha, linha, 5, res->desvio_padrao, NULL);
        worksheet_write_number(folha, linha, 6, res->tempo_execucao, NULL);
        
        // Rastrear melhor
        if (res->melhor > melhor_global[idx_folha]) {
            melhor_global[idx_folha] = res->melhor;
            linha_melhor[idx_folha] = linha;
        }
    }
    
    // Destacar melhores
    for (int f = 0; f < 4; f++) {
        if (linha_melhor[f] != -1) {
            for (int col = 0; col < 7; col++) {
                // Re-escrever com formato destacado
                lxw_worksheet *folha = folhas[f];
                int linha = linha_melhor[f];
                
                // Ler valor atual e re-escrever com formato
                if (col == 0) {
                    worksheet_write_number(folha, linha, col, 
                                         resultados->resultados[linha-1].config_id, 
                                         fmt_melhor);
                } else if (col == 1) {
                    worksheet_write_string(folha, linha, col,
                                         resultados->resultados[linha-1].descricao_params,
                                         fmt_melhor);
                }
            }
        }
    }
    
    // Folha Resumo
    worksheet_write_string(folha_resumo, 0, 0, "Algoritmo", fmt_cabecalho);
    worksheet_write_string(folha_resumo, 0, 1, "Melhor Fitness", fmt_cabecalho);
    worksheet_write_string(folha_resumo, 0, 2, "Configuracao", fmt_cabecalho);
    worksheet_set_column(folha_resumo, 2, 2, 60, NULL);
    
    for (int f = 0; f < 4; f++) {
        worksheet_write_string(folha_resumo, f + 1, 0, nomes_alg[f], NULL);
        worksheet_write_number(folha_resumo, f + 1, 1, melhor_global[f], NULL);
        
        // Encontrar configuração correspondente
        for (int i = 0; i < resultados->num_resultados; i++) {
            if (strcmp(resultados->resultados[i].algoritmo, nomes_alg[f]) == 0 &&
                resultados->resultados[i].melhor == melhor_global[f]) {
                worksheet_write_string(folha_resumo, f + 1, 2, 
                                     resultados->resultados[i].descricao_params, NULL);
                break;
            }
        }
    }
    
    workbook_close(livro);
    printf("\n=== Analise salva em %s ===\n", ficheiro);
}

// Mostrar melhores configurações
void mostrar_melhores_configuracoes(ConjuntoResultados *resultados) {
    printf("\n========================================\n");
    printf("  MELHORES CONFIGURACOES ENCONTRADAS\n");
    printf("========================================\n");
    
    const char *algoritmos[] = {"Hill Climbing", "Evolutivo", "Hibrido 1", "Hibrido 2"};
    
    for (int a = 0; a < 4; a++) {
        double melhor_fitness = -1;
        int melhor_idx = -1;
        
        for (int i = 0; i < resultados->num_resultados; i++) {
            if (strcmp(resultados->resultados[i].algoritmo, algoritmos[a]) == 0) {
                if (resultados->resultados[i].melhor > melhor_fitness) {
                    melhor_fitness = resultados->resultados[i].melhor;
                    melhor_idx = i;
                }
            }
        }
        
        if (melhor_idx != -1) {
            ResultadoTeste *res = &resultados->resultados[melhor_idx];
            printf("\n%s:\n", algoritmos[a]);
            printf("  Fitness: %.2f\n", res->melhor);
            printf("  Media: %.2f\n", res->media);
            printf("  Parametros: %s\n", res->descricao_params);
            printf("  Tempo: %.2f segundos\n", res->tempo_execucao);
        }
    }
    
    printf("\n========================================\n");
}

// Executar análise completa
void executar_analise_profunda(Problema *prob, const char *ficheiro_saida) {
    printf("\n");
    printf("========================================\n");
    printf("  INICIANDO ANALISE PROFUNDA\n");
    printf("========================================\n");
    printf("Ficheiro de saida: %s\n", ficheiro_saida);
    printf("Testes por configuracao: %d\n", NUM_TESTES_POR_CONFIG);
    printf("\n");
    printf("AVISO: Esta analise pode demorar varios minutos!\n");
    printf("========================================\n");
    
    ConjuntoResultados *resultados = criar_conjunto_resultados();
    
    clock_t inicio_total = clock();
    
    // Executar análises
    executar_analise_hill_climbing(prob, resultados);
    executar_analise_evolutivo(prob, resultados);
    executar_analise_hibridos(prob, resultados);
    
    clock_t fim_total = clock();
    double tempo_total = ((double)(fim_total - inicio_total)) / CLOCKS_PER_SEC;
    
    printf("\n========================================\n");
    printf("  ANALISE CONCLUIDA\n");
    printf("========================================\n");
    printf("Total de configuracoes testadas: %d\n", resultados->num_resultados);
    printf("Tempo total: %.2f segundos (%.2f minutos)\n", 
           tempo_total, tempo_total / 60.0);
    printf("========================================\n");
    
    // Mostrar melhores
    mostrar_melhores_configuracoes(resultados);
    
    // Escrever Excel
    escrever_analise_excel(ficheiro_saida, resultados);
    
    // Libertar memória
    libertar_conjunto_resultados(resultados);
}