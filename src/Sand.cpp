#include <algorithm>
#include <cstdlib>
#include <memory>
#include <stb_image.h>
#include <vector>

#include "Renderer.h"

#define WIDTH 200
#define HEIGHT 200

#define LEFT true
#define RIGHT false
struct Pixel
{
    Type type;
    int x, y;
    bool sleep;
};

struct SpawnPoint
{
    int x, y;
    int radius;
    float frequency;
};

Pixel pixelBuffer[WIDTH][HEIGHT+1];
std::vector<SpawnPoint> spawnPoints;
std::vector<Pixel> activePixels;

int main()
{
    auto renderer = std::make_shared<Renderer>(WIDTH, HEIGHT);
    renderer->Init();

    //init buffer
    for(int x = 0; x < WIDTH; x++)
    {
        for(int y = 0; y < WIDTH; y++)
        {
            pixelBuffer[x][y] = {AIR, x, y};
        }
    }
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

    SpawnPoint p = {WIDTH / 2, HEIGHT, 1, 0.5};
    spawnPoints.push_back(p);

    //main loop
    while (!renderer->ShouldClose())
    {
        if(spawnPoints.size() == 0)
        {
            renderer->Draw();
            continue;
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
            else if(pixelBuffer[p.x][p.y-1].type == OBSTACLE ||pixelBuffer[p.x][p.y-1].type == SAND)
            {
                bool left  = (pixelBuffer[p.x-1][p.y-1].type == OBSTACLE || pixelBuffer[p.x-1][p.y-1].type == SAND);
                bool right = (pixelBuffer[p.x+1][p.y-1].type == OBSTACLE || pixelBuffer[p.x+1][p.y-1].type == SAND);
                if(left && right)
                {
                    if(pixelBuffer[p.x-1][p.y-1].type == SAND && !pixelBuffer[p.x-1][p.y-1].sleep || pixelBuffer[p.x+1][p.y-1].type == SAND && !pixelBuffer[p.x+1][p.y-1].sleep)
                    {
                        continue;
                    }
                    p.sleep = true;
                    toDelete.push_back(i);
                    continue;
                }

                if(left)
                {
                    activePixels.at(i).x++;
                    activePixels.at(i).y--;
                    pixelBuffer[p.x][p.y].type = AIR;
                    pixelBuffer[p.x+1][p.y-1].type = SAND;
                    renderer->SetPixel(p.x, p.y, AIR);
                    renderer->SetPixel(p.x+1, p.y-1, SAND);
                    continue;
                }

                if(right)
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
                if(dir == LEFT)
                {
                    activePixels.at(i).x--;
                    activePixels.at(i).y--;
                    pixelBuffer[p.x][p.y].type = AIR;
                    pixelBuffer[p.x-1][p.y-1].type = SAND;
                    renderer->SetPixel(p.x, p.y, AIR);
                    renderer->SetPixel(p.x-1, p.y-1, SAND);
                    continue;
                }
                else
                {
                    activePixels.at(i).x++;
                    activePixels.at(i).y--;
                    pixelBuffer[p.x][p.y].type = AIR;
                    pixelBuffer[p.x+1][p.y-1].type = SAND;
                    renderer->SetPixel(p.x, p.y, AIR);
                    renderer->SetPixel(p.x+1, p.y-1, SAND);
                    continue;
                }
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

        renderer->Draw();
    }

    renderer->Terminate();
}
