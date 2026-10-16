#pragma once


class car {
    int x, y;
    int default_x, default_y;

    car();
public:
    car(int x, int y);
    int get_x();
    int get_y();
    void set_x(int x);
    void set_y(int y);
    void draw();
    void act(float dt, int road_top, int road_bottom);
    ~car() {};
};