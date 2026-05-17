cmake_minimum_required(VERSION 3.10)
project(Beadando)

set(CMAKE_C_STANDARD 99)

#header
include_directories(external/include)

#library
link_directories(external/lib)

#source
add_executable(Beadando src/engine.c src/main.c src/glad.c src/camera.c src/window.c)

#link
target_link_libraries(Beadando mingw32 SDL2main SDL2 opengl32)


file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/SDL2.dll")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/SDL2.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()