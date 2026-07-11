#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include <cstdint>
#include <string>

    struct Shader
    {
        uint32_t ShaderId;

        Shader(const std::string &shaderPath);
        virtual ~Shader();

        void Bind() const;
        void Unbind() const;

        void SetUniform4f(const std::string &name, float x, float y, float z, float w);
        void SetUniform2f(const std::string &name, float x, float y);
        void SetUniform1f(const std::string &name, float f);
        void SetUniform1i(const std::string &name, int i);
        void SetUniform1b(const std::string &name, bool value);
        void SetMatrix4(const std::string &name, glm::mat4 value);
    private:
        uint32_t Compile(const std::string &shaderSource, uint32_t type);
        std::string Parse(const char* filename);
        uint32_t CreateShader(const std::string &shaderPath);
    };
