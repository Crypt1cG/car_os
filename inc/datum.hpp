#pragma once

#include <TFT_eSPI.h>
#include <TouchScreen.h>
#include <list>
#include "stuff.hpp"
#include "data_stream.hpp"

class Datum
{
    const static uint32_t GAUGE_BG = TFT_DARKGREY;

    String label;
    uint32_t color;
    std::list<double> history;
    DataStream* ds;
    Point pos;
    int radius;

public:
    // removed in favor of just using ds
    // NOTE: this might be slower (we have to do a dereference and stuff to get
    // the value if we just use ds)
    // double value;
    Datum(String label, Point p, uint32_t color, int radius, DataStream* ds) 
        : label(label), pos(p), color(color), radius(radius), ds(ds) {}
    Datum(String label, uint32_t color, int radiusl, DataStream* ds)
        : Datum(label, {0, 0}, color, radius, ds) {}
    Datum(String label, uint32_t color, DataStream* ds)
        : Datum(label, color, 0, ds) {}

    // NOTE: should probably make big three to manage datastream bc it's dynamic

    void update();
    void move(Point p);
    void set_radius(int r) { radius = r; };
    void draw(TFT_eSPI& tft);
    /**
     * takes up whole screen, shows graph and current value
    */
    void drawDetailed(TFT_eSPI& tft);
    // checks if a point is within the circle of this datum
    bool contains(Point p);
};