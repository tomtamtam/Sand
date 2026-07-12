#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stb_image.h>
#include <vector>

#include "Renderer.h"
#include "defines.h"
#include "GLFW/glfw3.h"
#include "glm/ext/vector_float2.hpp"

auto renderer = std::make_shared<Renderer>(WIDTH, HEIGHT);

Pixel pixelBuffer[WIDTH][HEIGHT+1];
std::vector<SpawnPoint> spawnPoints;
std::vector<Pixel> activePixels;

bool leftMousePressed = false;
int penSize = 2;
bool pause = false;
PaintType paintType = P_OBSTACLE;

void clear(std::shared_ptr<Renderer> renderer, bool firstTime);
void evaluateMousePress(std::shared_ptr<Renderer> renderer);
void clearObsacles(std::shared_ptr<Renderer> renderer);
bool isMousePosValid(std::shared_ptr<Renderer> renderer);
glm::vec2 getLocalMousePos(std::shared_ptr<Renderer> renderer);
void drawBorders(std::shared_ptr<Renderer> renderer);
void calculatePixels(std::shared_ptr<Renderer> renderer);

//Key Callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
       penSize ++;
       std::cout << "penSize: " << penSize << '\n';
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && penSize > 1)
    {
       penSize --;
       std::cout << "penSize: " << penSize << '\n';
    }

    if(key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        pause = true;
        clear(renderer, false);
        pause = false;
    }

    if(key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        pause = true;
        clearObsacles(renderer);
        pause = false;
    }

    if(key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        paintType = P_SAND;
    }

    if(key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        paintType = P_SPAWN;
    }

    if(key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        paintType = P_OBSTACLE;
    }
}

//Mose Button Callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(paintType == P_SPAWN && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if(!isMousePosValid(renderer))
            return;

        auto pos = getLocalMousePos(renderer);
        SpawnPoint p = {(int)pos.x, (int)pos.y, penSize, 0.5f};
        spawnPoints.push_back(p);
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        leftMousePressed = true;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        leftMousePressed = false;
}

int main()
{
    renderer->SetKeyCallback(key_callback);
    renderer->SetMouseButtonCallback(mouse_button_callback);
    renderer->Init();

    clear(renderer, true);

    SpawnPoint p = {WIDTH / 2, HEIGHT, 1, 0.5};
    spawnPoints.push_back(p);

    //main loop
    while (!renderer->ShouldClose())
    {
        if(pause)
            continue;

        //input
        if(leftMousePressed)
        {
            evaluateMousePress(renderer);
        }

        calculatePixels(renderer);

        renderer->Draw();
    }

    renderer->Terminate();
}

void clear(std::shared_ptr<Renderer> renderer, bool firstTime)
{
    for(int x = 0; x < WIDTH; x++)
    {
        for(int y = 0; y < WIDTH; y++)
        {
            pixelBuffer[x][y] = {AIR, x, y};
            if(!firstTime)
                renderer->SetPixel(x, y, AIR);
        }
    }

    drawBorders(renderer);

    activePixels.clear();
}

void clearObsacles(std::shared_ptr<Renderer> renderer)
{
    for(int x = 0; x < WIDTH; x++)
    {
        for(int y = 0; y < WIDTH; y++)
        {
            if(pixelBuffer[x][y].type == OBSTACLE)
            {
                pixelBuffer[x][y].type = AIR;
                renderer->SetPixel(x, y, AIR);
            }
        }
    }

    drawBorders(renderer);

    for(int x = 0; x < WIDTH; x++)
    {
        for(int y = 0; y < WIDTH; y++)
        {
            if(pixelBuffer[x][y].type == SAND && pixelBuffer[x][y].sleep)
            {
                if(pixelBuffer[x][y-1].type == AIR || pixelBuffer[x-1][y-1].type == AIR || pixelBuffer[x][y-1].type == AIR)
                {
                    pixelBuffer[x][y].sleep = false;
                    activePixels.push_back(pixelBuffer[x][x]);
                    pixelBuffer[x][y].type = AIR;
                    renderer->SetPixel(x, y, AIR);
                }
            }
        }
    }
}

