# Projeto IIA — Otimizador de Roteiro Turístico

## Visão Geral

Este projeto implementa e compara algoritmos de otimização para selecionar um subconjunto de pontos turísticos que maximiza a distância média entre os pontos escolhidos. Suporta execução normal e um modo de análise profunda de parâmetros, com exportação automática de resultados para ficheiros Excel.

Problema: dado um conjunto de locais e as distâncias entre eles, escolher K locais maximizando a distância média intra‑conjunto. O K e os dados vêm dos ficheiros `Data/tourism_*.txt`.

## Estrutura do Projeto

```
TP2/
├── CMakeLists.txt       # Configuração de compilação e dependências
├── estruturas.h         # Estruturas: Problema, Solucao, Populacao
├── utils.h/.c           # I/O, cálculo de fitness, auxiliares, aleatoriedade
├── hill_climbing.h/.c   # Hill Climbing e vizinhanças
├── evolutivo.h/.c       # EA: seleção, recombinação, mutação
├── hibridos.h/.c        # Combinações EA + HC
├── excel.h/.c           # Escrita de resultados em Excel (libxlsxwriter)
├── config.h/.c          # Sistema de configuração (options.txt)
├── analise_profunda.h/.c# Estudo sistemático de parâmetros (multithread)
├── guia_configuracao.md # Guia detalhado do ficheiro options.txt
├── main.c               # Programa principal
├── options.txt          # Parâmetros dos algoritmos (editável)
├── README.md            # Este documento
└── Data/
    ├── tourism_5.txt
    ├── tourism_20.txt
    ├── tourism_50.txt
    ├── tourism_100.txt
    ├── tourism_250.txt
    └── tourism_500.txt
```

## Como Compilar

Este repositório usa CMake e baixa automaticamente as dependências `zlib` e `libxlsxwriter` via FetchContent na primeira configuração.

- Requisitos: CMake, toolchain C (GCC/Clang/MinGW/MSVC). No Windows, recomenda‑se CLion com MinGW ou Visual Studio.

### CLion (recomendado)

1. Abrir a pasta do projeto no CLion
2. Selecionar o perfil de CMake ativo (ex.: Debug com MinGW)
3. Target principal: `TP2`
4. Build: Build Project; Run: Run 'TP2'

### Linha de comando (genérico)

```bash
cmake -S . -B build
cmake --build build --target TP2
```

No Windows, o executável será algo como `build\TP2.exe` dependendo do gerador.

## Como Executar

Sintaxe geral:

```
TP2 <ficheiro_entrada> [--config <ficheiro>] [--analise-profunda]
```

- `<ficheiro_entrada>`: um ficheiro do diretório `Data/` (por exemplo, `Data/tourism_20.txt`).
- `--config <ficheiro>`: usa um ficheiro de configuração alternativo (por padrão, `options.txt`).
- `--analise-profunda`: ativa a análise sistemática de parâmetros com multithreading.

Exemplos:

- Windows (PowerShell):
  - Execução normal e exportação para Excel:```
  .\TP2.exe Data\tourism_20.txt
  ```
  - Usando configuração personalizada:```
  .\TP2.exe Data\tourism_50.txt --config minha_config.txt
  ```
  - Análise profunda multi‑thread:```
  .\TP2.exe Data\tourism_100.txt --analise-profunda
  ```

- Linux/macOS:
  - `./TP2 Data/tourism_20.txt --analise-profunda`

Saídas principais:
- Modo normal: `resultados_<dataset>.xlsx`
- Análise profunda: `analise_profunda_<dataset>.xlsx`

## Ficheiro de Configuração (options.txt)

O programa lê automaticamente `options.txt` da pasta atual. Pode indicar outro com `--config`. O formato é simples `chave=valor`, com comentários iniciados por `#`. Veja exemplos e recomendações avançadas em `guia_configuracao.md`.

