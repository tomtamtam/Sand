#include "Renderer.h"
#include <GLFW/glfw3.h>
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <ctime>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Renderer::Renderer(int width, int height)
    : m_Width(width), m_Height(height), m_Layout(), m_WinSize(width, height), m_UpdateTextureGPU(false)
{
    glfwSwapInterval(1);
    std::srand(std::time({}));
    m_Data = new unsigned char[width * height * RGBA];
    glfwSetErrorCallback([](int error, const char *desc) {
        std::cerr << "GLFW Error " << error << ": " << desc << '\n';
    });

    if (!glfwInit()) {
        assert(false && "Error: Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    m_Window = glfwCreateWindow(m_Width, m_Height, "sand", nullptr, nullptr);

    if (m_Window == NULL)
    {
        glfwTerminate();
        assert(false && "Error: failes to create GLFW Window");
    }
    glfwMakeContextCurrent(m_Window);

    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window,[](GLFWwindow *window, int width, int height)
        {
            auto* self = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
            self->m_Proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
            float s = (float)((width >= height)? height : width);
            self->m_Size = {s, s};
            glViewport(0, 0, width, height);
            self->m_WinSize = {width, height};
        }
    );

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
       glfwTerminate();
       assert(false && "Error: failed to load GLAD");
    }

    m_Proj = glm::ortho(0.0f, (float)m_Width, 0.0f, (float)m_Height, -1.0f, 1.0f);
    glViewport(0.0f, 0.0f, m_Width, m_Height);
}

void Renderer::SetPixel(int x, int y, Type type)
{
    size_t offset = (m_Width * y + x) * RGBA;
    Color c = colorMap.at(type);
    if(type == SAND)
    {
        c = getColorVariation(c);
    }
    for(int i = 0; i < RGBA; i++)
    {
        m_Data[offset + i] = c[i];
    }
    m_UpdateTextureGPU = true;
}

void Renderer::Draw()
{
    glfwPollEvents();
    glfwGetCursorPos(m_Window, &m_MouseX, &m_MouseY);
    glClearColor((float)C_GREY.R / 255, (float)C_GREY.G / 255, (float)C_GREY.B / 255, (float)C_GREY.A / 255);
    glClear(GL_COLOR_BUFFER_BIT);

    if(m_UpdateTextureGPU)
    {
        UpdateTextureGPU();
        m_UpdateTextureGPU = false;
    }

    m_Shader->Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    m_Shader->SetUniform1i("u_Texture", 0);

    m_Shader->SetMatrix4("u_Proj", m_Proj);

    m_Shader->SetUniform2f("u_Size", m_Size.x, m_Size.y);

    m_Vao->Bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glfwSwapBuffers(m_Window);

    if(m_Recording)
    {
        Record();
    }
}

void Renderer::Init()
{
    float vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    m_Vao.emplace();
    m_Vbo.emplace(vertices, sizeof(vertices));
    m_Layout.Push<float>(2);
    m_Vao->AddBuffer(*m_Vbo, m_Layout);

    m_Shader.emplace("../res/main.glsl");
    m_Shader->SetMatrix4("u_Proj", m_Proj);
    m_Shader->Bind();

    //data
    for(int y = 0; y < m_Height; y++)
    {
        for(int x = 0; x < m_Width; x++)
        {
            size_t offset = (y * m_Width + x) * RGBA;
            Color c = colorMap.at(AIR);
            for(int i = 0; i < RGBA; i++)
            {
                m_Data[offset + i] = c[i];
            }
        }
    }

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0,  GL_RGBA, GL_UNSIGNED_BYTE, m_Data);

}

Renderer::~Renderer()
{
    delete [] m_Data;
}

bool Renderer::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

void Renderer::Terminate()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Renderer::SetKeyCallback(GLFWkeyfun func)
{
    glfwSetKeyCallback(m_Window, func);
}


void Renderer::SetMouseButtonCallback(GLFWmousebuttonfun func)
{
    glfwSetMouseButtonCallback(m_Window, func);
}

double Renderer::GetMouseX() const
{
    return m_MouseX;
}

double Renderer::GetMouseY() const
{
    return m_MouseY;
}

glm::vec2 Renderer::GetTextureSize() const
{
    return m_Size;
}

glm::vec2 Renderer::GetWindowSize() const
{
    return m_WinSize;
}

void Renderer::UpdateTextureGPU() const
{
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, m_Data);
}

std::string getTime()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
    auto str = oss.str();
    return str;
}

void Renderer::StartRecording(OutputFormat format)
{
    m_RecordingFormat = format;
    m_Recording = true;
    m_RecordingCount = 0;
    m_RecordingName = "Recording " + getTime();
    std::filesystem::create_directories("./output/" + m_RecordingName);
    std::cout << "Started Recording!\n";
}

void removeFilesN(const std::string& dir, const std::string &extension)
{
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            std::filesystem::remove(entry.path());
        }
    }
}

void Renderer::StopRecording()
{
    m_Recording = false;
    std::cout << "Stopped Recording!\n";

    std::string dir = "./output/" + m_RecordingName;
    std::string cmd;

    switch (m_RecordingFormat)
    {
    case MP4:
        cmd = "ffmpeg -y -framerate 60 -i \"" + dir + "/%d.bmp\" "
              "-c:v libx264 -pix_fmt yuv420p \"" + dir + "/out.mp4\"";
        break;

    case GIF:
        cmd = "ffmpeg -y -framerate 60 -start_number 1 -i \"" + dir + "/%d.bmp\" "
              "-loop 0 \"" + dir + "/out.gif\"";
        break;
    }

    std::system(cmd.c_str());
    removeFilesN(dir, ".bmp");
}

void Renderer::Record()
{
    std::string filename = "./output/" + m_RecordingName + "/" + std::to_string(m_RecordingCount) + ".bmp";
    stbi_flip_vertically_on_write(1);
    stbi_write_bmp(filename.c_str(), m_Width, m_Height, 4, m_Data);
    m_RecordingCount ++;
}

bool Renderer::IsRecording() const
{
    return m_Recording;
}
