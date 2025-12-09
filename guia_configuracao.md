# Guia de Configuração - Ficheiro options.txt

## Visão Geral

O projeto agora suporta **configuração externa** através do ficheiro `options.txt`. Isto permite ajustar todos os parâmetros dos algoritmos **sem recompilar o código**.

## Estrutura de Ficheiros

```
projeto/
├── options.txt           ← Ficheiro de configuração
├── main.c                ← Lê options.txt automaticamente
├── config.h/.c           ← Sistema de configuração
├── tourism_5.txt         ← Dados de entrada
└── ...
```

## Como Usar

### 1. Uso Básico (Padrão)

```bash
# Compilar
cmake --build .

# Executar (usa options.txt automaticamente)
./TP2 tourism_5.txt
```

O programa:
1. Procura `options.txt` na pasta atual
2. Se não encontrar, cria um ficheiro com valores padrão
3. Carrega os parâmetros
4. Mostra a configuração carregada
5. Executa os algoritmos

### 2. Ficheiro de Configuração Personalizado

```bash
# Usar ficheiro de configuração diferente
./TP2 tourism_5.txt minha_config.txt
```

### 3. Workflow de Experimentação

```bash
# 1. Editar options.txt
nano options.txt

# 2. Executar (sem recompilar!)
./TP2 tourism_5.txt

# 3. Ver resultados
libreoffice resultados_tourism_5.xlsx
```

## Formato do Ficheiro options.txt

### Sintaxe

```
# Comentários começam com #
parametro=valor

# Exemplos:
num_execucoes=10
hc_max_iteracoes=5000
ea_prob_cruzamento=0.8
```

**Regras:**
- Uma configuração por linha
- Formato: `chave=valor`
- Sem espaços ao redor do `=`
- Linhas vazias são ignoradas
- Comentários com `#`

## Parâmetros Disponíveis

### Parâmetros Gerais

| Parâmetro | Valores | Padrão | Descrição |
|-----------|---------|--------|-----------|
| `num_execucoes` | 1-100 | 10 | Número de execuções de cada algoritmo |
| `seed_aleatoria` | -1 ou inteiro | -1 | Seed para aleatoriedade (-1 = tempo atual) |

**Exemplos:**
```
# 30 execuções para análise estatística robusta
num_execucoes=30

# Seed fixa para resultados reproduzíveis
seed_aleatoria=42

# Teste rápido
num_execucoes=3
```

### Hill Climbing

| Parâmetro | Valores | Padrão | Descrição |
|-----------|---------|--------|-----------|
| `hc_max_iteracoes` | 100-100000 | 5000 | Número máximo de iterações |
| `hc_vizinhanca` | 0 ou 1 | 0 | 0=trocar_um, 1=trocar_dois |
| `hc_aceitar_iguais` | 0 ou 1 | 1 | Aceitar soluções de fitness igual |

**Exemplos:**
```
# Teste rápido
hc_max_iteracoes=1000
hc_vizinhanca=0

# Exploração intensiva
hc_max_iteracoes=10000
hc_vizinhanca=1

# Mais conservador
hc_aceitar_iguais=0
```

### Algoritmo Evolutivo

| Parâmetro | Valores | Padrão | Descrição |
|-----------|---------|--------|-----------|
| `ea_tamanho_populacao` | 10-200 | 50 | Tamanho da população |
| `ea_num_geracoes` | 10-1000 | 200 | Número de gerações |
| `ea_prob_cruzamento` | 0.0-1.0 | 0.8 | Probabilidade de cruzamento |
| `ea_prob_mutacao` | 0.0-1.0 | 0.1 | Probabilidade de mutação |
| `ea_tipo_selecao` | 0 ou 1 | 0 | 0=torneio, 1=roleta |
| `ea_tamanho_torneio` | 2-10 | 3 | Tamanho do torneio |
| `ea_tipo_cruzamento` | 0 ou 1 | 0 | 0=uniforme, 1=um_ponto |
| `ea_tipo_mutacao` | 0 ou 1 | 0 | 0=trocar, 1=embaralhar |

