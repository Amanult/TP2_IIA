#ifndef EXCEL_H
#define EXCEL_H

#include "estruturas.h"
#include "config.h"

// Escreve os resultados em ficheiro Excel
void escrever_resultados_excel(const char *nome_ficheiro, Problema *prob, Configuracao *config);

#endif