Parâmetros gerais:
- `num_execucoes` (int): número de execuções independentes por algoritmo no modo normal.
- `seed_aleatoria` (int): `-1` usa tempo atual; outro valor fixa a seed para reprodutibilidade.
- `analise_n_threads` (int): número de threads quando usa `--analise-profunda`.

Hill Climbing (prefixo `hc_`):
- `hc_max_iteracoes` (int): iterações máximas do HC.
- `hc_vizinhanca` (0/1): 0=trocar um ponto; 1=trocar dois pontos.
- `hc_aceitar_iguais` (0/1): aceita movimentos com fitness igual (ajuda a sair de planaltos).

Algoritmo Evolutivo (prefixo `ea_`):
- `ea_tamanho_populacao` (int): tamanho da população.
- `ea_num_geracoes` (int): número de gerações.
- `ea_prob_cruzamento` (0.0–1.0): probabilidade de recombinação.
- `ea_prob_mutacao` (0.0–1.0): probabilidade de mutação.
- `ea_tipo_selecao` (0/1): 0=torneio; 1=roleta.
- `ea_tamanho_torneio` (int): apenas se seleção=torneio.
- `ea_tipo_cruzamento` (0/1): 0=uniforme; 1=um ponto.
- `ea_tipo_mutacao` (0/1): 0=trocar um gene; 1=embaralhar.

Métodos Híbridos (prefixo `h1_` e `h2_`):
- H1 (EA + HC):
  - `h1_pop_ea`, `h1_ger_ea`: parâmetros do EA na fase de exploração
  - `h1_iter_hc`: número de iterações de refinamento com HC
- H2 (HC + EA):
  - `h2_iter_hc`: iterações do HC inicial
  - `h2_pop_ea`, `h2_ger_ea`: parâmetros do EA após a semente do HC

Sugestões de valores e presets para testes rápidos/completos encontram‑se comentados em `options.txt`.

## Como Funcionam os Algoritmos

### Fitness e Representação

- Uma `Solucao` contém `tamanho` índices de pontos selecionados e o seu `fitness`.
- O `fitness` é a distância média entre todos os pares de pontos selecionados, calculado a partir da matriz de distâncias do `Problema` (ver `utils.c`). Maximizar o fitness equivale a espalhar os pontos.

### Hill Climbing (HC)

Objetivo: melhorar iterativamente uma solução por pequenas alterações locais.

Passos (ver `hill_climbing.c`):
1. Gera uma solução inicial aleatória: `criar_solucao_aleatoria`.
2. Em cada iteração até `hc_max_iteracoes`:
   - Gera um vizinho com base em `hc_vizinhanca`:
     - 0 — `vizinho_trocar_um`: troca um ponto selecionado por um não selecionado aleatório.
     - 1 — `vizinho_trocar_dois`: troca dois pontos distintos por dois não selecionados.
   - Calcula o fitness do vizinho.
   - Aceita o vizinho se o fitness for maior; se `hc_aceitar_iguais=1`, também aceita empates.
   - Mantém a melhor solução observada.

Parâmetros e impacto:
- Mais iterações tendem a melhor qualidade (mais tempo).
- Vizinhança 2 explora mais (mais lento) e pode escapar de ótimos locais diferentes.
- Aceitar iguais facilita atravessar planaltos no espaço de soluções.

### Algoritmo Evolutivo (EA)

Objetivo: explorar diversas regiões do espaço de soluções com população e operadores evolutivos.

Componentes (ver `evolutivo.c`):
- População inicial: `criar_populacao` com soluções aleatórias válidas.
- Seleção de pais:
  - Torneio (`selecao_torneio`): escolhe o melhor entre amostras aleatórias de tamanho `ea_tamanho_torneio`.
  - Roleta (`selecao_roleta`): probabilidade proporcional ao fitness (normalizada, com cuidado a limites).
- Recombinação (cruzamento):
  - Uniforme (`recombinacao_uniforme`): para cada posição, escolhe gene do pai 1 ou 2 aleatoriamente e repara duplicados/omissões garantindo validade.
  - Um ponto (`recombinacao_um_ponto`): concatena prefixo de um pai com sufixo do outro, com reparação para manter um conjunto sem repetidos.
