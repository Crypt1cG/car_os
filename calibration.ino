#include "inc/calibration.hpp"
#include "inc/stuff.hpp"

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
    std::array<std::pair<std::pair<int, int>, std::pair<int, int>>, 4> points = 
        {{ {{40, 40}, {}},      {{440, 40}, {}},
           {{440, 280}, {}},    {{40, 280}, {}} }};

    // NOTE: touchscreen x is actually the smaller dimension of the screen (y in this orientation),
    // so we swap the x and y coords
    for (int i = 0; i < 4; i++)
    {
        drawCrosshair(tft, ts, points[i].first.first, points[i].first.second);
        TSPoint p = ts.getPoint();
        while (p.z < ts.pressureThreshhold)
        {
            delay(30);
            p = ts.getPoint();
        }

        points[i].second = {p.y, p.x}; // note the coords are swapped
        tft.fillScreen(TFT_BLACK);

        Serial.println("Drawn at: (" + String(points[i].first.first) + ", " + 
                                       String(points[i].first.second) + 
                       "), touched at: (" + String(p.y) + ", " + String(p.x) + ")");
        delay(150);
    }

    // calculate parameters from data
    // second means recorded coords from touchscreen, first means x
    double avgXDelta = ((points[1].second.first - points[0].second.first) + 
                        (points[2].second.first - points[3].second.first)) / 2;
    xAxisSlope = 400.0 / avgXDelta;

    double avgYDelta = ((points[3].second.second - points[0].second.second) +
                        (points[2].second.second - points[1].second.second)) / 2;
    yAxisSlope = 240.0 / avgYDelta;

    // calculate intercepts for each point, then average
    double xSum = 0, ySum = 0;
    for (int i = 0; i < 4; i++)
    {
        // this comes from point-slope form
        xSum += points[i].first.first - xAxisSlope * points[i].second.first;
        ySum += points[i].first.second - yAxisSlope * points[i].second.second;
    }

    xAxisIntercept = xSum / 4.0;
    yAxisIntercept = ySum / 4.0;

    Serial.println("Calibration output: " + String(xAxisSlope) + ", " +
                                            String(xAxisIntercept) + ", " +
                                            String(yAxisSlope) + ", " +
                                            String(yAxisIntercept));
}

std::pair<int, int> touchscreenAdjust(std::pair<int, int> point)
{
    return {point.first * xAxisSlope + xAxisIntercept,
            point.second * yAxisSlope + yAxisIntercept};
}