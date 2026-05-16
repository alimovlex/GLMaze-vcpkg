#pragma once
#include <glad/glad.h>
#include "ImageLoader.h"

struct TextureParameters
{
    int wrapS;
    int wrapT;
    int minFilter;
    int magFilter;
    //To be added later in advanced opengl
    //bool srgb;

    TextureParameters()
        : wrapS(GL_REPEAT),
          wrapT(GL_REPEAT),
          minFilter(GL_LINEAR_MIPMAP_LINEAR),
          magFilter(GL_LINEAR) {}

    explicit TextureParameters(int wrapS, int wrapT, int minFilter, int magFilter)
        : wrapS(wrapS),
          wrapT(wrapT),
          minFilter(minFilter),
          magFilter(magFilter) {}
};

class Texture
{
    std::string m_Filepath;
    unsigned int m_TextureID;
    int m_Width;
    int m_Height;

public:
    Texture &operator=(const Texture &) = delete;

    Texture(const Texture &) = delete;

    ~Texture()
    {
        glDeleteTextures(1, &m_TextureID);
    }

    explicit Texture(const std::string &path, const TextureParameters &params = TextureParameters())
        : m_Filepath(path), m_TextureID(0), m_Width(0), m_Height(0)

    {
        const ImageLoader img(path);
        if (!img.imageLoaded()) {
            std::cerr << "[Texture] Error: Failed to load image: " << path << std::endl;
            return;
        }
        m_Width = img.getWidth();
        m_Height = img.getHeight();

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrapT);

        if (img.getNrChannels() == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.getImage());
            glGenerateMipmap(GL_TEXTURE_2D);
        } else if (img.getNrChannels() == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getImage());
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "[Texture] Error: Unsupported image format: " << path << std::endl;
            glDeleteTextures(1, &m_TextureID);
            m_TextureID = 0;
            return;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void bindTexture(const unsigned int slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }

    int getWidth() const { return m_Width; }
    int getHeight() const { return m_Height; }
};
