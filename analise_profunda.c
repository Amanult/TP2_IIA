#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include "analise_profunda.h"
#include "hill_climbing.h"
#include "evolutivo.h"
#include "hibridos.h"
#include "utils.h"
#include "xlsxwriter.h"

// Tipos de algoritmos para a fila de tarefas
typedef enum { ALG_HC, ALG_EA, ALG_H1, ALG_H2 } TipoAlgoritmo;

// Estrutura de Tarefa Genérica
typedef struct {
    int id;
    TipoAlgoritmo tipo;
    // Armazenamento genérico de parâmetros para evitar structs complexas
    int p1, p2, p3, p4, p5, p6, p7, p8;
    double d1, d2;
} Tarefa;

// Variáveis Globais de Controle (Thread Safe via Mutex)
Tarefa *fila_tarefas = NULL;
int total_tarefas = 0;
int tarefa_atual = 0;
ConjuntoResultados *resultados_global = NULL;
Problema *problema_global = NULL;
Configuracao *config_global = NULL;

pthread_mutex_t mutex_tarefas;
pthread_mutex_t mutex_resultados;
pthread_mutex_t mutex_rand; // NOVO: Mutex para o RNG

// --- Gestão de Estruturas ---

ConjuntoResultados *criar_conjunto_resultados() {
    ConjuntoResultados *conj = malloc(sizeof(ConjuntoResultados));
    conj->capacidade = 1000;
    conj->num_resultados = 0;
    conj->resultados = malloc(conj->capacidade * sizeof(ResultadoTeste));
    return conj;
}

void adicionar_resultado(ConjuntoResultados *conj, ResultadoTeste resultado) {
    if (conj->num_resultados >= conj->capacidade) {
        conj->capacidade *= 2;
        conj->resultados = realloc(conj->resultados, conj->capacidade * sizeof(ResultadoTeste));
    }
    conj->resultados[conj->num_resultados++] = resultado;
}

void libertar_conjunto_resultados(ConjuntoResultados *conj) {
    if (conj) {
        free(conj->resultados);
        free(conj);
    }
}

// --- Funções Thread-Safe ---

void adicionar_tarefa(Tarefa t) {
    fila_tarefas = realloc(fila_tarefas, (total_tarefas + 1) * sizeof(Tarefa));
    fila_tarefas[total_tarefas] = t;
    total_tarefas++;
}

void adicionar_resultado_safe(ResultadoTeste res) {
    pthread_mutex_lock(&mutex_resultados);
    adicionar_resultado(resultados_global, res);
    if (resultados_global->num_resultados % 5 == 0 || resultados_global->num_resultados == total_tarefas) {
        printf("\r[Progresso] %d/%d configuracoes concluidas (%.1f%%)",
               resultados_global->num_resultados, total_tarefas,
               (float)resultados_global->num_resultados/total_tarefas*100);
        fflush(stdout);
    }
    pthread_mutex_unlock(&mutex_resultados);
}

// --- Processamento dos Algoritmos ---

void processar_hc(Tarefa t) {
    ResultadoTeste res;
    sprintf(res.algoritmo, "Hill Climbing");
    sprintf(res.descricao_params, "iter=%d, viz=%d, aceitar=%d", t.p1, t.p2, t.p3);
    res.config_id = t.id;
    res.parametro1 = t.p1; res.parametro2 = t.p2;
    res.parametro3 = (double)t.p3; res.parametro4 = 0;

    double *valores = malloc(config_global->num_execucoes * sizeof(double));
    clock_t inicio = clock();

    for (int i = 0; i < config_global->num_execucoes; i++) {
        Solucao *sol = hill_climbing(problema_global, t.p1, t.p2, t.p3);
        valores[i] = sol->fitness;
        libertar_solucao(sol);
    }
    clock_t fim = clock();
    res.tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    Estatisticas stats;
    calcular_estatisticas(valores, config_global->num_execucoes, &stats);
    res.melhor = stats.melhor; res.media = stats.media;
    res.pior = stats.pior; res.desvio_padrao = stats.desvio_padrao;

    free(valores);
    adicionar_resultado_safe(res);
}

