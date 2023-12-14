#include "inc/datum.hpp"

Datum::Datum(String label, int x, int y, double min, double max, uint32_t color, int radius) : min(min), max(max)
{
    this->label = label;
    this->x = x;
    this->y = y;
    this->color = color;
    this->radius = radius;
    value = 0;
}

void Datum::update(double num)
{
    history.push_front(num);
    if (history.size() > 200) history.pop_back();
    value = num;
}

void Datum::move(int x, int y)
{
    this->x = x;
    this->y = y;
}

void Datum::draw(TFT_eSPI& tft)
{
    TFT_eSprite spr = TFT_eSprite(&tft);
    spr.setColorDepth(16); // consider changing this for more speed?
    spr.setTextColor(TFT_WHITE, GAUGE_BG);
    // set font for number (could use smooth fonts, but it would be slower)
    spr.setFreeFont(&FreeMonoBold18pt7b);
    spr.createSprite(2 * radius, 2 * radius);
    // background around circle
    spr.fillSprite(TFT_BLACK);
    // gauge background circle
    spr.fillSmoothCircle(radius, radius, radius, GAUGE_BG);
    spr.drawSmoothArc(radius, radius, 0.95 * radius, 0.8 * radius,
                      45, 45 + ((value - min) * 1.0 / (max - min)) * 270,
                      color, GAUGE_BG);
    // draw the number
    spr.setTextDatum(BC_DATUM);
    spr.drawString(String(value), radius, radius);
    
    // draw the label text
    spr.setTextDatum(TC_DATUM);
    spr.drawString(label, radius, radius);
    
    spr.pushSprite(x - radius, y - radius);
    spr.deleteSprite();
}

void Datum::drawDetailed(TFT_eSPI& tft)
{
    // the graph will be 200 px wide and 260 px tall
    TFT_eSprite spr = TFT_eSprite(&tft);
    spr.setColorDepth(8);
    spr.setTextColor(TFT_WHITE, TFT_BLACK);
    // font for axis labels
    spr.setFreeFont(&FreeMono9pt7b);
    // we can't do the whole screen at once cause it's too much memory??
    spr.createSprite(300, SCREEN_HEIGHT);
    spr.fillSprite(TFT_BLACK);
    // draw axes
    spr.drawFastHLine(50, 270, 200, TFT_WHITE);
    spr.drawFastVLine(50, 10, 260, TFT_WHITE);
    for (int i = 0; i < 10; i++)
    {
        // tickmark on y axis
        spr.drawLine(45, 10 + i * 26, 55, 10 + i * 26, TFT_WHITE);
        spr.setTextDatum(MR_DATUM);
        spr.drawString(String((int)(min + (10 - i) * (max - min) / 10)), 45, 10 + i * 26);
        // tickmark on x axis
        spr.drawLine(250 - i * 20, 265, 250 - i * 20, 275, TFT_WHITE);
        spr.setTextDatum(TC_DATUM);
        if (i % 2 == 0)
            spr.drawString(String((10 - i) * 20), 250 - i * 20, 275);
    }

    int n = 0;
    std::list<double>::iterator it = history.end();
    it--; // end() returns an iterator pointing to one past the end
    while (n < history.size() - 1)
    {
        // spr.drawLine(50 + n, 10 + 260 * (max - *it) / (max - min),
                    //  51 + n, 10 + 260 * (max - *(--it)) / (max - min), color);
        spr.drawWideLine(50 + n, 10 + 260 * (max - *it) / (max - min),
                         51 + n, 10 + 260 * (max - *(--it)) / (max - min), 2, color);
        n++;
    }

    spr.pushSprite(0, 0);
    spr.deleteSprite();

    // big number display thing of current reading
    spr.createSprite(180, SCREEN_HEIGHT);
    spr.setFreeFont(&FreeMonoBold24pt7b);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.drawString(String(value), 90, SCREEN_HEIGHT / 4);
    spr.setFreeFont(&FreeSans18pt7b);
    spr.setTextDatum(BC_DATUM);
    spr.drawString(label, 90, 3 * SCREEN_HEIGHT / 4);

    spr.pushSprite(300, 0);
    spr.deleteSprite();
}