bool isMousePosValid(std::shared_ptr<Renderer> renderer)
{
    if(getLocalMousePos(renderer).x == -1)
        return false;

    return true;
}

glm::vec2 getLocalMousePos(std::shared_ptr<Renderer> renderer)
{

    float mouseX = renderer->GetMouseX();
    float mouseY = renderer->GetMouseY();

    auto texSize = renderer->GetTextureSize();
    auto winSize = renderer->GetWindowSize();

    float offsetX = 0.0f;
    float offsetY = winSize.y - texSize.y;

    float localX = mouseX - offsetX;
    float localY = mouseY - offsetY;

    if (localX < 0 || localX > texSize.x || localY < 0 || localY > texSize.y)
        return {-1, -1};

    int posx    = (int)(localX * ((float)WIDTH  / texSize.x));
    int posyTop = (int)(localY * ((float)HEIGHT / texSize.y));

    int posy = HEIGHT - 1 - posyTop;

    posx = std::clamp(posx, 0, WIDTH  - 1);
    posy = std::clamp(posy, 0, HEIGHT - 1);
    return {posx, posy};
}

void evaluateMousePress(std::shared_ptr<Renderer> renderer)
{
    if(!isMousePosValid(renderer))
        return;

    auto pos = getLocalMousePos(renderer);

    switch (paintType)
    {
    case P_OBSTACLE:
    {
        for(int i = 0; i < penSize; i++)
        {
            if(pos.x + i >= WIDTH)
                break;
            for(int j = 0; j < penSize; j++)
            {
                if(pos.y + j >= HEIGHT)
                    break;
                pixelBuffer[(int)pos.x + i][(int)pos.y + j].type = OBSTACLE;
                renderer->SetPixel(pos.x + i, pos.y + j, OBSTACLE);
            }
        }
        break;
    }
    case P_SAND:
    {
        for(int i = 0; i < penSize; i++)
        {
            if(pos.x + i >= WIDTH)
                break;

            if(pixelBuffer[(int)pos.x + i][(int)pos.y].type != AIR)
                continue;

            pixelBuffer[(int)pos.x + i][(int)pos.y].type = SAND;
            pixelBuffer[(int)pos.x + i][(int)pos.y].sleep = false;
            renderer->SetPixel(pos.x + i, pos.y, SAND);
            activePixels.push_back(pixelBuffer[(int)pos.x + i][(int)pos.y]);
        }
        break;
    }
    default:
        return;
    }
}

void drawBorders(std::shared_ptr<Renderer> renderer)
{
    for(int x = 0; x < WIDTH; x++)
    {
        pixelBuffer[x][0].type = OBSTACLE;
        renderer->SetPixel(x, 0, OBSTACLE);
    }

    for(int y = 0; y < WIDTH; y++)
    {
        pixelBuffer[0][y].type = OBSTACLE;
        renderer->SetPixel(0, y, OBSTACLE);
        pixelBuffer[WIDTH - 1][y].type = OBSTACLE;
        renderer->SetPixel(WIDTH - 1, y, OBSTACLE);
    }
}

