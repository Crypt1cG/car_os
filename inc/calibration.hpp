#pragma once

#include <TouchScreen.h>
#include <TFT_eSPI.h>
#include <utility>
#include <array>

// some decent values 
double xAxisSlope = 0.6;
double xAxisIntercept = -90;
double yAxisSlope = -0.55; // negative because direction is flipped
double yAxisIntercept = 425;

void drawCrosshair(TFT_eSPI& tft, TouchScreen& ts, int x, int y);
void calibrate(TFT_eSPI& tft, TouchScreen& ts);
/**
 * takes raw input from touchscreen and transforms it to actual screen coords
*/
std::pair<int, int> touchscreenAdjust(std::pair<int, int> point);