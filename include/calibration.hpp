#pragma once

#include <TouchScreen.h>
#include <TFT_eSPI.h>
#include <utility>
#include <array>
#include "stuff.hpp"

extern double xAxisSlope;
extern double xAxisIntercept;
extern double yAxisSlope;
extern double yAxisIntercept;

void drawCrosshair(TFT_eSPI& tft, TouchScreen& ts, int x, int y);
void calibrate(TFT_eSPI& tft, TouchScreen& ts);
/**
 * takes raw input from touchscreen and transforms it to actual screen coords
*/
Point touchscreenAdjust(Point point);