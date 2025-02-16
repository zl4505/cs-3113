/**
* Author: Christy Liu
* Assignment: Simple 2D Scene
* Date due: 2025-02-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "glm/gtc/matrix_transform.hpp"

// Window dimensions
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

// Shader file paths
const char V_SHADER_PATH[] = "shader.vert";
const char F_SHADER_PATH[] = "shader.frag";

GLuint load_texture(const char* filepath);

SDL_Window* displayWindow;
bool gameRunning = true;

// Object definitions
struct GameObject {
    glm::vec3 position;
    glm::vec3 scale;
    float rotation;
    GLuint texture;
};

// Global objects
GameObject object1;
GameObject object2;

// Time management
float lastTicks = 0.0f;

void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project 1: 2D Scene",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Load shaders
    ShaderProgram program;
    program.load(V_SHADER_PATH, F_SHADER_PATH);

    // Load textures
    object1.texture = load_texture("texture1.png");
    object2.texture = load_texture("texture2.png");

    // Initialize object properties
    object1.position = glm::vec3(0.0f, 0.0f, 0.0f);
    object1.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    object1.rotation = 0.0f;

    object2.position = glm::vec3(1.0f, 0.0f, 0.0f);
    object2.scale = glm::vec3(0.5f, 0.5f, 1.0f);
    object2.rotation = 0.0f;

    // Set projection matrix
    glm::mat4 projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    program.set_projection_matrix(projectionMatrix);
    program.set_view_matrix(glm::mat4(1.0f));
}

void processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameRunning = false;
        }
    }
}

void update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    // Object 1: Horizontal movement with scaling
    object1.position.x = 3.0f * sin(ticks);  // Non-linear horizontal movement
    object1.scale.x = 1.0f + 0.5f * sin(ticks * 2.0f);  // Scale change for extra credit

    // Object 2: Rotate around object1 with circular path
    float orbitRadius = 2.0f;
    float orbitSpeed = 1.5f;
    object2.position.x = object1.position.x + orbitRadius * cos(ticks * orbitSpeed);
    object2.position.y = object1.position.y + orbitRadius * sin(ticks * orbitSpeed);
    object2.rotation += 90.0f * deltaTime;  // Rotation for requirement 4
}

void render(ShaderProgram& program) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw object1
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, object1.position);
    modelMatrix = glm::scale(modelMatrix, object1.scale);
    program.set_model_matrix(modelMatrix);

    float vertices1[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float texCoords[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    glBindTexture(GL_TEXTURE_2D, object1.texture);
    glVertexAttribPointer(program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices1);
    glVertexAttribPointer(program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Draw object2
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, object2.position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(object2.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    program.set_model_matrix(modelMatrix);

    float vertices2[] = {
        0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f
    };

    glBindTexture(GL_TEXTURE_2D, object2.texture);
    glVertexAttribPointer(program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices2);
    glVertexAttribPointer(program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(displayWindow);
}

int main(int argc, char* argv[]) {
    initialize();
    ShaderProgram program;
    program.load(V_SHADER_PATH, F_SHADER_PATH);

    while (gameRunning) {
        processInput();
        update();
        render(program);
    }

    SDL_Quit();
    return 0;
}

GLuint load_texture(const char* filepath) {
    SDL_Surface* surface = IMG_Load(filepath);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_FreeSurface(surface);
    return textureID;
}
