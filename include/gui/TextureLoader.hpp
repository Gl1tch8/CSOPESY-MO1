#pragma once

#include <string>

// Forward declaration of OpenGL texture ID type
typedef unsigned int GLuint;

class TextureLoader {
public:
    static GLuint loadTexture(const std::string& filename, int& width, int& height);
};