void processar_ea(Tarefa t) {
    ResultadoTeste res;
    sprintf(res.algoritmo, "Evolutivo");
    sprintf(res.descricao_params, "pop=%d, ger=%d, cruz=%.2f, mut=%.2f", t.p1, t.p2, t.d1, t.d2);
    res.config_id = t.id;
    res.parametro1 = t.p1; res.parametro2 = t.p2;
    res.parametro3 = t.d1; res.parametro4 = t.d2;

    double *valores = malloc(config_global->num_execucoes * sizeof(double));
    clock_t inicio = clock();

    for (int i = 0; i < config_global->num_execucoes; i++) {
        // t.p5=sel, p6=tc, p7=tm, p8=torneio
        Solucao *sol = algoritmo_evolutivo(problema_global, t.p1, t.p2, t.d1, t.d2, t.p5, t.p6, t.p7, t.p8);
        valores[i] = sol->fitness;
        libertar_solucao(sol);
    }
    clock_t fim = clock();
    res.tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    Estatisticas stats;
    calcular_estatisticas(valores, config_global->num_execucoes, &stats);
    res.melhor = stats.melhor; res.media = stats.media;
    res.pior = stats.pior; res.desvio_padrao = stats.desvio_padrao;

    free(valores);
    adicionar_resultado_safe(res);
}

void processar_hibrido(Tarefa t) {
    ResultadoTeste res;
    Configuracao cfg_temp;
    // Defaults seguros para o que não está a ser testado
    cfg_temp.ea.prob_cruzamento = 0.8; cfg_temp.ea.prob_mutacao = 0.1;
    cfg_temp.ea.tipo_selecao = 0; cfg_temp.ea.tipo_cruzamento = 0;
    cfg_temp.ea.tipo_mutacao = 0; cfg_temp.ea.tamanho_torneio = 3;
    cfg_temp.hc.usar_vizinhanca_2 = 0; cfg_temp.hc.aceitar_iguais = 1;

    if(t.tipo == ALG_H1) {
        sprintf(res.algoritmo, "Hibrido 1");
        sprintf(res.descricao_params, "pop=%d, ger=%d, hc=%d", t.p1, t.p2, t.p3);
        cfg_temp.hib.hibrido1_pop_ea = t.p1;
        cfg_temp.hib.hibrido1_ger_ea = t.p2;
        cfg_temp.hib.hibrido1_iter_hc = t.p3;
    } else {
        sprintf(res.algoritmo, "Hibrido 2");
        sprintf(res.descricao_params, "hc=%d, pop=%d, ger=%d", t.p1, t.p2, t.p3);
        cfg_temp.hib.hibrido2_iter_hc = t.p1;
        cfg_temp.hib.hibrido2_pop_ea = t.p2;
        cfg_temp.hib.hibrido2_ger_ea = t.p3;
    }

    res.config_id = t.id;
    res.parametro1 = t.p1; res.parametro2 = t.p2;
    res.parametro3 = (double)t.p3; res.parametro4 = 0;

    double *valores = malloc(config_global->num_execucoes * sizeof(double));
    clock_t inicio = clock();

    for (int i = 0; i < config_global->num_execucoes; i++) {
        Solucao *sol = (t.tipo == ALG_H1) ? hibrido1(problema_global, &cfg_temp) : hibrido2(problema_global, &cfg_temp);
        valores[i] = sol->fitness;
        libertar_solucao(sol);
    }
    clock_t fim = clock();
    res.tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    Estatisticas stats;
    calcular_estatisticas(valores, config_global->num_execucoes, &stats);
    res.melhor = stats.melhor; res.media = stats.media;
    res.pior = stats.pior; res.desvio_padrao = stats.desvio_padrao;

    free(valores);
    adicionar_resultado_safe(res);
}

// --- Worker Thread ---

void *thread_worker(void *arg) {
    int thread_id = arg ? *(int*)arg : -1;
    while (1) {
        Tarefa t;
        int ok = 0;

        // Região Crítica: Buscar tarefa
        pthread_mutex_lock(&mutex_tarefas);
        if (tarefa_atual < total_tarefas) {
            t = fila_tarefas[tarefa_atual++];
            ok = 1;
        }
        pthread_mutex_unlock(&mutex_tarefas);

        if (!ok) break;

        // Log: qual thread pegou qual tarefa
        pthread_mutex_lock(&mutex_resultados);
        const char* tipo_str = (t.tipo==ALG_HC?"HC":(t.tipo==ALG_EA?"EA":(t.tipo==ALG_H1?"H1":"H2")));
        char params_str[200];
        if (t.tipo == ALG_HC) {
            sprintf(params_str, "iter=%d, viz=%d, aceitar=%d", t.p1, t.p2, t.p3);
        } else if (t.tipo == ALG_EA) {
            sprintf(params_str, "pop=%d, ger=%d, cruz=%.2f, mut=%.2f", t.p1, t.p2, t.d1, t.d2);
        } else if (t.tipo == ALG_H1) {
            sprintf(params_str, "pop_ea=%d, ger_ea=%d, iter_hc=%d", t.p1, t.p2, t.p3);
        } else { // ALG_H2
            sprintf(params_str, "iter_hc=%d, pop_ea=%d, ger_ea=%d", t.p1, t.p2, t.p3);
        }
        printf("\n[Thread %d] Iniciou tarefa #%d (%s) com parametros: %s\n", thread_id, t.id, tipo_str, params_str);
        fflush(stdout);
        pthread_mutex_unlock(&mutex_resultados);

        // Execução (Sem bloqueio)
        if (t.tipo == ALG_HC) processar_hc(t);
        else if (t.tipo == ALG_EA) processar_ea(t);
        else processar_hibrido(t);

        // Log de conclusão
        pthread_mutex_lock(&mutex_resultados);
        printf("[Thread %d] Concluiu tarefa #%d\n", thread_id, t.id);
        fflush(stdout);
        pthread_mutex_unlock(&mutex_resultados);
    }
    return NULL;
}