void calculatePixels(std::shared_ptr<Renderer> renderer)
{
    std::cout << "calxc";
    //calculating pixels
    if(spawnPoints.size() == 0)
    {
        renderer->Draw();
        return;
    }

        for(int i = 0; i < spawnPoints.size(); i++)
        {
            SpawnPoint point = spawnPoints.at(i);
            Pixel p = {SAND, point.x, point.y, false};
            int perc = std::rand() % 100;
            if(perc <= point.frequency * 100)
            {
                pixelBuffer[point.x][point.y] = p;
                activePixels.push_back(p);
                renderer->SetPixel(point.x, point.y, SAND);
            }
        }

        std::vector<int> toDelete;

        for(int i = 0; i < activePixels.size(); i++)
        {
            Pixel p = activePixels.at(i);
            if(pixelBuffer[p.x][p.y-1].type == AIR && !p.sleep)
            {
                activePixels.at(i).y --;
                pixelBuffer[p.x][p.y] = {AIR, p.x, p.y, true};
                pixelBuffer[p.x][p.y-1] = {SAND, p.x, p.y - 1, false};
                renderer->SetPixel(p.x, p.y, AIR);
                renderer->SetPixel(p.x, p.y-1, SAND);
            }
            else if(pixelBuffer[p.x][p.y-1].type == OBSTACLE || (pixelBuffer[p.x][p.y-1].type == SAND && pixelBuffer[p.x][p.y-1].sleep))
            {
                bool left  = (pixelBuffer[p.x-1][p.y-1].type == OBSTACLE || pixelBuffer[p.x-1][p.y-1].type == SAND);
                bool right = (pixelBuffer[p.x+1][p.y-1].type == OBSTACLE || pixelBuffer[p.x+1][p.y-1].type == SAND);
                bool dirLeft = (pixelBuffer[p.x-1][p.y].type == OBSTACLE || pixelBuffer[p.x-1][p.y].type == SAND);
                bool dirRight = (pixelBuffer[p.x+1][p.y].type == OBSTACLE || pixelBuffer[p.x+1][p.y].type == SAND);
                if(left && right)
                {
                    if(pixelBuffer[p.x-1][p.y-1].type == SAND && !pixelBuffer[p.x-1][p.y-1].sleep || pixelBuffer[p.x+1][p.y-1].type == SAND && !pixelBuffer[p.x+1][p.y-1].sleep)
                    {
                        continue;
                    }
                    pixelBuffer[p.x][p.y].sleep = true;
                    toDelete.push_back(i);
                    continue;
                }

                if(left && !dirRight)
                {
                    activePixels.at(i).x++;
                    activePixels.at(i).y--;
                    pixelBuffer[p.x][p.y].type = AIR;
                    pixelBuffer[p.x+1][p.y-1].type = SAND;
                    renderer->SetPixel(p.x, p.y, AIR);
                    renderer->SetPixel(p.x+1, p.y-1, SAND);
                    continue;
                }

                if(right && !dirLeft)
                {
                    activePixels.at(i).x--;
                    activePixels.at(i).y--;
                    pixelBuffer[p.x][p.y].type = AIR;
                    pixelBuffer[p.x-1][p.y-1].type = SAND;
                    renderer->SetPixel(p.x, p.y, AIR);
                    renderer->SetPixel(p.x-1, p.y-1, SAND);
                    continue;
                }

                bool dir = std::rand() % 2 == 1;
                if(dir == LEFT && !dirRight)
                {
                    activePixels.at(i).x--;
                    activePixels.at(i).y--;
                    pixelBuffer[p.x][p.y].type = AIR;
                    pixelBuffer[p.x-1][p.y-1].type = SAND;
                    renderer->SetPixel(p.x, p.y, AIR);
                    renderer->SetPixel(p.x-1, p.y-1, SAND);
                    continue;
                }
                else if(!dirLeft)
                {
                    activePixels.at(i).x++;
                    activePixels.at(i).y--;
                    pixelBuffer[p.x][p.y].type = AIR;
                    pixelBuffer[p.x+1][p.y-1].type = SAND;
                    renderer->SetPixel(p.x, p.y, AIR);
                    renderer->SetPixel(p.x+1, p.y-1, SAND);
                    continue;
                }

                pixelBuffer[p.x][p.y].sleep = true;
                toDelete.push_back(i);
            }
        }
        if(toDelete.size() != 0)
        {
            std::sort(toDelete.begin(), toDelete.end(), std::greater<int>());
            for(int i = 0; i < toDelete.size(); i++)
            {
                activePixels.erase(activePixels.begin() + toDelete.at(i));
            }
        }
}
