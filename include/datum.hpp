#pragma once

#include <TFT_eSPI.h>
#include <TouchScreen.h>
#include <list>
#include "stuff.hpp"
#include <map>
#include <tuple>
#include <array>
#include <algorithm>
// #define TESTING

enum Data {
    RPM,
    SPEED,
    ENGINE_TEMP,
    THROTTLE,
    LOAD,
    TIMING_ADV,
    MAF_RATE,
    AIR_TEMP,
    MPG,
    RUN_TIME,
};

class Datum
{
    const static std::map<Data, 
        std::tuple<String, uint32_t, double, double, int>> default_configs;

    const static uint32_t GAUGE_BG = TFT_DARKGREY;
    const static int HIST_SIZE = 200;

    String label;
    uint32_t color;
    std::array<float, HIST_SIZE> history = {0};
    int curr_ind = 0;
    Point pos;
    int radius;
    double min, max;
    int precision;
    Data data_ind;
    // TFT_eSPI& tft;
    // TFT_eSprite spr;

    // class Getter {
    //     ELM327& elm;
    // public:
    //     virtual float operator()() = 0;
    // };
    // const Getter& getter;

public:
    static std::array<float, 10> data_arr;

    // Datum(const String& label, Point p, uint32_t color, int radius, double min, double max, TFT_eSPI& tft) 
    //     : label(label), pos(p), color(color), radius(radius), min(min), max(max), tft(tft), spr(&tft) {
    //         spr.setColorDepth(8);
    //         spr.setTextColor(TFT_WHITE, GAUGE_BG);
    //     }
    // Datum(const String& label, uint32_t color, int radius, double min, double max, TFT_eSPI& tft)
    //     : Datum(label, {0, 0}, color, radius, min, max, tft) {}
    // Datum(const String& label, uint32_t color, double min, double max, TFT_eSPI& tft)
    //     : Datum(label, color, 0, min, max, tft) {}

    Datum(Data d, Point p, uint32_t color, int radius, double min,
          double max, int prec = 1)
        : label(std::get<0>(default_configs.at(d))), pos(p), color(color), radius(radius), min(min), 
          max(max), precision(prec), data_ind(d) {}

    Datum(Data d, uint32_t color, int radius, double min, double max, 
          int p = 1)
        : Datum(d, {0, 0}, color, radius, min, max, p) {}

    Datum(Data d, uint32_t color, double min, double max,
          int p = 1)
        : Datum(d, color, 0, min, max, p) {}

    Datum(Data d) : Datum(d, std::get<1>(default_configs.at(d)),
        std::get<2>(default_configs.at(d)), std::get<3>(default_configs.at(d)),
        std::get<4>(default_configs.at(d))) {}

    void update();
    void move(Point p) { pos = p; }
    Point getPos() { return pos; }
    void resize(int r) { radius = r; }
    int getSize() { return radius; }
    Data getDataType() { return data_ind; }
    void draw(TFT_eSPI& tft);
    /**
     * takes up whole screen, shows graph and current value
    */
    void drawDetailed(TFT_eSPI& tft);
    // checks if a point is within the circle of this datum
    bool contains(Point p);
};