// --- Geração de Tarefas ---

void gerar_tarefas() {
    int id = 0;

    // 1. Hill Climbing
    int hc_iter[] = {1000, 5000, 10000, 20000};
    int hc_viz[] = {0, 1};
    int hc_ac[] = {0, 1};
    for(int i=0; i<4; i++) for(int v=0; v<2; v++) for(int a=0; a<2; a++) {
        Tarefa t = {++id, ALG_HC, hc_iter[i], hc_viz[v], hc_ac[a]};
        adicionar_tarefa(t);
    }

    // 2. Evolutivo (Amostragem para não demorar horas)
    int ea_pop[] = {20, 50, 100};
    int ea_ger[] = {100, 200, 500};
    double ea_cruz[] = {0.6, 0.8, 0.9};
    double ea_mut[] = {0.05, 0.1, 0.2};
    // Fixos: sel=0(torneio), tc=0(uniforme), tm=0(trocar), tt=3
    for(int p=0; p<3; p++) for(int g=0; g<3; g++) for(int c=0; c<3; c++) for(int m=0; m<3; m++) {
        if((p+g+c+m) % 2 != 0) continue; // Pula metade para ser mais rápido
        Tarefa t = {++id, ALG_EA, ea_pop[p], ea_ger[g], 0, 0, 0, 0, 0, 3};
        t.d1 = ea_cruz[c]; t.d2 = ea_mut[m];
        adicionar_tarefa(t);
    }

    // 3. Hibrido 1
    int h1_pop[] = {30, 50, 100};
    int h1_ger[] = {50, 100, 200};
    int h1_iter_hc[] = {200, 500, 1000};
    for(int p=0; p<3; p++) for(int g=0; g<3; g++) for(int h=0; h<3; h++) {
        Tarefa t = {++id, ALG_H1, h1_pop[p], h1_ger[g], h1_iter_hc[h]};
        adicionar_tarefa(t);
    }

    // 4. Hibrido 2
    int h2_iter_hc[] = {500, 1000, 2000};
    int h2_pop[] = {20, 30, 50};
    int h2_ger[] = {30, 50, 100};
    for(int h=0; h<3; h++) for(int p=0; p<3; p++) for(int g=0; g<3; g++) {
        Tarefa t = {++id, ALG_H2, h2_iter_hc[h], h2_pop[p], h2_ger[g]};
        adicionar_tarefa(t);
    }
}

// --- Excel e Display ---

