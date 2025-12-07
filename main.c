#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "estruturas.h"
#include "utils.h"
#include "excel.h"

int main(int argc, char *argv[]) {
    char ficheiro_entrada[256];

    if (argc < 2) {
        printf("Nenhum ficheiro especificado.\n");
        printf("Por favor introduza o nome do ficheiro de entrada: ");
        if (!fgets(ficheiro_entrada, sizeof(ficheiro_entrada), stdin)) {
            fprintf(stderr, "Erro a ler do stdin\n");
            return 1;
        }
        size_t len = strlen(ficheiro_entrada);
        if (len > 0 && ficheiro_entrada[len - 1] == '\n') ficheiro_entrada[len - 1] = '\0';
    } else {
        strncpy(ficheiro_entrada, argv[1], sizeof(ficheiro_entrada) - 1);
        ficheiro_entrada[sizeof(ficheiro_entrada) - 1] = '\0';
    }

    srand(time(NULL));

    Problema prob;
    prob.nomes = NULL;
    prob.distancias = NULL;
    if (!ler_problema(ficheiro_entrada, &prob)) {
        printf("Erro ao ler ficheiro %s\n", ficheiro_entrada);
        return 1;
    }

    printf("========================================\n");
    printf("  PROBLEMA DE OTIMIZACAO TURISTICA\n");
    printf("========================================\n");
    printf("Ficheiro: %s\n", ficheiro_entrada);
    printf("Numero de candidatos: %d\n", prob.num_candidatos);
    printf("Numero a selecionar: %d\n", prob.num_selecionados);
    printf("========================================\n");

    // Construire un nom de sortie sans inclure de répertoire présent dans le nom d'entrée
    char base[256];
    const char *p_slash = strrchr(ficheiro_entrada, '/');
    const char *p_bslash = strrchr(ficheiro_entrada, '\\');
    const char *fname = ficheiro_entrada;
    if (p_slash) fname = p_slash + 1;
    if (p_bslash && p_bslash + 1 > fname) fname = p_bslash + 1;
    strncpy(base, fname, sizeof(base) - 1);
    base[sizeof(base) - 1] = '\0';
    // enlever l'extension si elle existe
    char *dot = strrchr(base, '.');
    if (dot) *dot = '\0';
    char ficheiro_saida[256];
    snprintf(ficheiro_saida, sizeof(ficheiro_saida), "resultados_%s.xlsx", base);

    escrever_resultados_excel(ficheiro_saida, &prob);

    printf("\n========================================\n");
    printf("  EXECUCAO CONCLUIDA COM SUCESSO\n");
    printf("========================================\n");

    libertar_problema(&prob);
    return 0;
}