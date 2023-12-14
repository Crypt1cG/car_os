#pragma once

#include <TouchScreen.h>
#include <TFT_eSPI.h>
#include <vector>

// ###########################
// 		TOUCHSCREEN STUFF
// ###########################
#define YP 13 // Y+ pin
#define XM 32 // X- pin
#define XP 22 // X+ pin
#define YM 23 // Y- pin

// class used for storing a particular piece of data
struct Datum
{
    String label;
    const double min, max;
    double value;
};

class Display
{
public:
    TFT_eSPI tft = TFT_eSPI();
    TouchScreen ts = TouchScreen(XP, YP, XM, YM, 280);
    std::vector<Datum> data;
    void draw();
};