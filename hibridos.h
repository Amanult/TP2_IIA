//
// Created by Gabriel on 07/12/2025.
//

#ifndef HIBRIDOS_H
#define HIBRIDOS_H

#include "estruturas.h"

// Híbrido 1: Evolutivo + Hill Climbing (refinamento)
Solucao *hibrido1(Problema *prob);

// Híbrido 2: Hill Climbing inicial + Evolutivo
Solucao *hibrido2(Problema *prob);

#endif