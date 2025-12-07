# Projeto IIA - Otimização de Roteiro Turístico

## Descrição
Implementação de algoritmos de otimização para o problema de seleção de pontos turísticos maximizando a distância média entre eles.

## Estrutura do Projeto

```
projeto/
├── CMakeLists.txt     # Configuração de compilação
├── estruturas.h       # Definição das estruturas de dados
├── utils.h/.c         # Funções auxiliares (leitura, fitness, estatísticas)
├── hill_climbing.h/.c # Algoritmo Hill Climbing e vizinhanças
├── evolutivo.h/.c     # Algoritmo Evolutivo (seleção, recombinação, mutação)
├── hibridos.h/.c      # Métodos híbridos
├── excel.h/.c         # Geração de ficheiros Excel com resultados
├── main.c             # Programa principal      # Ficheiro de teste (exemplo)
├── README.md          # Este ficheiro
└── Data/
    ├── tourism_5.txt
    ├── tourism_20.txt
    ├── tourism_50.txt
    ├── tourism_100.txt
    ├── tourism_250.txt
    └── tourism_500.txt
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

### Software Necessário

- **CMake** (versão 4.0 ou superior)
- **Compilador C** (GCC, Clang ou MSVC)
- **Git** (para baixar dependências automaticamente)

### Instalação dos Requisitos

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install cmake build-essential git
```

#### MacOS
```bash
brew install cmake git
```

