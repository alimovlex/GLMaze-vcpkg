#pragma once
#include <filesystem>
#include <iostream>
#include <string>
#include <stb_image.h>

namespace fs = std::filesystem;

class ImageLoader
{
    std::string m_filepath;
    int m_width;
    int m_height;
    int m_nrChannels;
    unsigned char *m_data = nullptr;

public:
    ImageLoader() = default;

    ImageLoader(const ImageLoader &) = delete;

    ImageLoader &operator=(const ImageLoader &) = delete;

    explicit ImageLoader(const std::string &filepath)
    {
        m_filepath = filepath;
        loadImage(m_filepath);
    }

    ~ImageLoader()
    {
        unloadImage();
    }

    bool loadImage(const std::string &filepath)
    {
        unloadImage();

        if (!fs::exists(filepath) || !fs::is_regular_file(filepath)) {
            std::cerr << "(ERROR) ImageLoader: File does not exist : " << filepath << std::endl;
            return false;
        }
        m_filepath = filepath;

        stbi_set_flip_vertically_on_load(true);
        m_data = stbi_load(m_filepath.c_str(), &m_width, &m_height, &m_nrChannels, 0);

        if (m_data == nullptr) {
            std::cerr << "Failed to load image at: " << m_filepath << std::endl;
            m_filepath.clear();
            return false;
        }

        return true;
    }


    void unloadImage()
    {
        stbi_image_free(m_data);
        m_height = 0;
        m_width = 0;
        m_nrChannels = 0;
    }

    bool imageLoaded() const { return m_data != nullptr; }
    unsigned char *getImage() const { return m_data; }

    std::string getFilepath() const { return m_filepath; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getNrChannels() const { return m_nrChannels; }
};