**Exemplos:**
```
# Configuração exploratória
ea_tamanho_populacao=100
ea_num_geracoes=500
ea_prob_cruzamento=0.9
ea_prob_mutacao=0.15

# Configuração rápida
ea_tamanho_populacao=20
ea_num_geracoes=50
ea_prob_cruzamento=0.7
ea_prob_mutacao=0.05

# Seleção por roleta
ea_tipo_selecao=1

# Cruzamento de um ponto
ea_tipo_cruzamento=1

# Mutação por embaralhamento
ea_tipo_mutacao=1
```

### Híbrido 1 (Evolutivo + Hill Climbing)

| Parâmetro | Valores | Padrão | Descrição |
|-----------|---------|--------|-----------|
| `h1_pop_ea` | 10-200 | 50 | População do EA |
| `h1_ger_ea` | 10-500 | 100 | Gerações do EA |
| `h1_iter_hc` | 100-10000 | 500 | Iterações HC refinamento |

**Exemplos:**
```
# Mais exploração, menos refinamento
h1_pop_ea=100
h1_ger_ea=200
h1_iter_hc=200

# Menos exploração, mais refinamento
h1_pop_ea=30
h1_ger_ea=50
h1_iter_hc=2000
```

### Híbrido 2 (Hill Climbing + Evolutivo)

| Parâmetro | Valores | Padrão | Descrição |
|-----------|---------|--------|-----------|
| `h2_iter_hc` | 100-10000 | 1000 | Iterações HC inicial |
| `h2_pop_ea` | 10-100 | 30 | População do EA |
| `h2_ger_ea` | 10-500 | 50 | Gerações do EA |

**Exemplos:**
```
# HC inicial forte
h2_iter_hc=5000
h2_pop_ea=20
h2_ger_ea=30

# Mais diversificação
h2_iter_hc=500
h2_pop_ea=50
h2_ger_ea=100
```

## Cenários de Uso

### Cenário 1: Teste Rápido (30 segundos)

```
num_execucoes=3
hc_max_iteracoes=1000
ea_tamanho_populacao=20
ea_num_geracoes=50
h1_pop_ea=20
h1_ger_ea=30
h1_iter_hc=200
h2_iter_hc=500
h2_pop_ea=15
h2_ger_ea=20
```

### Cenário 2: Teste Padrão (5 minutos)

```
num_execucoes=10
hc_max_iteracoes=5000
ea_tamanho_populacao=50
ea_num_geracoes=200
h1_pop_ea=50
h1_ger_ea=100
h1_iter_hc=500
h2_iter_hc=1000
h2_pop_ea=30
h2_ger_ea=50
```

### Cenário 3: Análise Completa (30 minutos)

```
num_execucoes=30
hc_max_iteracoes=10000
ea_tamanho_populacao=100
ea_num_geracoes=500
h1_pop_ea=100
h1_ger_ea=200
h1_iter_hc=1000
h2_iter_hc=5000
h2_pop_ea=50
h2_ger_ea=100
```

### Cenário 4: Comparar Vizinhanças HC

**Ficheiro: config_viz1.txt**
```
hc_vizinhanca=0
hc_max_iteracoes=5000
```

**Ficheiro: config_viz2.txt**
```
hc_vizinhanca=1
hc_max_iteracoes=5000
```

```bash
./TP2 tourism_5.txt config_viz1.txt
mv resultados_tourism_5.xlsx resultados_viz1.xlsx

./TP2 tourism_5.txt config_viz2.txt
mv resultados_tourism_5.xlsx resultados_viz2.xlsx
```

### Cenário 5: Comparar Seleções EA

**Ficheiro: config_torneio.txt**
```
ea_tipo_selecao=0
ea_tamanho_torneio=3
```

**Ficheiro: config_roleta.txt**
```
ea_tipo_selecao=1
```

