#pragma once
#include "datum.hpp"
#include <vector>

class GUI {
    std::vector<Datum> data;
    int data_size = 4;
    // used for enlarging one datum
    int focused = -1;
    TFT_eSPI& tft;
    bool menu_showing = false;
    int to_change;
    int which_datum(Point p);

public:
    std::array<Data, 4> selected_data;
    GUI(TFT_eSPI& tft_in) : tft(tft_in) {};
    // void fill_data(const std::array<Datum, 4>& data_in);
    void fill_data(Datum* data_in);
    void draw();
    void update(float val, int i);
    // handle touch input (already properly transformed and such)
    void on_touch(Point p);
    void show_menu(Point p);
};