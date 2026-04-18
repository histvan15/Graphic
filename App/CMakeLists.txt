cmake_minimum_required(VERSION 3.10)
project(Beadando)

set(CMAKE_C_STANDARD 99)

#header
include_directories(external/include)

#library
link_directories(external/lib)

#source
add_executable(Beadando src/main.c src/glad.c)

#link
target_link_libraries(Beadando mingw32 SDL2main SDL2 opengl32)


file(COPY textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY models DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})