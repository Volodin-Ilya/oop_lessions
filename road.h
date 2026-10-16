#pragma once

class road {
    int y;
    int height;

public:
    road(int y, int height);
    void draw();

    int get_top()    { return y; }
    int get_bottom() { return y + height; }

    ~road() {};
};