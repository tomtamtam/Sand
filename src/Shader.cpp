#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <glad/glad.h>

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

 Shader::Shader(const std::string &shaderPath)
{
    ShaderId = CreateShader(shaderPath);
}

Shader::~Shader()
{
    glDeleteProgram(ShaderId);
}

    void Shader::Bind() const
    {
        glUseProgram(ShaderId);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    void Shader::SetUniform4f(const std::string &name, float x, float y, float z, float w)
    {
        int location = glGetUniformLocation(ShaderId, name.c_str());
        glUniform4f(location, x, y, z, w);
    }

    void Shader::SetUniform1f(const std::string &name, float f)
    {
        int location = glGetUniformLocation(ShaderId, name.c_str());
        glUniform1f(location, f);
    }

    void Shader::SetUniform2f(const std::string &name, float x, float y)
    {
        int location = glGetUniformLocation(ShaderId, name.c_str());
        glUniform2f(location, x, y);
    }

    void Shader::SetUniform1i(const std::string &name, int i)
    {
        int location = glGetUniformLocation(ShaderId, name.c_str());
        glUniform1i(location, i);
    }

    void Shader::SetUniform1b(const std::string &name, bool value)
    {
        int location = glGetUniformLocation(ShaderId, name.c_str());
        glUniform1i(location, value);
    }

    void Shader::SetMatrix4(const std::string &name, glm::mat4 value)
    {
        int location = glGetUniformLocation(ShaderId, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    uint32_t Shader::Compile(const std::string &shaderSource, uint32_t type)
    {
        GLuint id = glCreateShader(type);
        const char* src = shaderSource.c_str();
        glShaderSource(id, 1, &src, 0);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if(result != GL_TRUE)
        {
            int length = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = new char[length];
            glGetShaderInfoLog(id, length, &length, message);
            std::cout << "error whilst compiling " << (type == GL_VERTEX_SHADER?  "vertex" : "fragment") << " shader: " << message << '\n';
            delete[] message;
            return 0;
        }

        return id;
    }

    std::string Shader::Parse(const char* fileName)
    {
        FILE* file;
        file = fopen(fileName, "rb");
        if(file == nullptr)
        {
            std::cout << "file " << fileName << " not found\n";
            return 0;
        }

        std::string contents;
        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);
        rewind(file);
        contents.resize(fileSize);
 
        fread(&contents[0], 1, fileSize, file);
        fclose(file);

        return contents;
    }

    uint32_t Shader::CreateShader(const std::string &shaderPath)
    {
        std::istringstream stream(Parse(shaderPath.c_str()));
        std::string line;

        std::string vertexShaderSource = "";
        std::string fragmentShaderSource = "";

        bool isActive = false;
        bool isVertex = true;
        while (std::getline(stream, line)) {
            if(line.starts_with("#define vertex"))
            {
                isActive = true;
                isVertex = true;
                continue;
            }
            else if(line.starts_with("#define fragment"))
            {
                isActive = true;
                isVertex = false;
                continue;
            }
            if(!isActive)
                continue;

            if(isVertex)
                vertexShaderSource += line + '\n';
            else
                fragmentShaderSource += line + '\n';
        }


        uint32_t program = glCreateProgram();
        uint32_t vs = Compile(vertexShaderSource, GL_VERTEX_SHADER);
        uint32_t fs = Compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        glDetachShader(program, vs);
        glDetachShader(program, fs);

        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }
