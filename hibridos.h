#ifndef HIBRIDOS_H
#define HIBRIDOS_H

#include "estruturas.h"
#include "config.h"

// Híbrido 1: Evolutivo + Hill Climbing (refinamento)
Solucao *hibrido1(Problema *prob, Configuracao *config);

// Híbrido 2: Hill Climbing inicial + Evolutivo
Solucao *hibrido2(Problema *prob, Configuracao *config);

#endif