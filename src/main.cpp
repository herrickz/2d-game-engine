
#include "Hello.h"
#include "Goodbye.h"
#include "Logger.h"
#include "Render/Render.h"
#include "Window/Window.h"

#include <glad/gl.h>
#include "GLFW/glfw3.h"

#include <iostream>
#include <fmt/format.h>

int32_t windowWidth = 640;
int32_t windowHeight = 480;

void errorCallback(int code, const char *description) {
    logger::logError(fmt::format("code={} description={}", code, description));
}

int main(int argc, char **argv) {

    if(!glfwInit()) {
        logger::logError("GLFW error");
        return 1;
    }

    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Golf Simulator", NULL, NULL);
    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);

    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }

    logger::logInfo(fmt::format("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version)));

    if (!window) {
        logger::logError("Failed to create window");

        const char *error;

        int glfwCode = glfwGetError(&error);

        if(error) {
            logger::logError(fmt::format("code={} error={}", glfwCode, error));
        }

        return 1;
    }

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    uint32_t vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,  GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3,  GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "} \n";

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        logger::logError(fmt::format("Vertex shader compilation error: {}", infoLog));
    }
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        logger::logError(fmt::format("Fragment shader compilation error: {}", infoLog));
    }
    uint32_t shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        logger::logError(fmt::format("Shader linking error: {}", infoLog));
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);

    while(!glfwWindowShouldClose(window)) {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // logger::logInfo(fmt::format("{}x{}", windowWidth, windowHeight));

        // if(glfwGetWindowAttrib(window, GLFW_HOVERED)) {
        //     logger::logInfo("Window is hovered");
        // }
    }

}