void escrever_analise_excel(const char *ficheiro, ConjuntoResultados *resultados) {
    lxw_workbook *livro = workbook_new(ficheiro);
    if (!livro) { printf("Erro ao criar Excel!\n"); return; }

    // Criar primeiro as folhas por algoritmo
    lxw_worksheet *ws_hc = workbook_add_worksheet(livro, "Hill Climbing");
    lxw_worksheet *ws_ea = workbook_add_worksheet(livro, "Evolutivo");
    lxw_worksheet *ws_h1 = workbook_add_worksheet(livro, "Hibrido 1");
    lxw_worksheet *ws_h2 = workbook_add_worksheet(livro, "Hibrido 2");

    if (!ws_hc || !ws_ea || !ws_h1 || !ws_h2) {
        printf("Erro ao criar folhas por algoritmo!\n");
        workbook_close(livro);
        return;
    }

    lxw_format *bold = workbook_add_format(livro);
    format_set_bold(bold);

    // Cabeçalhos comuns
    const char *headers[] = {"Config ID", "Melhor", "Media", "Pior", "Desvio", "Tempo(s)", "Parametros"};
    for (int c = 0; c < 7; c++) {
        worksheet_write_string(ws_hc, 0, c, headers[c], bold);
        worksheet_write_string(ws_ea, 0, c, headers[c], bold);
        worksheet_write_string(ws_h1, 0, c, headers[c], bold);
        worksheet_write_string(ws_h2, 0, c, headers[c], bold);
    }

    // Linhas atuais por folha
    int linha_hc = 1, linha_ea = 1, linha_h1 = 1, linha_h2 = 1;

    // Rastreamento dos melhores por algoritmo
    double best_hc = -INFINITY, best_ea = -INFINITY, best_h1 = -INFINITY, best_h2 = -INFINITY;
    int best_id_hc = -1, best_id_ea = -1, best_id_h1 = -1, best_id_h2 = -1;
    const char *best_desc_hc = NULL, *best_desc_ea = NULL, *best_desc_h1 = NULL, *best_desc_h2 = NULL;

    // Distribuir resultados pelas folhas e calcular melhores
    for (int i = 0; i < resultados->num_resultados; i++) {
        ResultadoTeste *r = &resultados->resultados[i];
        lxw_worksheet *dest = NULL;
        int *linha_ptr = NULL;

        if (strcmp(r->algoritmo, "Hill Climbing") == 0) {
            dest = ws_hc; linha_ptr = &linha_hc;
            if (r->melhor > best_hc) { best_hc = r->melhor; best_id_hc = r->config_id; best_desc_hc = r->descricao_params; }
        } else if (strcmp(r->algoritmo, "Evolutivo") == 0) {
            dest = ws_ea; linha_ptr = &linha_ea;
            if (r->melhor > best_ea) { best_ea = r->melhor; best_id_ea = r->config_id; best_desc_ea = r->descricao_params; }
        } else if (strcmp(r->algoritmo, "Hibrido 1") == 0) {
            dest = ws_h1; linha_ptr = &linha_h1;
            if (r->melhor > best_h1) { best_h1 = r->melhor; best_id_h1 = r->config_id; best_desc_h1 = r->descricao_params; }
        } else if (strcmp(r->algoritmo, "Hibrido 2") == 0) {
            dest = ws_h2; linha_ptr = &linha_h2;
            if (r->melhor > best_h2) { best_h2 = r->melhor; best_id_h2 = r->config_id; best_desc_h2 = r->descricao_params; }
        } else {
            // Algoritmo desconhecido, ignorar
            continue;
        }

        int linha = (*linha_ptr);
        worksheet_write_number(dest, linha, 0, r->config_id, NULL);
        worksheet_write_number(dest, linha, 1, r->melhor, NULL);
        worksheet_write_number(dest, linha, 2, r->media, NULL);
        worksheet_write_number(dest, linha, 3, r->pior, NULL);
        worksheet_write_number(dest, linha, 4, r->desvio_padrao, NULL);
        worksheet_write_number(dest, linha, 5, r->tempo_execucao, NULL);
        worksheet_write_string(dest, linha, 6, r->descricao_params, NULL);
        (*linha_ptr)++;
    }

    // Criar a folha de resumo por último, para que fique no fim
    lxw_worksheet *ws_resumo = workbook_add_worksheet(livro, "Resumo");
    if (ws_resumo) {
        worksheet_write_string(ws_resumo, 0, 0, "Algoritmo", bold);
        worksheet_write_string(ws_resumo, 0, 1, "Melhor Fitness", bold);
        worksheet_write_string(ws_resumo, 0, 2, "Melhor Configuracao", bold);
        worksheet_write_string(ws_resumo, 0, 3, "Melhor Config ID", bold);

        int l = 1;
        worksheet_write_string(ws_resumo, l, 0, "Hill Climbing", NULL);
        worksheet_write_number(ws_resumo, l, 1, best_hc, NULL);
        worksheet_write_string(ws_resumo, l, 2, best_desc_hc ? best_desc_hc : "", NULL);
        worksheet_write_number(ws_resumo, l, 3, best_id_hc, NULL);
        l++;

        worksheet_write_string(ws_resumo, l, 0, "Evolutivo", NULL);
        worksheet_write_number(ws_resumo, l, 1, best_ea, NULL);
        worksheet_write_string(ws_resumo, l, 2, best_desc_ea ? best_desc_ea : "", NULL);
        worksheet_write_number(ws_resumo, l, 3, best_id_ea, NULL);
        l++;

        worksheet_write_string(ws_resumo, l, 0, "Hibrido 1", NULL);
        worksheet_write_number(ws_resumo, l, 1, best_h1, NULL);
        worksheet_write_string(ws_resumo, l, 2, best_desc_h1 ? best_desc_h1 : "", NULL);
        worksheet_write_number(ws_resumo, l, 3, best_id_h1, NULL);
        l++;

        worksheet_write_string(ws_resumo, l, 0, "Hibrido 2", NULL);
        worksheet_write_number(ws_resumo, l, 1, best_h2, NULL);
        worksheet_write_string(ws_resumo, l, 2, best_desc_h2 ? best_desc_h2 : "", NULL);
        worksheet_write_number(ws_resumo, l, 3, best_id_h2, NULL);
        l++;
    } else {
        printf("Aviso: nao foi possivel criar a folha 'Resumo' no Excel de analise.\n");
    }

    workbook_close(livro);
    printf("\nResultados guardados em: %s\n", ficheiro);
}

