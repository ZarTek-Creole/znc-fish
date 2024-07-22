cmake_minimum_required(VERSION 3.10)

# Nom du projet
project(ZncFish)

# Utiliser C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ajouter les fichiers source
add_executable(znc-fish fish.cpp)

# Inclure les headers si nécessaire
# include_directories(include)

# Trouver GoogleTest (si vous utilisez GoogleTest pour vos tests)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Lier GoogleTest aux tests
# add_executable(runTests test/test_fish.cpp)
# target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)

# Ajouter des tests
# enable_testing()
# add_test(NAME MyTest COMMAND runTests)
