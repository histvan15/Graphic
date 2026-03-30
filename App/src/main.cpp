#define SDL_MAIN_HANDLED
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.7f, 0.2f, 1.0f);\n" 
    "}\n\0";


glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float yaw   = -90.0f;
float pitch =  0.0f; 

int main(int argc, char* argv[]) {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("Sziget Projekt - FPS Kamera", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return 1;

    SDL_SetRelativeMouseMode(SDL_TRUE); 

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 
         0.5f, -0.5f, 0.0f, 
         0.0f,  0.5f, 0.0f  
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    int projLoc  = glGetUniformLocation(shaderProgram, "projection");

    int running = 1;
    SDL_Event event;
    while (running) {
        float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;

            if (event.type == SDL_MOUSEMOTION) {
                float xoffset = event.motion.xrel;
                float yoffset = -event.motion.yrel;

                float sensitivity = 0.1f;
                xoffset *= sensitivity;
                yoffset *= sensitivity;

                yaw   += xoffset;
                pitch += yoffset;

                if (pitch > 89.0f)  pitch = 89.0f;
                if (pitch < -89.0f) pitch = -89.0f;

                glm::vec3 front;
                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront = glm::normalize(front);
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        float cameraSpeed = 2.5f * deltaTime;
        
        if (state[SDL_SCANCODE_W]) cameraPos += cameraSpeed * cameraFront;
        if (state[SDL_SCANCODE_S]) cameraPos -= cameraSpeed * cameraFront;
        if (state[SDL_SCANCODE_A]) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (state[SDL_SCANCODE_D]) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        
        if (state[SDL_SCANCODE_ESCAPE]) running = 0; 

        glClearColor(0.1f, 0.3f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}