## Dicas de Experimentação

### Para o Relatório

1. **Comparar Vizinhanças HC:**
   ```
   hc_vizinhanca=0  vs  hc_vizinhanca=1
   ```

2. **Comparar Iterações HC:**
   ```
   hc_max_iteracoes=1000
   hc_max_iteracoes=5000
   hc_max_iteracoes=10000
   ```

3. **Comparar Tamanhos População EA:**
   ```
   ea_tamanho_populacao=20
   ea_tamanho_populacao=50
   ea_tamanho_populacao=100
   ```

4. **Comparar Probabilidades:**
   ```
   ea_prob_cruzamento=0.6, 0.8, 0.9
   ea_prob_mutacao=0.05, 0.1, 0.2
   ```

5. **Comparar Operadores:**
   ```
   ea_tipo_cruzamento=0  vs  ea_tipo_cruzamento=1
   ea_tipo_mutacao=0     vs  ea_tipo_mutacao=1
   ea_tipo_selecao=0     vs  ea_tipo_selecao=1
   ```

### Script de Teste Automático

```bash
#!/bin/bash
# testar_parametros.sh

# Testar diferentes iterações HC
for iter in 1000 5000 10000; do
    echo "hc_max_iteracoes=$iter" > options.txt
    echo "# Resto dos parametros..." >> options.txt
    ./TP2 tourism_5.txt
    mv resultados_tourism_5.xlsx resultados_hc_${iter}.xlsx
done
```

## Validação de Parâmetros

O programa **não valida** os parâmetros automaticamente. Certifique-se de:

✅ **Valores Razoáveis:**
- `num_execucoes` > 0
- `*_populacao` >= 10
- `*_geracoes` > 0
- Probabilidades entre 0.0 e 1.0

❌ **Evitar:**
- População muito pequena (< 10)
- Gerações = 0
- Probabilidades > 1.0 ou < 0.0

## Troubleshooting

### Problema: Ficheiro não encontrado

```
Aviso: Ficheiro options.txt nao encontrado. A criar ficheiro padrao...
```

**Solução:** O programa cria automaticamente. Edite e execute novamente.

### Problema: Parâmetro ignorado

Se um parâmetro não está a funcionar:
1. Verifique a ortografia (case-sensitive)
2. Verifique o formato: `parametro=valor` (sem espaços)
3. Verifique se não há caracteres especiais

### Problema: Resultados não mudam

```bash
# Garantir que está a ler o ficheiro certo
./TP2 tourism_5.txt options.txt

# Verificar se o ficheiro foi modificado
cat options.txt | grep "hc_max_iteracoes"
```

## Exemplo Completo de Workflow

```bash
# 1. Criar configuração personalizada
cat > test_config.txt << EOF
num_execucoes=5
hc_max_iteracoes=3000
ea_tamanho_populacao=40
ea_num_geracoes=150
EOF

# 2. Executar
./TP2 tourism_5.txt test_config.txt

# 3. Ver configuração usada
# (o programa mostra automaticamente)

# 4. Analisar resultados
libreoffice resultados_tourism_5.xlsx
```

## Integração com Excel/LibreOffice

O ficheiro `resultados_*.xlsx` contém:
- Melhor, Média, Pior, Desvio Padrão
- Para cada algoritmo
- Baseado nos parâmetros de `options.txt`

**Dica:** Mantenha diferentes configurações e compare resultados:
```
resultados_config1.xlsx
resultados_config2.xlsx
resultados_config3.xlsx
```

## Resumo

✅ **Vantagens da Configuração Externa:**
- Testar diferentes parâmetros sem recompilar
- Fácil comparação de configurações
- Reproduzibilidade com seed fixa
- Ideal para análise experimental

📝 **Para o Relatório:**
- Documentar configurações testadas
- Incluir ficheiros `options.txt` usados
- Comparar resultados sistematicamente
- Justificar escolhas de parâmetros
