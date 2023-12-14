#pragma once

#include <TFT_eSPI.h>
#include <TouchScreen.h>
#include <list>
#include "stuff.hpp"

#define GAUGE_BG TFT_DARKGREY

class Datum
{
    String label;
    const double min, max;
    uint32_t color;
    std::list<double> history;

public:
    double value;
    int x, y;
    int radius;
    Datum(String label, int x, int y, double min, double max, uint32_t color, int radius);
    void update(double num);
    void move(int x, int y);
    void draw(TFT_eSPI& tft);
    /**
     * takes up whole screen, shows graph and current value
    */
    void drawDetailed(TFT_eSPI& tft);
};