/* MIT License
 * 
 * Copyright (c) 2026 Voronezh State University
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define _DEFAULT_SOURCE

#include "Engine.h"
#include <stdlib.h>
#include <memory.h>
#include <sys/time.h>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <ctime>
#include <ctime>
#include <cstdarg>
#include <cstdio>

// car position and speed
static float carX = 440.0f;
static float carY = 540.0f;
static const float CAR_SPEED = 300.0f;

// initialize game data in this function
void initialize()
{
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
    if (is_key_pressed('W')) carY -= CAR_SPEED * dt;
    if (is_key_pressed('S')) carY += CAR_SPEED * dt;
    if (is_key_pressed('A')) carX -= CAR_SPEED * dt;
    if (is_key_pressed('D')) carX += CAR_SPEED * dt;

    // ограничения, чтобы машинка за экран не выехала
    if (carX < 0)                   carX = 0;
    if (carX > SCREEN_WIDTH - 144) carX = SCREEN_WIDTH - 144;

    // Или только вперед назад кататься с фиксированной машинкой по "y" в правой полосе
    //carY = 610;
}

// fill buffer in this function
// uint32_t buffer[SCREEN_WIDTH][SCREEN_HEIGHT] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
    // sky background
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            buffer[i][j] = 0x4477aa;
        }
    }

    // road
    int roadY = 580;
    for (int j = 0; j < SCREEN_WIDTH; j++)
    {
        for (int i = roadY; i < roadY + 120; i++)
        {
            if (i < SCREEN_HEIGHT)
                buffer[i][j] = 0x555555;
        }
    }

    // road lines
    for (int j = 0; j < SCREEN_WIDTH; j += 80)
    {
        for (int dj = 0; dj < 40; dj++)
        {
            if (j + dj < SCREEN_WIDTH)
                buffer[roadY + 60][j + dj] = 0xffffff;
        }
    }

    int cx = (int)carX;
    int cy = (int)carY;

    // car body (red)
    for (int i = carY; i < carY + 60 && i < SCREEN_HEIGHT; i++)
    {
        for (int j = carX; j < carX + 144; j++)
        {
            if (j < SCREEN_WIDTH)
                buffer[i][j] = 0xcc2222;
        }
    }

    // car top (cabin)
    for (int i = carY - 30; i < carY && i >= 0; i++)
    {
        for (int j = carX + 24; j < carX + 120; j++)
        {
            if (j < SCREEN_WIDTH)
                buffer[i][j] = 0xcc2222;
        }
    }

    // windows (light blue)
    for (int i = carY - 28; i < carY - 5; i++)
    {
        for (int j = carX + 30; j < carX + 65; j++)
        {
            if (j < SCREEN_WIDTH)
                buffer[i][j] = 0x88ccff;
        }
    }
    for (int i = carY - 28; i < carY - 5; i++)
    {
        for (int j = carX + 75; j < carX + 114; j++)
        {
            if (j < SCREEN_WIDTH)
                buffer[i][j] = 0x88ccff;
        }
    }

    // wheels (black)
    int wheelY = carY + 52;
    int wheelPositions[] = { carX + 16, carX + 100 };
    for (int w = 0; w < 2; w++)
    {
        for (int i = wheelY; i < wheelY + 28 && i < SCREEN_HEIGHT; i++)
        {
            for (int j = wheelPositions[w]; j < wheelPositions[w] + 24; j++)
            {
                if (j < SCREEN_WIDTH)
                    buffer[i][j] = 0x111111;
            }
        }
    }

    // wheel hubs (gray)
    int hubPositions[] = { carX + 22, carX + 106 };
    for (int w = 0; w < 2; w++)
    {
        for (int i = wheelY + 8; i < wheelY + 20; i++)
        {
            for (int j = hubPositions[w]; j < hubPositions[w] + 8; j++)
            {
                if (i < SCREEN_HEIGHT && j < SCREEN_WIDTH)
                    buffer[i][j] = 0x888888;
            }
        }
    }

    // headlights (yellow)
    for (int i = carY + 10; i < carY + 40; i++)
    {
        for (int j = carX + 138; j < carX + 144; j++)
        {
            if (i < SCREEN_HEIGHT && j < SCREEN_WIDTH)
                buffer[i][j] = 0xffff44;
        }
    }

    // taillights (red-orange)
    for (int i = carY + 10; i < carY + 40; i++)
    {
        for (int j = carX; j < carX + 6; j++)
        {
            if (i < SCREEN_HEIGHT && j < SCREEN_WIDTH)
                buffer[i][j] = 0xff4400;
        }
    }
}

// free game data in this function
void finalize()
{
}
