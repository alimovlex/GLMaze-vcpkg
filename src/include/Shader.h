#pragma once

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:
    unsigned int shaderProgram;
    // Cache for uniform locations to improve performance
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;

public:
    Shader(const char *vertexPath, const char *fragmentPath)
    {
        // retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (const std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ:: " << e.what() << std::endl;
        }
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();

        // compiles vertex shader from shader source code
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
        glCompileShader(vertexShader);
        checkShaderCompileStatus(vertexShader);

        // compiles fragment shader from shader source code
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
        glCompileShader(fragmentShader);
        checkShaderCompileStatus(fragmentShader);

        // creates shader program and attaches vertex and fragment shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        checkShaderProgramStatus(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    // Safe deletion of shader when class goes out of scope
    ~Shader()
    {
        glDeleteProgram(shaderProgram);
    }

    // Activates the shader program
    void use() const
    {
        glUseProgram(shaderProgram);
    }
    
    // The single, templated uniform setter to avoid multiple methods for GLSL Types
    template <typename T>
    void setUniform(const std::string &name, const T &value)
    {
        int location = getUniformLocation(name);

        // Evaluates types at compile time to optimise away checks at runtime
        if constexpr (std::is_same_v<T, bool>) {
            glUniform1i(location, static_cast<int>(value));
        }
        else if constexpr (std::is_same_v<T, int>) {
            glUniform1i(location, value);
        }
        else if constexpr (std::is_same_v<T, float>) {
            glUniform1f(location, value);
        }
        else if constexpr (std::is_same_v<T, glm::vec2>) {
            glUniform2fv(location, 1, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::vec3>) {
            glUniform3fv(location, 1, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::vec4>) {
            glUniform4fv(location, 1, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::mat3>) {
            glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::mat4>) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
        else {
            // This will produce a compile-time error if you try to use an unsupported type.
            static_assert(sizeof(T) == 0, "Unsupported uniform type used in Shader::setUniform");
        }
    }

private:
    // Caches uniform location to avoid getting it every frame
    int getUniformLocation(const std::string &name) const
    {
        // Check if we already have this location in our cache
        if (m_UniformLocationCache.contains(name))
        {
            return m_UniformLocationCache[name];
        }

        // If not, retrieve it from OpenGL
        const int location = glGetUniformLocation(shaderProgram, name.c_str());
        if (location == -1)
        {
            std::cerr << "Warning: uniform '" << name << "' not found!" << std::endl;
        }

        // Add the location to our cache for future calls
        m_UniformLocationCache[name] = location;
        return location;
    }


    static void checkShaderCompileStatus(const unsigned int shader)
    {
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            int shaderType;
            std::string shaderName{"Unknown Shader"};
            glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);

            switch (shaderType)
            {
            case GL_VERTEX_SHADER:
                shaderName = "Vertex Shader";
                break;

            case GL_FRAGMENT_SHADER:
                shaderName = "Fragment Shader";
                break;
            default:
                // No need to do anything if the type is unknown
                break;
            }
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);

            std::cerr << shaderName << " failed to compile: " << infoLog << '\n';
        }
    }

    static void checkShaderProgramStatus(unsigned int program)
    {
        int success;
        char infoLog[512];
        // after the program is linked, we do a check to see if it's valid
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "Shader Program failed to link!: \n"
                      << infoLog << '\n';
        }
    }
};