#pragma once

#include <TFT_eSPI.h>
#include <TouchScreen.h>
#include <list>
#include "stuff.hpp"

class Datum
{
    const static uint32_t GAUGE_BG = TFT_DARKGREY;

    String label;
    uint32_t color;
    std::list<double> history;
    Point pos;
    int radius;
    double value = 0;
    double min, max;
    int precision;
    // TFT_eSPI& tft;
    // TFT_eSprite spr;

public:
    // Datum(const String& label, Point p, uint32_t color, int radius, double min, double max, TFT_eSPI& tft) 
    //     : label(label), pos(p), color(color), radius(radius), min(min), max(max), tft(tft), spr(&tft) {
    //         spr.setColorDepth(8);
    //         spr.setTextColor(TFT_WHITE, GAUGE_BG);
    //     }
    // Datum(const String& label, uint32_t color, int radius, double min, double max, TFT_eSPI& tft)
    //     : Datum(label, {0, 0}, color, radius, min, max, tft) {}
    // Datum(const String& label, uint32_t color, double min, double max, TFT_eSPI& tft)
    //     : Datum(label, color, 0, min, max, tft) {}
    
    Datum(const String& label, Point p, uint32_t color, int radius, double min, double max, int prec = 1) 
        : label(label), pos(p), color(color), radius(radius), min(min), max(max), precision(prec) {}
    Datum(const String& label, uint32_t color, int radius, double min, double max, int p = 1)
        : Datum(label, {0, 0}, color, radius, min, max, p) {}
    Datum(const String& label, uint32_t color, double min, double max, int p = 1)
        : Datum(label, color, 0, min, max, p) {}

    void update(double new_val);
    void move(Point p) { pos = p; }
    void resize(int r) { radius = r; }
    void draw(TFT_eSPI& tft);
    /**
     * takes up whole screen, shows graph and current value
    */
    void drawDetailed(TFT_eSPI& tft);
    // checks if a point is within the circle of this datum
    bool contains(Point p);
};