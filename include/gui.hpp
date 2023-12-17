#pragma once
#include "datum.hpp"
#include <vector>

class GUI {
    std::vector<Datum> data;
    int data_size = 4;
    // used for enlarging one datum
    int focused = -1;
    TFT_eSPI& tft;

public:
    GUI(TFT_eSPI& tft_in) : tft(tft_in) {};
    void fill_data(const std::array<Datum, 4>& data_in);
    void draw();
    void update(double val, Data d);
    // handle touch input (already properly transformed and such)
    void on_touch(Point p);
};