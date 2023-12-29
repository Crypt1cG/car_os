#include "../include/calibration.hpp"
#include "stuff.hpp"

// some decent values 
double xAxisSlope = 0.6;
double xAxisIntercept = -90;
double yAxisSlope = -0.55; // negative because direction is flipped
double yAxisIntercept = 425;

void drawCrosshair(TFT_eSPI& tft, TouchScreen& ts, int x, int y)
{
    tft.fillRect(x - 1, y - 12, 3, 10, TFT_WHITE);
    tft.fillRect(x - 1, y + 3, 3, 10, TFT_WHITE);
    tft.fillRect(x - 12, y - 1, 10, 3, TFT_WHITE);
    tft.fillRect(x + 3, y - 1, 10, 3, TFT_WHITE);
    tft.drawPixel(x, y, TFT_RED);
}

void calibrate(TFT_eSPI& tft, TouchScreen& ts)
{
    std::array<std::pair<Point, Point>, 4> points = 
        {{ {{40, 40}, {}},      {{440, 40}, {}},
           {{440, 280}, {}},    {{40, 280}, {}} }};

    // NOTE: touchscreen x is actually the smaller dimension of the screen (y in this orientation),
    // so we swap the x and y coords
    for (int i = 0; i < 4; i++)
    {
        drawCrosshair(tft, ts, points[i].first.x, points[i].first.y);
        TSPoint p = ts.getPoint();
        int time = 0;
        while (p.z < ts.pressureThreshhold)
        {
            delay(30);
            time += 30;
            if (time > 10000)
                return;
            p = ts.getPoint();
        }

        points[i].second = {p.y, p.x}; // note the coords are swapped
        tft.fillScreen(TFT_BLACK);

        Serial.println("Drawn at: (" + String(points[i].first.x) + ", " + 
                                       String(points[i].first.y) + 
                       "), touched at: (" + String(p.y) + ", " + String(p.x) + ")");
        delay(150);
    }

    // calculate parameters from data
    // second means recorded coords from touchscreen, first means x
    double avgXDelta = ((points[1].second.x - points[0].second.x) + 
                        (points[2].second.x - points[3].second.x)) / 2;
    xAxisSlope = 400.0 / avgXDelta;

    double avgYDelta = ((points[3].second.y - points[0].second.y) +
                        (points[2].second.y - points[1].second.y)) / 2;
    yAxisSlope = 240.0 / avgYDelta;

    // calculate intercepts for each point, then average
    double xSum = 0, ySum = 0;
    for (int i = 0; i < 4; i++)
    {
        // this comes from point-slope form
        xSum += points[i].first.x - xAxisSlope * points[i].second.x;
        ySum += points[i].first.y- yAxisSlope * points[i].second.y;
    }

    xAxisIntercept = xSum / 4.0;
    yAxisIntercept = ySum / 4.0;

    Serial.println("Calibration output: " + String(xAxisSlope) + ", " +
                                            String(xAxisIntercept) + ", " +
                                            String(yAxisSlope) + ", " +
                                            String(yAxisIntercept));
}

Point touchscreenAdjust(Point point)
{
    return {point.x * xAxisSlope + xAxisIntercept,
            point.y * yAxisSlope + yAxisIntercept};
}