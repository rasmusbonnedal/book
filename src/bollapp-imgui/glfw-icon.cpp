#include "glfw-icon.h"

#include <GLFW/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool loadGlfwIcon(GLFWwindow* window, const std::string& filename) {
    GLFWimage icons[1];
    int comp;
    icons[0].pixels = stbi_load(filename.c_str(), &icons[0].width, &icons[0].height, &comp, 0);
    if (icons[0].pixels == 0) {
        std::cout << "Error loading " << filename << std::endl;
        return false;
    }
    if (comp != 4) {
        std::cout << "Error: Only RGBA images supported as icon" << std::endl;
        stbi_image_free(icons[0].pixels);
    }
    glfwSetWindowIcon(window, 1, icons);
    stbi_image_free(icons[0].pixels);
    return true;
}
