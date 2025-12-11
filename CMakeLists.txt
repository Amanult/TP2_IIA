cmake_minimum_required(VERSION 3.10)
project(TP2 C)

set(CMAKE_C_STANDARD 23)

include(FetchContent)

# Télécharger et compiler zlib automatiquement
FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG v1.3.1
)

FetchContent_MakeAvailable(zlib)

# Configurer les variables pour que libxlsxwriter trouve zlib
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(ZLIB_LIBRARY zlibstatic)
set(ZLIB_FOUND TRUE)
set(ZLIB_VERSION_STRING "1.3.1")

# Créer un alias pour la cible ZLIB
add_library(ZLIB::ZLIB ALIAS zlibstatic)

# Télécharger libxlsxwriter
FetchContent_Declare(
        libxlsxwriter
        GIT_REPOSITORY https://github.com/jmcnamara/libxlsxwriter.git
        GIT_TAG v1.2.3
)

FetchContent_MakeAvailable(libxlsxwriter)

# Ficheiros fonte
set(SOURCES
        main.c
        utils.c
        config.c
        hill_climbing.c
        evolutivo.c
        hibridos.c
        excel.c
        analise_profunda.c
        analise_profunda.h
)

# Ficheiros header
set(HEADERS
        estruturas.h
        utils.h
        config.h
        hill_climbing.h
        evolutivo.h
        hibridos.h
        excel.h
        analise_profunda.c
        analise_profunda.h
)

# Criar executável
add_executable(TP2 ${SOURCES} ${HEADERS})

# Linkar bibliotecas usando os nomes dos Targets (Alvos)
target_link_libraries(TP2
        xlsxwriter      # Nome do target criado pelo FetchContent do libxlsxwriter
        ZLIB::ZLIB      # Nome do alias que você criou para o zlib
        # ${MATH_LIB}   # Geralmente desnecessário no Windows (MSVC). No Linux seria 'm'.
)

# Mensagens informativas
message(STATUS "Projeto: ${PROJECT_NAME}")
message(STATUS "Compilador C: ${CMAKE_C_COMPILER}")
message(STATUS "Flags: ${CMAKE_C_FLAGS}")

# Opcional: Copiar ficheiros de teste para a pasta build
file(COPY ${CMAKE_SOURCE_DIR}/Data/tourism_5.txt
        DESTINATION ${CMAKE_BINARY_DIR})

# Target personalizado para executar
add_custom_target(run
        COMMAND TP2 tourism_5.txt
        DEPENDS TP2
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "A executar TP2..."
)