cmake_minimum_required(VERSION 3.10)
project(MyZNCModule)

# Spécifiez le chemin vers les fichiers d'en-tête de ZNC
set(ZNC_INCLUDE_DIR /usr/local/include/znc)
set(ZNC_LIBRARY_DIR /usr/local/lib)  # Ajoutez ce chemin si la bibliothèque est installée ici

# Ajouter les chemins d'inclusion
include_directories(${ZNC_INCLUDE_DIR})

# Ajouter les chemins de bibliothèque
link_directories(${ZNC_LIBRARY_DIR})

# Ajouter les fichiers sources de votre module
set(SOURCES
    fish.cpp
)

# Créez la cible pour le module
add_library(fish MODULE ${SOURCES})

# Lier avec les bibliothèques de ZNC
target_link_libraries(fish znc)
