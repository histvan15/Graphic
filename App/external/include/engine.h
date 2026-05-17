#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct { float x, y; } vec2;
typedef struct { float x, y, z; } vec3;

vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_scale(vec3 v, float s);
vec3 vec3_cross(vec3 a, vec3 b);
float vec3_dot(vec3 a, vec3 b);
vec3 vec3_normalize(vec3 v);
float radians(float deg);
void mat4_identity(float m[16]);
void mat4_perspective(float fovy, float aspect, float near_z, float far_z, float m[16]);
void mat4_lookAt(vec3 eye, vec3 center, vec3 up, float m[16]);

char* loadShaderSource(const char* filePath);
unsigned int createProgram(const char* vertexPath, const char* fragmentPath);
float* loadOBJ(const char* path, int* out_vertexCount);
unsigned int loadCubemap(char* faces[]);
unsigned int loadTexture(const char* path);
void setupMesh(float* vertices, size_t size, unsigned int* vao, unsigned int* vbo);
void drawMesh(unsigned int vao, unsigned int texture, int vertexCount, int modelLoc, float* modelMatrix);

#endif