#### Windows
1. Instalar [CMake](https://cmake.org/download/)
2. Instalar [MinGW-w64](https://www.mingw-w64.org/) ou Visual Studio
3. Instalar [Git](https://git-scm.com/download/win)

### Dependências (Baixadas Automaticamente)

O projeto usa **FetchContent** do CMake para baixar e compilar automaticamente:
- ✅ **zlib** (v1.3.1) - Compressão
- ✅ **libxlsxwriter** (v1.2.3) - Geração de ficheiros Excel

**Não precisa instalar manualmente!** O CMake faz tudo automaticamente na primeira compilação.

## Compilação

### Método 1: Linha de Comando (Linux/Mac/Windows)

```bash
# 1. Criar pasta de build
mkdir build
cd build

# 2. Configurar projeto (baixa dependências na primeira vez)
cmake ..

# 3. Compilar
cmake --build .

# 4. O executável está em: build/TP2 (ou build/Debug/TP2.exe no Windows)
```

### Método 2: CLion (IDE)

1. Abrir a pasta do projeto no CLion
2. CLion detecta automaticamente o `CMakeLists.txt`
3. Clicar em **Build** → **Build Project**
4. Clicar em **Run** → **Run 'TP2'**

### Método 3: Visual Studio Code

1. Instalar extensão **CMake Tools**
2. Abrir pasta do projeto
3. Pressionar `Ctrl+Shift+P` → "CMake: Configure"
4. Pressionar `F7` para compilar
5. Pressionar `Shift+F5` para executar

### Método 4: Visual Studio (Windows)

1. Abrir pasta com **File** → **Open** → **Folder**
2. Visual Studio detecta `CMakeLists.txt`
3. Selecionar configuração (Debug/Release)
4. Clicar em **Build** → **Build All**

## Primeira Compilação (Importante!)

⏱️ **A primeira compilação demora mais tempo** (2-5 minutos) porque:
- Baixa o código-fonte do zlib (~500KB)
- Baixa o código-fonte do libxlsxwriter (~2MB)
- Compila ambas as bibliotecas

📌 **Compilações seguintes são rápidas** (~10 segundos) porque as bibliotecas já estão compiladas.

## Execução

### Linux/Mac
```bash
cd build
./TP2 tourism_5.txt
```

### Windows (MinGW)
```bash
cd build
TP2.exe tourism_5.txt
```

### Windows (Visual Studio)
```bash
cd build\Debug
TP2.exe ..\..\tourism_5.txt
```

### Exemplo de Saída

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
  Execucao 3: 51.80
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

=== Resultados salvos em resultados_tourism_5.txt.xlsx ===

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
e1 e5 33.198071
e2 e3 72.642889
e2 e4 72.506609
e2 e5 17.058938
e3 e4 71.690251
e3 e5 70.915510
e4 e5 56.557921
```

**Formato:**
- **Primeira linha**: `C m` (número de candidatos, número a selecionar)
- **Linhas seguintes**: `ponto1 ponto2 distância` (distâncias entre pares)

## Ficheiro de Saída

O programa gera um ficheiro Excel: `resultados_<nome_entrada>.xlsx`

**Conteúdo:**
- Comparação dos 4 algoritmos
- Melhor valor encontrado em 10 execuções
- Média das 10 execuções
- Pior valor encontrado
- Desvio padrão

**Exemplo:**

| Algoritmo | Melhor | Média | Pior | Desvio Padrão |
|-----------|--------|-------|------|---------------|
| Hill Climbing | 51.80 | 51.80 | 51.80 | 0.00 |
| Evolutivo | 51.80 | 51.65 | 51.20 | 0.25 |
| Hibrido 1 | 51.80 | 51.80 | 51.80 | 0.00 |
| Hibrido 2 | 51.80 | 51.75 | 51.50 | 0.15 |

## Parametrização

Os parâmetros podem ser ajustados em **excel.c**:

```c
// Linha ~30: Hill Climbing
Solucao *sol = hill_climbing(prob, 5000, 0);
                                    //   ↑     ↑
                                    // iter  vizinhança

// Linha ~45: Evolutivo
Solucao *sol = algoritmo_evolutivo(prob, 50, 200, 0.8, 0.1, 0, 0, 0);
                                        // ↑   ↑    ↑    ↑   ↑  ↑  ↑
                                        // pop ger  cruz mut sel cr mt
```

**Parâmetros:**
- `iter`: Número de iterações do Hill Climbing (ex: 1000, 5000, 10000)
- `pop`: Tamanho da população (ex: 20, 50, 100)
- `ger`: Número de gerações (ex: 100, 200, 500)
- `cruz`: Probabilidade de cruzamento (ex: 0.6, 0.8, 0.9)
- `mut`: Probabilidade de mutação (ex: 0.05, 0.1, 0.2)
- `sel`: Tipo de seleção (0=torneio, 1=roleta)
- `cr`: Tipo de cruzamento (0=uniforme, 1=um ponto)
- `mt`: Tipo de mutação (0=trocar, 1=embaralhar)

Para alterar o número de execuções, editar em **estruturas.h**:
```c
#define NUM_EXECUCOES 10  // Alterar para 20, 30, etc.
```

## Comandos Úteis

```bash
# Recompilar do zero
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .

# Compilar em modo Release (mais rápido)
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Ver todos os comandos de compilação
cmake --build . --verbose

# Limpar apenas ficheiros compilados (mantém dependências)
cmake --build . --target clean

# Executar testes com diferentes ficheiros
./TP2 tourism_5.txt
./TP2 tourism_10.txt
./TP2 tourism_50.txt
```

## Resolução de Problemas

### Erro: "CMake version 4.0 or higher is required"
```bash
# Verificar versão
cmake --version

# Atualizar CMake
# Linux:
sudo apt-get install cmake

# Mac:
brew upgrade cmake

# Windows: baixar de cmake.org
```

### Erro: "Git not found"
```bash
# Instalar Git
# Linux:
sudo apt-get install git

# Mac:
brew install git

# Windows: baixar de git-scm.com
```

### Erro na compilação do zlib ou libxlsxwriter
```bash
# Limpar cache e tentar novamente
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build .
```

### Erro: "Stack overflow" ou crash
O problema foi corrigido usando alocação dinâmica. Se persistir:
```bash
# Linux: aumentar stack
ulimit -s unlimited

# Recompilar
cmake --build . --target clean
cmake --build .
```

### Ficheiro Excel não abre
- Verificar se o ficheiro foi gerado: `ls -l resultados_*.xlsx`
- Tentar abrir com LibreOffice ou Excel Online
- Verificar permissões: `chmod 644 resultados_*.xlsx`

## Testes Experimentais Sugeridos

### Hill Climbing
- [ ] Variar número de iterações: 1000, 5000, 10000
- [ ] Comparar vizinhança 1 (0) vs vizinhança 2 (1)
- [ ] Testar aceitação de soluções de custo igual

### Algoritmo Evolutivo
- [ ] Tamanhos de população: 20, 50, 100
- [ ] Probabilidades de cruzamento: 0.6, 0.8, 0.9
- [ ] Probabilidades de mutação: 0.05, 0.1, 0.2
- [ ] Comparar seleção: torneio (0) vs roleta (1)
- [ ] Comparar cruzamento: uniforme (0) vs um ponto (1)
- [ ] Comparar mutação: trocar (0) vs embaralhar (1)

### Híbridos
- [ ] Comparar Híbrido 1 vs Híbrido 2
- [ ] Analisar tempo de execução vs qualidade da solução
- [ ] Testar com diferentes instâncias (5, 10, 50, 100 pontos)

## Variáveis em Português

Todas as variáveis e funções estão em português para facilitar a leitura:

**Estruturas:**
- `Problema`, `Solucao`, `Populacao`, `Estatisticas`
- `num_candidatos`, `num_selecionados`, `distancias`

**Funções:**
- `ler_problema()`, `libertar_problema()`
- `calcular_fitness()`, `criar_solucao_aleatoria()`
- `vizinho_trocar_um()`, `vizinho_trocar_dois()`
- `selecao_torneio()`, `selecao_roleta()`
- `recombinacao_uniforme()`, `mutacao_trocar()`
- `algoritmo_evolutivo()`, `hibrido1()`, `hibrido2()`

## Estrutura de Ficheiros Gerados

```
build/
├── _deps/                    # Dependências (zlib, libxlsxwriter)
│   ├── zlib-src/
│   ├── zlib-build/
│   ├── libxlsxwriter-src/
│   └── libxlsxwriter-build/
├── CMakeFiles/               # Ficheiros internos do CMake
├── TP2 (ou TP2.exe)          # Executável
└── resultados_*.xlsx         # Resultados gerados
```

## Notas Importantes

1. ✅ **Primeira compilação demora mais** - é normal!
2. ✅ **Não precisa instalar zlib/libxlsxwriter** - CMake faz automaticamente
3. ✅ **Funciona em Windows, Linux e Mac** - mesmo CMakeLists.txt
4. ✅ **Alocação dinâmica** - sem problemas de stack overflow
5. ✅ **10 execuções por algoritmo** - para análise estatística robusta

## Checklist de Entrega

- [ ] Código compila sem erros (`cmake --build .`)
- [ ] Código executa com tourism_5.txt
- [ ] Ficheiro Excel é gerado corretamente
- [ ] Todas as variáveis em português
- [ ] Código comentado
- [ ] README.md completo
- [ ] CMakeLists.txt incluído
- [ ] Ficheiros .h e .c organizados
- [ ] Relatório em PDF com análise dos resultados
- [ ] Ficheiro Excel com todos os testes experimentais

## Autores

[Seu nome e número]  
[Nome do colega e número]

**Curso:** Licenciatura em Engenharia Informática  
**Disciplina:** Introdução à Inteligência Artificial  
**Ano Letivo:** 2025/2026

## Licença

Projeto académico - IIA 2025/2026