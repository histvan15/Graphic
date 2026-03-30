cmake_minimum_required(VERSION 3.10)
project(Beadando)

set(CMAKE_CXX_STANDARD 17)

#header
include_directories(external/include)

#library
link_directories(external/lib)

#source
add_executable(Beadando src/main.cpp src/glad.c)

#link
target_link_libraries(Beadando mingw32 SDL2main SDL2 opengl32)
