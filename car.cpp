#include "car.h"
#include "Engine.h"


car::car(int x, int y)
{
    this->x = x;
    this->y = y;
    this->default_x = x;
    this->default_y = y;
}

int car::get_x()
{
    return this->x;
}

int car::get_y()
{
    return this->y;
}

void car::set_x(int x)
{
    this->x = x;
}

void car::set_y(int y)
{
    this->y = y;
}

void car::draw()
{

    // car position
    int carX = this->x;
    int carY = this->y;

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

void car::act(float dt, int road_top, int road_bottom)
{
    const float speed = 300.0f;
    int step = (int)(speed * dt);

    if (is_key_pressed('W')) this->y -= step;
    if (is_key_pressed('S')) this->y += step;
    if (is_key_pressed('A')) this->x -= step;
    if (is_key_pressed('D')) this->x += step;

    // ораничение машинки экраном
    if (this->x < 0)                   this->x = 0;
    if (this->x > SCREEN_WIDTH - 144)  this->x = SCREEN_WIDTH - 144;

    // и дорогой
    if (this->y - 30 < road_top)     this->y = road_top + 30;
    if (this->y + 80 > road_bottom)  this->y = road_bottom - 80;
}