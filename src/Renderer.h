#pragma once

#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <optional>
#include <unordered_map>

#include "Buffers/VertexArray.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/VertexBufferLayout.h"
#include "Shader.h"

#define RGBA 4
#define VARIATION 30

enum Type {
    AIR,
    SAND,
    OBSTACLE
};

struct Color
{
    int R, G, B, A;
    Color(int r, int g, int b, int a)
        : R(r), G(g), B(b), A(a)
    {}
    Color(int v)
        : R(v), G(v), B(v), A(v)
    {}
    inline int &operator[](int idx)
    {
        switch (idx)
        {
            case 0:
                return R;
                break;
            case 1:
                return G;
                break;
            case 2:
                return B;
                break;
            case 3:
                return A;
                break;
            default:
                return R;
                break;
        }
    }
    inline float *GetFloat() const
    {
        float *f = new float[4];
        f[0] = R;
        f[1] = G;
        f[2] = B;
        f[3] = A;
        return f;
    }
};

const Color C_WHITE(255);
const Color C_BACK(0);
const Color C_SAND(236, 203, 100, 255);
const Color C_GREY(25, 25, 25, 255);

inline Color getColorVariation(Color c)
{
    for(int i = 0; i < 4; i++)
    {
        int var = std::rand() % VARIATION;
        var -= VARIATION / 2;

        c[i] += var;
    }
    return c;
}

const std::unordered_map<Type, Color> colorMap =
{
    {AIR, C_GREY},
    {SAND, C_SAND},
    {OBSTACLE, C_WHITE}
};


class Renderer
{
public:
    void Draw();
    void SetPixel(int x, int y, Type type);
    void Init();
    bool ShouldClose() const;
    void Terminate();
    void SetKeyCallback(GLFWkeyfun func);
    void SetMouseButtonCallback(GLFWmousebuttonfun func);
    Renderer(int width, int height);
    double GetMouseX() const;
    double GetMouseY() const;
    glm::vec2 GetTextureSize() const;
    glm::vec2 GetWindowSize() const;
    ~Renderer();
private:
    unsigned char *m_Data;

    int m_Width, m_Height;
    GLFWwindow *m_Window;
    glm::vec2 m_Size;
    glm::vec2 m_WinSize;

    std::optional<VertexArray> m_Vao;
    std::optional<VertexBuffer> m_Vbo;
    std::optional<Shader> m_Shader;
    VertexBufferLayout m_Layout;
    glm::mat4 m_Proj;
    uint32_t m_Texture;
    double m_MouseX, m_MouseY;
};
