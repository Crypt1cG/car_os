#include "datum.hpp"


const std::map<Data, std::tuple<String, uint32_t, double, double, func, int>> Datum::default_configs =
    {
#ifdef TESTING
        {RPM, {"rpm", TFT_RED, 0, 10000, [](){ return (rand() % 10000) * 1.f; }, 0}},
        {SPEED, {"speed", TFT_BLUE, 0, 100, [](){ return 50.f; }, 1}},
        {ENGINE_TEMP, {"temp", TFT_GREEN, 0, 120, [](){ static int val = 0; return (val++ % 120) * 1.f; }, 0}},
        {THROTTLE, {"throt", TFT_CYAN, 0, 100, [](){ return (rand() % 100) * 1.f; }, 1}},
        {LOAD, {"load", TFT_MAGENTA, 0, 100, [](){return 0.f;}, 1}},
        {TIMING_ADV, {"time", TFT_DARKCYAN, -64, 63.5, [](){return (rand() % 100 - 50) * 1.f;}, 1}},
        {MAF_RATE, {"maf", TFT_ORANGE, 0, 655.35, [](){return (rand() % 655) * 1.f;}, 1}},
        {AIR_TEMP, {"atmp", TFT_NAVY, -40, 215, [](){return (rand() % 255 - 40) * 1.f;}, 0}},
        {RUN_TIME, {"run time", TFT_MAROON, 0, 65535, [](){static float time = 0; return time++;}, 1}}
#else
        {RPM, {"rpm", TFT_RED, 0, 10000, &ELM327::rpm, 0}},
        {SPEED, {"speed", TFT_BLUE, 0, 100, &ELM327::mph, 1}},
        {ENGINE_TEMP, {"temp", TFT_GREEN, 0, 120, &ELM327::engineCoolantTemp, 0}},
        {THROTTLE, {"throt", TFT_CYAN, 0, 100, &ELM327::throttle, 1}},
        {LOAD, {"load", TFT_MAGENTA, 0, 100, &ELM327::engineLoad, 1}},
        {TIMING_ADV, {"time", TFT_DARKCYAN, -64, 63.5, &ELM327::timingAdvance, 1}},
        {MAF_RATE, {"maf", TFT_ORANGE, 0, 655.35, &ELM327::mafRate, 1}},
        {AIR_TEMP, {"atmp", TFT_NAVY, -40, 215, &ELM327::intakeAirTemp, 0}},
        {RUN_TIME, {"run time", TFT_MAROON, 0, 65535, (float (ELM327::*)())(&ELM327::runTime), 1}}
#endif
    };

void Datum::update(double new_val)
{
    value = new_val;
    curr_ind = (curr_ind + 1) % HIST_SIZE;
    history[curr_ind] = value;
    // history.push_front(new_val);
    // if (history.size() > 200) history.pop_back();
}

void Datum::draw(TFT_eSPI& tft)
{
    TFT_eSprite spr = TFT_eSprite(&tft);
    spr.setColorDepth(16); // consider changing this for more speed?
    spr.setTextColor(TFT_WHITE, GAUGE_BG);
    // set font for number (could use smooth fonts, but it would be slower)
    spr.setFreeFont(&FreeMonoBold18pt7b);
    // add one for like anti-aliasing (it gets a little cut off otherwise)
    spr.createSprite(2 * radius + 1, 2 * radius + 1);
    // background around circle
    spr.fillSprite(TFT_BLACK);
    // gauge background circle
    spr.fillSmoothCircle(radius, radius, radius, GAUGE_BG);
    // don't draw if arc is too small - sometimes it wraps all the way around
    if (abs(value - min) > 0.001 * (max - min))
        spr.drawSmoothArc(radius, radius, 0.95 * radius, 0.8 * radius,
                          45, 45 + ((value - min) * 1.0 / (max - min)) * 270,
                          color, GAUGE_BG);
    // draw the number
    spr.setTextDatum(BC_DATUM);
    spr.drawString(String(value, precision), radius, radius);
    
    // draw the label text
    spr.setFreeFont(&FreeSans12pt7b);
    spr.setTextDatum(TC_DATUM);
    spr.drawString(label, radius, radius);
    
    spr.pushSprite(pos.x - radius, pos.y - radius);
    spr.deleteSprite();

    // tft.setTextColor(TFT_WHITE, GAUGE_BG);
    // tft.setFreeFont(&FreeMonoBold18pt7b);
    // tft.fillSmoothCircle(pos.x, pos.y, radius, GAUGE_BG);
    // tft.drawSmoothArc(pos.x, pos.y, 0.95 * radius, 0.8 * radius,
    //                   45, 45 + ((value - min) * 1.0 / (max - min)) * 270,
    //                   color, GAUGE_BG);
    // tft.setTextDatum(BC_DATUM);
    // tft.drawString(String(value), pos.x, pos.y);
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
    spr.createSprite(280, SCREEN_HEIGHT);
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

    // int n = 0;
    // std::list<float>::iterator it = history.end();
    // it--; // end() returns an iterator pointing to one past the end
    // while (n + 1 < history.size())
    // {
    //     // spr.drawLine(50 + n, 10 + 260 * (max - *it) / (max - min),
    //                 //  51 + n, 10 + 260 * (max - *(--it)) / (max - min), color);
    //     if (*it <= max && *it >= min)
    //         // spr.drawWideLine(50 + n, 10 + 260 * (max - *it) / (max - min),
    //         //                 51 + n, 10 + 260 * (max - *(--it)) / (max - min), 2, color);
    //     {
    //         int p1 = 10 + 260 * (max - *it) / (max - min);
    //         int p2 = 10 + 260 * (max - *(--it)) / (max - min);
    //         spr.drawFastVLine(50 + n, std::min(p1, p2), abs(p2 - p1), color);
    //     }
    //     n++;
    // }
    // int n = 0;
    // auto it = history.begin();
    // while (it != history.end()) {
    //     spr.drawSpot(250 - n, 10 + 260 * (max - *it) / (max - min), 2, color);
    //     it++;
    //     n++;
    // }

    int ind = (curr_ind + 1) % HIST_SIZE; // should be the last element  
    for (int i = 0; i < HIST_SIZE - 1; i++) {
        if (history[ind] <= max && history[ind] >= min) {
            int p1 = 10 + 260 * (max - history[ind]) / (max - min);
            int p2 = 10 + 260 * (max - history[(ind + 1) % HIST_SIZE]) / (max - min);
            int length = abs(p2 - p1) + 1; // add 1 to length so that constant isn't 0
            spr.drawFastVLine(50 + i, std::min(p1, p2), length, color);
        }
        ind = (ind + 1) % HIST_SIZE;
    }

    spr.pushSprite(0, 0);
    spr.deleteSprite();

    // big number display thing of current reading
    spr.createSprite(200, SCREEN_HEIGHT);
    spr.setFreeFont(&FreeMonoBold24pt7b);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.drawString(String(value, precision), 90, SCREEN_HEIGHT / 4);
    spr.setFreeFont(&FreeSans18pt7b);
    spr.setTextDatum(BC_DATUM);
    spr.drawString(label, 90, 3 * SCREEN_HEIGHT / 4);

    spr.pushSprite(280, 0);
    spr.deleteSprite();
}

bool Datum::contains(Point p) {
    return sqrt((p.x - pos.x) * (p.x - pos.x) +
                (p.y - pos.y) * (p.y - pos.y)) < radius;
}