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

GUI gui(tft, elm327);
double val;
int curr_data;
int n;

int num_loops_touching = 0;
bool was_touching;

void setup()
{
    Serial.begin(115200);

    analogReadResolution(10);
    // delay(2000);
    // Connect to elm327 - mostly taken from example
    Serial.println("Beginning connection process...");
    // serial_bt.begin("Car_OS");
    // serial_bt.begin();
    // gap_set_local_name("CarOS");
    // delay(2000);
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
        // Datum("rpm", TFT_RED, 0, 10000, &ELM327::rpm, 0),
        // Datum("speed", TFT_BLUE, 0, 100, &ELM327::mph),
        // Datum("temp", TFT_GREEN, 0, 120, &ELM327::engineCoolantTemp),
        // Datum("throt", TFT_CYAN, 0, 100, &ELM327::throttle)
        Datum("rpm"), Datum("speed"), Datum("temp"), Datum("throt")
    };
    gui.fill_data(data);
    
    ts.pressureThreshhold = 140;
}

void loop() {
    TSPoint p = ts.getPoint();
    if (n % 5 == 0 && p.z > ts.pressureThreshhold) {
        Point screenCoords = touchscreenAdjust({p.y, p.x});
        Serial.println("Just got a touch");
        Serial.print("("); Serial.print(screenCoords.x); Serial.print(", "); Serial.print(screenCoords.y);
        Serial.println(")");
        Serial.println(p.z);
        if (0 <= screenCoords.x && screenCoords.x <= SCREEN_WIDTH && 
            0 <= screenCoords.y && screenCoords.y <= SCREEN_HEIGHT) {
            if (++num_loops_touching > 5) { // we've been holding for a bit
                // tft.fillScreen(TFT_RED);
                gui.show_menu(screenCoords);
            } else
                gui.on_touch(screenCoords);
        }
    } else if (n % 5 == 0) {
        num_loops_touching = max(num_loops_touching - 1, 0);
    }

    val = gui.get(curr_data);

    // we finally got a response
    if (elm327.nb_rx_state == ELM_SUCCESS) {
        // update gui
        gui.update(val, curr_data);
        // move to next data thing (there are 4 total)
        curr_data = (curr_data + 1) % 4;
        // Serial.print("Got: ");
        // Serial.println(val);
    }
    // error - just move on to next
    else if (elm327.nb_rx_state != ELM_GETTING_MSG) {
        elm327.printError();
        curr_data = (curr_data + 1) % 4;
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