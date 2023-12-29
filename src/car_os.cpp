#include <TFT_eSPI.h>
#include "TouchScreen.h"
#include <utility>
#include <map>
#include "stuff.hpp"
#include "calibration.hpp"
#include "datum.hpp"
#include "gui.hpp"

TFT_eSPI tft = TFT_eSPI();
ELM327 elm327;

// ###########################
// 		TOUCHSCREEN STUFF
// ###########################
#define YP A0 // 13 // Y+ pin, must be analog
#define XM A1 // 32 // X- pin, must be analog
#define XP 18 // 22 // X+ pin
#define YM 19 // 33 // Y- pin

// last argument is resistance between X+ and X- (280 ohms)
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 280);

GUI gui(tft);
double val;
Data curr_data;
int n;

int num_loops_touching = 0;
bool was_touching;

void setup()
{
    Serial.begin(115200);

    analogReadResolution(10);
    delay(2000);
    // Connect to elm327 - mostly taken from example
    Serial.println("Beginning connection process...");
    // serial_bt.begin("Car_OS");
    // serial_bt.begin();
    // gap_set_local_name("CarOS");
    delay(2000);
    // if (!serial_bt.connect("neven-laptop")) {
        // Serial.println("Couldn't connect to ELM327");
        // while (1); // halt execution
    // }

    // connection to esp32
    Serial1.setRX(17);
    Serial1.setTX(16);
    Serial1.begin(115200);

    // wait for esp32
    while (!Serial1.available() || Serial1.read() == '0') delay(50);

    if (!elm327.begin(Serial1, false, 2000)) {
        Serial.println("Couldn't set up elm327");
        while (1); // halt execution
    }
    Serial.println("Connected!");
    
    tft.init();
    tft.setRotation(1);
    tft.setTextColor(TFT_WHITE);
    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(&FreeSansBold12pt7b);
    
    Serial.println("Starting calibration...");
    calibrate(tft, ts);

    // Datum rpm = Datum("rpm", TFT_RED, new SinData(3000, 4000, 1/15.0));
    Datum data[4] = {
        Datum("rpm", TFT_RED, 0.f, 10000.f, 0),
        Datum("speed", TFT_BLUE, 0, 100),
        Datum("temp", TFT_GREEN, 0, 300),
        Datum("throt", TFT_CYAN, 0, 100)
    };
    gui.fill_data(data);
    
    ts.pressureThreshhold = 100;
}

void loop() {
    TSPoint p = ts.getPoint();
    // Point screenCoords = touchscreenAdjust({p.y, p.x});
    // if (p.z > ts.pressureThreshhold) {
    //     Serial.println("Just got a touch");
    //     Serial.print("("); Serial.print(screenCoords.x); Serial.print(", "); Serial.print(screenCoords.y);
    //     Serial.println(")");
    //     was_touching = true;
    // } else if (was_touching) {// not touching now, but just was (release) - coors might not be accurate
    //     gui.on_touch(screenCoords);
    //     was_touching = false;
    // }
    if (n % 5 == 0 && p.z > ts.pressureThreshhold) {
        Point screenCoords = touchscreenAdjust({p.y, p.x});
        Serial.println("Just got a touch");
        Serial.print("("); Serial.print(screenCoords.x); Serial.print(", "); Serial.print(screenCoords.y);
        Serial.println(")");
        Serial.println(p.z);
        if (0 <= screenCoords.x && screenCoords.x <= SCREEN_WIDTH && 
            0 <= screenCoords.y && screenCoords.y <= SCREEN_HEIGHT)
            gui.on_touch(screenCoords);
    }

    switch (curr_data) {
        case rpm: {
            val = elm327.rpm();
            // Serial.println("Querying rpm");
            break;
        }
        case speed: {
            val = elm327.mph();
            // Serial.println("Querying speed");
            break;
        }
        case temp: {
            val = elm327.engineCoolantTemp();
            // Serial.println("Querying temp");
            break;
        }
        case throt: {
            val = elm327.throttle();
            // Serial.println("Querying throttle");
            break;
        }
    }
    // we finally got a response
    if (elm327.nb_rx_state == ELM_SUCCESS) {
        // update gui
        gui.update(val, curr_data);
        // move to next data thing (there are 4 total)
        curr_data = (Data)((curr_data + 1) % 4);
        // Serial.print("Got: ");
        // Serial.println(val);
    }
    // error - just move on to next
    else if (elm327.nb_rx_state != ELM_GETTING_MSG) {
        elm327.printError();
        curr_data = (Data)((curr_data + 1) % 4);
    }

    // draw a random circle for testing purposes
    // if (n % 5 == 0) {
    //     int x = rand() % SCREEN_WIDTH;
    //     int y = rand() % SCREEN_HEIGHT;
    //     int r = rand() % 100;
    //     tft.fillCircle(x, y, r, rand() % 10 + 10);
    // }
    
    // gui.update(4000 + 3000.0 * sin((n * 1.0) / 10.0), curr_data);
    // curr_data = (Data)((curr_data + 1) % 4);
    n++;
    // gui.draw();
    // Serial.print(ESP.getFreeHeap()); Serial.println("    ");
    // delay(20);
    // delay(50);
}