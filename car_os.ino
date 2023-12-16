#include <TFT_eSPI.h>
#include "TouchScreen.h"
#include <utility>
#include <map>
#include "inc/stuff.hpp"
#include "inc/calibration.hpp"
#include "inc/datum.hpp"
#include "inc/gui.hpp"

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

GUI gui(tft);
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

    // Connect to elm327 - mostly taken from example
    Serial.println("Beginning connection process...");
    serial_bt.begin("Car_OS", true);
    if (!serial_bt.connect("OBDII")) {
        Serial.println("Couldn't connect to ELM327");
        while (1); // halt execution
    }
    if (!elm327.begin(serial_bt, true, 2000)) {
        Serial.println("Couldn't set up elm327");
        while (1); // halt execution
    }
    Serial.println("Connected!");

    //Datum rpm = Datum("rpm", TFT_RED, new SinData(3000, 4000, 1/15.0));
    Datum rpm = Datum("rpm", TFT_RED, new ELMStream(new RPMGetter(elm327), 0, 10000));
    Datum speed = Datum("speed", TFT_BLUE, new SinData(100, 100, 1/10.0));
    Datum temp = Datum("temp", TFT_GREEN, new SinData(70, 100, 1/20.0));
    Datum mpg = Datum("mpg", TFT_CYAN, new SinData(10, 10, 1/15.0));
    gui.fill_data({rpm, speed, temp, mpg});

}

void loop()
{
    TSPoint p = ts.getPoint();
    if (p.z > ts.pressureThreshhold)
    {
        Point screenCoords = touchscreenAdjust({p.y, p.x});
        gui.on_touch(screenCoords);
    }
    num++;
    gui.update();
    gui.draw();
    delay(20);
}