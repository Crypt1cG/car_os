#include <TFT_eSPI.h>
#include "TouchScreen.h"
#include <utility>
#include <map>
#include "inc/stuff.hpp"
#include "inc/calibration.hpp"
#include "inc/datum.hpp"

TFT_eSPI tft = TFT_eSPI();

// ###########################
// 		TOUCHSCREEN STUFF
// ###########################
#define YP 13 // Y+ pin
#define XM 32 // X- pin
#define XP 22 // X+ pin
#define YM 23 // Y- pin

// last argument is resistance between X+ and X- (280 ohms)
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 280);

Datum rpm = Datum("rpm", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 8000, TFT_GREEN, 100);
bool big = false;
int num;

void setup()
{
    Serial.begin(115200);

    analogReadResolution(10);
    
    tft.init();
    tft.setRotation(1);
    tft.setTextColor(TFT_WHITE);
    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(&FreeSansBold12pt7b);
    
    calibrate(tft, ts);
}

void loop()
{
    TSPoint p = ts.getPoint();
    if (p.z > ts.pressureThreshhold)
    {
        std::pair<int, int> screenCoords = touchscreenAdjust({p.y, p.x});
        // Serial.println("Touched at: (" + String(p.y) + ", " + String(p.x) + 
        //                "), converted to: (" + String(screenCoords.first) + ", " + String(screenCoords.second) + ")");
        // tft.fillCircle(screenCoords.first, screenCoords.second, 5, TFT_RED);
        if (!big && sqrt((screenCoords.first - rpm.x)*(screenCoords.first - rpm.x) + 
                         (screenCoords.second - rpm.y)*(screenCoords.second - rpm.y)) < rpm.radius)
        {
            Serial.println("GOING BIG!");
            big = true;
            tft.fillScreen(TFT_BLACK);
        }
        else if (big)
        {
            Serial.println("going small");
            big = false;
            tft.fillScreen(TFT_BLACK);
        }
    }
    num++;
    // num %= 100;
    rpm.update(3000 * sin(num / 15.0) + 4000);
    if (big) rpm.drawDetailed(tft);
    else rpm.draw(tft);
    delay(20);
}