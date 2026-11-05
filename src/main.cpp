
#include "Logger.h"

#include <glad/gl.h>
#include "GLFW/glfw3.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <thread>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <math.h>

int32_t windowWidth = 640;
int32_t windowHeight = 480;

void errorCallback(int code, const char *description) {
    logger::logError(fmt::format("code={} description={}", code, description));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    logger::logInfo(fmt::format("Received key={} action={}", key, action));

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        logger::logInfo("Escape pressed. Exiting");
        glfwSetWindowShouldClose(window, 1);
    }
}

std::vector<uint8_t> getImageFlipped(
    const std::vector<uint8_t> &imageData,
    const uint32_t width,
    const uint32_t height
) {

    const uint32_t fullSize = width * height * 3;
    std::vector<uint8_t> flippedImageData(fullSize, 0x00);

    logger::logInfo(
        fmt::format(
            "width={} x height={} x 3 = {} = {} = imageData.size() = {}",
            width,
            height,
            width * height * 3,
            fullSize,
            imageData.size()
        )
    );

    int32_t i = fullSize - width * 3;
    uint32_t fullWidthSize = width * 3;

    for(; i >= 0; i = i - fullWidthSize) {
        std::copy(
            imageData.data() + i,
            imageData.data() + i + fullWidthSize,
            flippedImageData.data() + fullSize - i - fullWidthSize
        );
    }

    return flippedImageData;
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
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // top left
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right

        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f    // top right
    };

    uint32_t vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3,  GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::ifstream vertexShaderFile(
        "/home/zack/Documents/development/cpp/golf-simulator/shaders/triangle.vert",
        std::ios_base::binary
    );

    std::stringstream ss;

    ss << vertexShaderFile.rdbuf();

    const std::string vertexShaderString = ss.str();
    const char  *vertexShaderCString = vertexShaderString.c_str();

    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCString, NULL);
    glCompileShader(vertexShader);

    std::ifstream fragmentShaderFile(
        "/home/zack/Documents/development/cpp/golf-simulator/shaders/triangle.frag",
        std::ios_base::binary
    );

    ss.str("");

    ss << fragmentShaderFile.rdbuf();

    const std::string fragementShaderSource = ss.str();
    const char *fragmentShaderCString = fragementShaderSource.c_str();

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        logger::logError(fmt::format("Vertex shader compilation error: {}", infoLog));
    }

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCString, NULL);
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

    int32_t runningCounter = 0;

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::ifstream textureFile(
        "/home/zack/Documents/development/cpp/golf-simulator/build/assets/temp.ppm",
        std::ios::binary
    );

    char firstTwoBytes[2];

    textureFile.read(firstTwoBytes, 2);

    if(firstTwoBytes[0] != 'P' || firstTwoBytes[1] != '6') {
        logger::logError("Unexpected first two bytes for texture file");
        exit(1);
    }

    while(textureFile.peek() == 0x0A) {
        textureFile.get();
    }

    std::vector<uint8_t> widthBytes;

    while(textureFile.peek() != 0x20) {
        widthBytes.push_back(textureFile.get());
    }

    widthBytes.push_back(0);

    while(textureFile.peek() == 0x20) {
        textureFile.get();
    }

    std::vector<uint8_t> heightBytes;

    while(textureFile.peek() != 0x0A) {
        heightBytes.push_back(textureFile.get());
    }

    heightBytes.push_back(0x00);

    while(textureFile.peek() == 0x0A) {
        textureFile.get();
    }

    std::vector<uint8_t> rgbBytes;

    while(textureFile.peek() != 0x0A) {
        rgbBytes.push_back(textureFile.get());
    }

    rgbBytes.push_back(0x00);

    int32_t width = std::stoi((const char *)widthBytes.data());
    int32_t height = std::stoi((const char *)heightBytes.data());
    int32_t rgb = std::stoi((const char *)rgbBytes.data());

    uint32_t headerSize = static_cast<uint32_t>(textureFile.tellg()) + 1;
    textureFile.seekg(0, std::ios::end);
    uint32_t restOfFileSize = static_cast<uint32_t>(textureFile.tellg()) - headerSize;
    uint32_t expectedRestOfFileSize = width * height * 3;

    if(rgb != 255) {
        logger::logError(fmt::format("Unexpected rgb value: {}", rgb));
        exit(1);
    }

    if(restOfFileSize != expectedRestOfFileSize) {
        logger::logError(fmt::format("restOfFileSize ({}) != expectedRestOfFileSize({})", restOfFileSize, expectedRestOfFileSize));
        exit(1);
    }

    logger::logInfo(fmt::format("width={} height={} rgb={}", width, height, rgb));
    logger::logInfo(
        fmt::format(
            "header_size={} rest_of_file_size={} expected_rest_of_file_size={}",
            headerSize,
            restOfFileSize,
            expectedRestOfFileSize
        )
    );

    std::vector<uint8_t> header(headerSize, 0x00);
    textureFile.seekg(0, std::ios::beg);
    textureFile.read((char *)header.data(), headerSize);

    std::vector<uint8_t> rgbData(expectedRestOfFileSize, 0x00);
    textureFile.seekg(headerSize, std::ios::beg);
    textureFile.read((char *)rgbData.data(), expectedRestOfFileSize);
    textureFile.close();

    auto flippedImage = getImageFlipped(rgbData, width, height);

    logger::logInfo(fmt::format("flippedImage size = {}", flippedImage.size()));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, flippedImage.data());

    glfwSetKeyCallback(window, keyCallback);

    while(!glfwWindowShouldClose(window)) {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();

        ++runningCounter;
    }

}
