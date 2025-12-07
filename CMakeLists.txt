cmake_minimum_required(VERSION 4.0)
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

# Créer l'exécutable
add_executable(TP2 main.c
        excel.c
        excel.h
        hibridos.c
        hibridos.h
        evolutivo.c
        evolutivo.h
        hill_climbing.c
        hill_climbing.h
        utils.c
        utils.h
        estruturas.h)

# Lier les bibliothèques
target_link_libraries(TP2 PRIVATE xlsxwriter)