- Mutação:
  - Trocar (`mutacao_trocar`): substitui um gene selecionado por um ponto não selecionado aleatório.
  - Embaralhar (`mutacao_embaralhar`): baralha várias posições, promovendo diversidade.
- Elitismo: preserva a melhor solução entre gerações (ver também `hibridos.c` para um elitismo explícito no H2).

Parâmetros e impacto:
- Populações maiores e mais gerações aumentam a exploração/convergência, mas custam tempo.
- Cruzamento alto (0.8–0.9) com mutação moderada/baixa (0.05–0.1) tende a funcionar bem.
- Torneio é geralmente mais estável e eficiente que roleta neste problema.

### Métodos Híbridos

Combinações que aproveitam forças complementares de EA e HC (ver `hibridos.c`).

- Híbrido 1 (EA → HC):
  1. Corre o EA com `h1_pop_ea` e `h1_ger_ea` para encontrar regiões promissoras.
  2. Refina a melhor solução com `h1_iter_hc` passos de HC usando vizinhança de troca de 1 ponto.
  - Quando usar: quando o EA encontra boas regiões, o HC melhora localmente.

- Híbrido 2 (HC → EA):
  1. Executa HC por `h2_iter_hc` para obter uma semente de qualidade.
  2. Injeta a semente como indivíduo elitista na população inicial do EA (`h2_pop_ea`) e evolui por `h2_ger_ea`.
  - Quando usar: quando HC encontra rapidamente um bom ponto de partida e o EA diversifica à volta.

## Modo de Análise Profunda (multi‑thread)

Ativado com `--analise-profunda`, explora grelhas de parâmetros e executa múltiplas repetições em paralelo para estimar estatísticas:

Métricas registadas (ver `analise_profunda.h`):
- `melhor`, `media`, `pior`, `desvio_padrao`, `tempo_execucao`.

Controlo de paralelismo:
- `analise_n_threads` em `options.txt`. Se não definido/0, o programa pergunta interativamente e usa um valor por defeito.

Saída: `analise_profunda_<dataset>.xlsx` com tabelas por configuração; no terminal, são mostradas as melhores combinações encontradas.

## Saída e Ficheiros Excel

Durante a execução normal, o programa agrega resultados de `num_execucoes` e escreve `resultados_<dataset>.xlsx`. O ficheiro contém, por algoritmo e configuração:
- Melhor, média, pior, desvio‑padrão do fitness
- Tempos médios por execução
- Opcionalmente, registos das melhores soluções (dependente da implementação em `excel.c`)

Os nomes baseiam‑se no nome do dataset. O sistema usa `libxlsxwriter` (disponibilizada automaticamente).

## Dicas Práticas

- Teste rápido:
  - `num_execucoes=3`
  - `hc_max_iteracoes=1000`
  - `ea_tamanho_populacao=20`, `ea_num_geracoes=50`

- Experiência completa:
  - `num_execucoes=30`
  - `hc_max_iteracoes=10000`
  - `ea_tamanho_populacao=100`, `ea_num_geracoes=500`

- Seeds: para reproduzir resultados, defina `seed_aleatoria=42` (ou outro inteiro).

## Perguntas Frequentes (FAQ)

- Onde defino K (número de pontos selecionados)?
  - É lido a partir dos ficheiros de entrada (estrutura do problema); o código assegura consistência no tamanho das soluções.

- O que acontece se eu não tiver `options.txt`?
  - O programa cria um ficheiro com valores padrão ao iniciar (via `config.c`) e informa no terminal.

- Como escolher entre vizinhança 0 e 1 no HC?
  - 0 é mais rápido e local; 1 explora mais e pode evitar ótimos locais, à custa de tempo.

## Licença

Uso académico/educativo. Ajuste conforme as necessidades do seu curso/disciplina.