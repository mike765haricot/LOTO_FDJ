cmake_minimum_required(VERSION 3.10)
project(Essaie_Millionaire)

# On force l'utilisation du standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# On indique que l'executable est genere a partir de main.cpp
add_executable(Essaie_Millionaire main.cpp)