void mostrar_melhores_configuracoes(ConjuntoResultados *resultados) {
    printf("\n=== MELHORES CONFIGURACOES ===\n");
    // Lógica simples: encontrar o melhor absoluto de cada tipo
    const char *tipos[] = {"Hill Climbing", "Evolutivo", "Hibrido 1", "Hibrido 2"};

    for(int t=0; t<4; t++) {
        int idx = -1;
        double melhor_fit = -1.0;

        for(int i=0; i<resultados->num_resultados; i++) {
            if(strcmp(resultados->resultados[i].algoritmo, tipos[t]) == 0) {
                if(resultados->resultados[i].melhor > melhor_fit) {
                    melhor_fit = resultados->resultados[i].melhor;
                    idx = i;
                }
            }
        }

        if(idx != -1) {
            // Linha corrigida para usar 'resultados' (o argumento da função)
            printf("[%s] Fitness: %.2f | Params: %s\n",
                   tipos[t], melhor_fit, resultados->resultados[idx].descricao_params);
        }
    }
}

// --- Função Principal ---

void executar_analise_profunda(Problema *prob, Configuracao *config, const char *ficheiro_saida) {
    printf("\n");
    printf("========================================\n");
    printf("  ANALISE PROFUNDA MULTITHREADED\n");
    printf("========================================\n");
    printf("Ficheiro Saida: %s\n", ficheiro_saida);
    printf("Threads: %d\n", config->n_threads);
    printf("Execucoes por config: %d\n", config->num_execucoes);
    printf("========================================\n");

    // Inicialização
    problema_global = prob;
    config_global = config;
    resultados_global = criar_conjunto_resultados();
    pthread_mutex_init(&mutex_tarefas, NULL);
    pthread_mutex_init(&mutex_resultados, NULL);
    pthread_mutex_init(&mutex_rand, NULL); // NOVO: Inicializar mutex rand

    // 1. Gerar Tarefas
    printf("A gerar tarefas...\n");
    gerar_tarefas();
    printf("Total de configuracoes a testar: %d\n", total_tarefas);

    // 2. Criar e Executar Threads
    pthread_t *threads = malloc(config->n_threads * sizeof(pthread_t));
    int *thread_ids = malloc(config->n_threads * sizeof(int));
    clock_t inicio_total = clock();

    printf("A iniciar %d threads...\n", config->n_threads);
    for (int i = 0; i < config->n_threads; i++) {
        thread_ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, thread_worker, &thread_ids[i]) != 0) {
            fprintf(stderr, "Erro critico ao criar thread %d\n", i);
            exit(1);
        }
    }

    // 3. Aguardar Conclusão
    for (int i = 0; i < config->n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t fim_total = clock();
    double tempo_total = ((double)(fim_total - inicio_total)) / CLOCKS_PER_SEC;

    printf("\n\n========================================\n");
    printf("  ANALISE CONCLUIDA\n");
    printf("========================================\n");
    printf("Tempo total (CPU aprox): %.2fs\n", tempo_total);

    // 4. Finalização
    mostrar_melhores_configuracoes(resultados_global);
    escrever_analise_excel(ficheiro_saida, resultados_global);

    // Limpeza
    free(threads);
    free(thread_ids);
    if (fila_tarefas) free(fila_tarefas);
    libertar_conjunto_resultados(resultados_global);
    pthread_mutex_destroy(&mutex_tarefas);
    pthread_mutex_destroy(&mutex_resultados);
    pthread_mutex_destroy(&mutex_rand); // NOVO: Destruir mutex rand
}