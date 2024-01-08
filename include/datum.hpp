#pragma once

#include <TFT_eSPI.h>
#include <TouchScreen.h>
#include <list>
#include "stuff.hpp"
#include <map>
#include <tuple>

class Datum
{
    const static std::map<const String, 
        std::tuple<uint32_t, double, double, float (ELM327::*)(), int>> default_configs;

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

    // class Getter {
    //     ELM327& elm;
    // public:
    //     virtual float operator()() = 0;
    // };
    // const Getter& getter;

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

    Datum(const String& label, Point p, uint32_t color, int radius, double min,
          double max, float (ELM327::*g)(), int prec = 1)
        : label(label), pos(p), color(color), radius(radius), min(min), 
          max(max), getter(g), precision(prec) {}

    Datum(const String& label, uint32_t color, int radius, double min, double max, 
          float (ELM327::*g)(), int p = 1)
        : Datum(label, {0, 0}, color, radius, min, max, g, p) {}

    Datum(const String& label, uint32_t color, double min, double max, float (ELM327::*g)(),
          int p = 1)
        : Datum(label, color, 0, min, max, g, p) {}

    Datum(const String& label) : Datum(label, std::get<0>(default_configs.at(label)),
        std::get<1>(default_configs.at(label)), std::get<2>(default_configs.at(label)),
        std::get<3>(default_configs.at(label)), std::get<4>(default_configs.at(label))) {}

    void update(double new_val);
    void move(Point p) { pos = p; }
    Point getPos() { return pos; }
    void resize(int r) { radius = r; }
    int getSize() { return radius; }
    void draw(TFT_eSPI& tft);
    /**
     * takes up whole screen, shows graph and current value
    */
    void drawDetailed(TFT_eSPI& tft);
    // checks if a point is within the circle of this datum
    bool contains(Point p);

    // might be a better way (not public)
    float (ELM327::*getter)();
};