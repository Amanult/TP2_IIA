# Projeto IIA - Otimização de Roteiro Turístico

## Descrição
Implementação de algoritmos de otimização para o problema de seleção de pontos turísticos maximizando a distância média entre eles.

## Estrutura do Projeto

```
projeto/
├── estruturas.h       # Definição das estruturas de dados
├── utils.h/.c         # Funções auxiliares (leitura, fitness, estatísticas)
├── hill_climbing.h/.c # Algoritmo Hill Climbing e vizinhanças
├── evolutivo.h/.c     # Algoritmo Evolutivo (seleção, recombinação, mutação)
├── hibridos.h/.c      # Métodos híbridos
├── excel.h/.c         # Geração de ficheiros Excel com resultados
├── main.c             # Programa principal
├── Makefile           # Script de compilação
└── README.md          # Este ficheiro
```

## Algoritmos Implementados

1. **Hill Climbing (Trepa-Colinas)**
    - Vizinhança 1: Trocar um ponto por outro não selecionado
    - Vizinhança 2: Trocar dois pontos por dois não selecionados

2. **Algoritmo Evolutivo**
    - Seleção: Torneio e Roleta
    - Recombinação: Uniforme e Um ponto
    - Mutação: Troca e Embaralhamento

3. **Métodos Híbridos**
    - Híbrido 1: Evolutivo seguido de Hill Climbing (refinamento)
    - Híbrido 2: Hill Climbing inicial + Evolutivo

## Requisitos

### Linux/Ubuntu
```bash
sudo apt-get update
sudo apt-get install libxlsxwriter-dev
sudo apt-get install zlib1g-dev
```

### MacOS
```bash
brew install libxlsxwriter
```

### Windows (MinGW/MSYS2)
```bash
pacman -S mingw-w64-x86_64-libxlsxwriter
```

## Compilação

### Usando Makefile
```bash
make
```

### Compilação manual
```bash
gcc -c main.c -Wall -O2
gcc -c utils.c -Wall -O2
gcc -c hill_climbing.c -Wall -O2
gcc -c evolutivo.c -Wall -O2
gcc -c hibridos.c -Wall -O2
gcc -c excel.c -Wall -O2
gcc main.o utils.o hill_climbing.o evolutivo.o hibridos.o excel.o -o otimizador_turismo -lxlsxwriter -lz -lm
```

## Execução

```bash
./otimizador_turismo tourism_5.txt
```

O programa irá:
1. Ler o ficheiro de entrada
2. Executar cada algoritmo NUM_EXECUCOES vezes (definido em estruturas.h)
3. Calcular estatísticas (melhor, média, pior, desvio padrão)
4. Gerar um ficheiro Excel com os resultados (ver secção abaixo)

### Exemplo de Saída (terminal)

```
========================================
  PROBLEMA DE OTIMIZACAO TURISTICA
========================================
Ficheiro: tourism_5.txt
Numero de candidatos: 5
Numero a selecionar: 3
========================================

=== Executando testes ===
Hill Climbing...
  Execucao 1: 51.80
  Execucao 2: 51.80
  ...

Algoritmo Evolutivo...
  Execucao 1: 51.80
  ...

Hibrido 1 (Evolutivo + HC)...
  Execucao 1: 51.80
  ...

Hibrido 2 (HC + Evolutivo)...
  Execucao 1: 51.80
  ...

=== Resultados salvos em resultados_tourism_5.xlsx ===

========================================
  EXECUCAO CONCLUIDA COM SUCESSO
========================================
```

## Formato do Ficheiro de Entrada

```
5 3
e1 e2 50.171360
e1 e3 82.421549
e1 e4 32.755369
...
```

- Primeira linha: número total de candidatos (C) e número a selecionar (m)
- Linhas seguintes: distâncias entre pares de pontos (identificadores no formato e1, e2, ...)

## Ficheiro de Saída

O programa gera um ficheiro Excel com o padrão:
`resultados_<base>.xlsx`  
onde `<base>` é o nome do ficheiro de entrada sem caminho nem extensão (por exemplo, para `tourism_5.txt` o ficheiro é `resultados_tourism_5.xlsx`).

A workbook contém uma folha chamada "Resultados" com, por algoritmo:
- Melhor valor encontrado
- Média de NUM_EXECUCOES execuções
- Pior valor
- Desvio padrão
- (as execuções individuais ficam também disponíveis na estrutura interna)

## Parametrização

Os parâmetros podem ser ajustados nos ficheiros:

**estruturas.h**
```c
#define NUM_EXECUCOES 10  // Número de repetições
```

**excel.c**
```c
// Hill Climbing
hill_climbing(prob, 5000, 0);  // max_iteracoes, usar_vizinho2

// Evolutivo
algoritmo_evolutivo(prob, 50, 200, 0.8, 0.1, 0, 0, 0);
// tamanho_pop, geracoes, prob_cruz, prob_mut, tipo_sel, tipo_cruz, tipo_mut
```

## Variáveis em Português

Todas as variáveis e funções estão em português:
- `num_candidatos`, `num_selecionados`
- `distancias`, `fitness`
- `selecionados`, `tamanho`
- `criar_solucao_aleatoria()`, `calcular_fitness()`
- `libertar_solucao()`, `copiar_solucao()`
- etc.

## Testes Experimentais Sugeridos

### Hill Climbing
- [ ] Variar número de iterações: 1000, 5000, 10000
- [ ] Comparar vizinhança 1 vs vizinhança 2
- [ ] Testar aceitação de soluções de custo igual

### Algoritmo Evolutivo
- [ ] Tamanhos de população: 20, 50, 100
- [ ] Probabilidades de cruzamento: 0.6, 0.8, 0.9
- [ ] Probabilidades de mutação: 0.05, 0.1, 0.2
- [ ] Comparar métodos de seleção (torneio vs roleta)
- [ ] Comparar operadores de recombinação
- [ ] Comparar operadores de mutação

### Híbridos
- [ ] Comparar qual produz melhores resultados
- [ ] Analisar tempo de execução vs qualidade

## Comandos Úteis

```bash
# Compilar
make

# Executar
make run

# Limpar ficheiros compilados
make clean

# Ver ficheiros objeto gerados
ls -l *.o
```

## Notas

- Todas as soluções respeitam a restrição de selecionar exatamente m pontos
- O programa usa seed aleatória baseada no tempo (srand(time(NULL)))
- Para resultados reproduzíveis, pode fixar a seed em main.c: `srand(42);`
- Os ficheiros .h contêm apenas declarações
- Os ficheiros .c contêm as implementações

## Autores

[Seu nome e número]  
[Nome do colega e número]

## Licença

Projeto académico - IIA 2025/2026