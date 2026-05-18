#include "Texture.h"
#include <stb_image.h>
#include <iostream>

Texture::Texture(const char* path, GLuint unit) : m_Unit(unit) {
    std::cout << "Carregando imagem \"" << path << "\"... ";
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 3);

    if (data == NULL) {
        std::cerr << "ERROR: Cannot open image file \"" << path << "\"." << std::endl;
        return;
    }

    std::cout << "OK (" << width << "x" << height << ")." << std::endl;

    glGenTextures(1, &m_ID);
    glGenSamplers(1, &m_SamplerID);

    glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    glActiveTexture(GL_TEXTURE0 + m_Unit);
    glBindTexture(GL_TEXTURE_2D, m_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(m_Unit, m_SamplerID);

    stbi_image_free(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_ID);
    glDeleteSamplers(1, &m_SamplerID);
}

void Texture::Bind() const {
    glActiveTexture(GL_TEXTURE0 + m_Unit);
    glBindTexture(GL_TEXTURE_2D, m_ID);
    glBindSampler(m_Unit, m_SamplerID);
}

void Texture::SetWrapping(GLint mode) const {
    glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, mode);
    glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, mode);
}
