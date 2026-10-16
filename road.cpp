#include "road.h"
#include "Engine.h"

road::road(int y, int height)
{
    this->y = y;
    this->height = height;
}

void road::draw()
{
    for (int j = 0; j < SCREEN_WIDTH; j++)
    {
        for (int i = y; i < y + height; i++)
        {
            if (i < SCREEN_HEIGHT)
                buffer[i][j] = 0x555555;
        }
    }

    int mid = y + height / 2;
    for (int j = 0; j < SCREEN_WIDTH; j += 80)
    {
        for (int dj = 0; dj < 40; dj++)
        {
            if (j + dj < SCREEN_WIDTH)
                buffer[mid][j + dj] = 0xffffff;
        }
    }
}