#include <TFT_eSPI.h>
#include "TouchScreen.h"
#include <utility>
#include <map>
#include "stuff.hpp"
#include "calibration.hpp"
#include "datum.hpp"
#include "gui.hpp"
#include "SerialBT_improved.hpp"

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
int curr_data;
int n;

int num_loops_touching = 0;
int time_pressed = 0;
int prev_time = 0;
bool was_touching;

void setup()
{
    Serial.begin(115200);

    analogReadResolution(10);
#ifdef TESTING
    delay(5000);
#endif
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
    // Serial1.setRX(17);
    // Serial1.setTX(16);
    // Serial1.begin(115200);

    // // wait for esp32
    // while (!Serial1.available() || Serial1.read() == '0') delay(50);

    // if (!elm327.begin(Serial1, false, 2000)) {
    //     Serial.println("Couldn't set up elm327");
    //     while (1); // halt execution
    // }
    // Serial.println("Connected!");

    tft.init();
    tft.setRotation(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(&FreeSansBold12pt7b);
 
#ifndef TESTING
    SerialBT.begin();
    // SerialBT.connect("C4:D0:E3:95:75:53"); // laptop
    bool connected = false;
    while (!connected) {
       tft.fillScreen(TFT_BLACK);
       tft.setTextDatum(MC_DATUM);
       tft.setTextColor(TFT_GREEN);
       tft.drawString("Connecting Bluetooth...", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
       SerialBT.connect("00:1D:A5:02:05:78"); // ELM327
        // Serial.println("Press any key to continue...");
        // while (!Serial.available()) {delay(100);};
        // wait till connected?
        // while (!SerialBT) { 
        //     Serial.println("Waiting...");
        //     delay(100);
        // }
        delay(2000);
        Serial.println("Connected??");

        if (!elm327.begin(SerialBT, false, 2000)) {
            Serial.println("Couldn't set up elm327");
            tft.setTextColor(TFT_RED);
            tft.fillScreen(TFT_BLACK);
            tft.drawString("Bluetooth Connection Failed!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
            delay(1500);
            // while (1); // halt execution
        } else {
            connected = true;
        }
    }
    Serial.println("ELM connected");
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.drawString("Connected!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
#endif

    Serial.println("Starting calibration...");
    ts.pressureThreshhold = 75;
    calibrate(tft, ts);

    // Datum rpm = Datum("rpm", TFT_RED, new SinData(3000, 4000, 1/15.0));
    Datum data[4] = {
        // Datum("rpm", TFT_RED, 0, 10000, &ELM327::rpm, 0),
        // Datum("speed", TFT_BLUE, 0, 100, &ELM327::mph),
        // Datum("temp", TFT_GREEN, 0, 120, &ELM327::engineCoolantTemp),
        // Datum("throt", TFT_CYAN, 0, 100, &ELM327::throttle)
        Datum(RPM), Datum(SPEED), Datum(MPG), Datum(LOAD)
        // Datum("rpm"), Datum("speed"), Datum("temp"), Datum("throt")
    };
    gui.fill_data(data);
}

void loop() {
    TSPoint p = ts.getPoint();
    if (p.z > ts.pressureThreshhold) {
        Point screenCoords = touchscreenAdjust({p.y, p.x});
        if (++num_loops_touching > 3 && !was_touching) {
            Serial.print("INPUT REGISTERED ");
            time_pressed = millis();
            Serial.println(time_pressed);
            gui.on_touch(screenCoords);
            was_touching = true;
        }
        if (num_loops_touching > 20 && millis() - time_pressed > 1000) {
            gui.show_menu(screenCoords);
        }
        if (num_loops_touching > 20 && millis() - time_pressed > 5000) {
            Serial.println("Recalibrating...");
            tft.fillScreen(TFT_BLACK);
            delay(1000);
            calibrate(tft, ts);
            num_loops_touching = 0;
            gui.draw();
        }
    } else {
        num_loops_touching = max(num_loops_touching - 2, 0);
    }
    if (num_loops_touching == 0) {
        was_touching = false;
    }

#ifdef TESTING
    switch (gui.selected_data[curr_data]) {
        case RPM:
            val = rand() % 10000;
            break;
        case SPEED:
            val = 50;
            break;
        case ENGINE_TEMP:
            static int temp = 0;
            val = temp++ % 120;
            break;
        case THROTTLE:
            val = 0;
            break;
        case LOAD:
            val = rand() % 100;
            break;
        case TIMING_ADV:
            val = rand() % 100 - 50;
            break;
        case MAF_RATE:
            val = rand() % 650;
            break;
        case AIR_TEMP:
            val = rand() % 255 - 40;
            break;
        case RUN_TIME:
            val = rand() % 1000;
            break;
        case MPG:
            val = rand() % 30;
            break;
        default:
            val = 0;
            break;
    }
    gui.update(val, curr_data);
    curr_data = (curr_data + 1) % 4;
#else
    switch (gui.selected_data[curr_data]) {
        case RPM:
            val = elm327.rpm();
            break;
        case SPEED:
            val = elm327.mph();
            break;
        case ENGINE_TEMP:
            val = elm327.engineCoolantTemp();
            break;
        case THROTTLE:
            val = elm327.throttle();
            break;
        case LOAD:
            val = elm327.engineLoad();
            break;
        case TIMING_ADV:
            val = elm327.timingAdvance();
            break;
        case MAF_RATE:
            val = elm327.mafRate();
            break;
        case AIR_TEMP:
            val = elm327.intakeAirTemp();
            break;
        case RUN_TIME: // doesn't work
            // val = elm327.runTime();
            val = 0;
            break;
        case MPG:
            static bool stage = 0;
            static float speed = 0;
            static float maf = 0;
            if (stage)
                speed = elm327.mph();
            else
                maf = elm327.mafRate();
            
            if (elm327.nb_rx_state == ELM_SUCCESS)
                stage = !stage;

            // g/s [air] -> g/s [gas] -> gal/s -> gal/h
            // mph / gph -> mpg
            if (maf != 0)
                val = speed / (((maf / 14.7) / 2801) * 3600);
            else
                val = infinityf();
            break;
        default:
            val = 0;
            break;
    }
    // we finally got a response
    if (elm327.nb_rx_state == ELM_SUCCESS)
    {
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
#endif

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
    if (n % 20 == 0) {
        int new_time = millis();
        float framerate = 20.f / ((new_time - prev_time) / 1000.f);
        prev_time = new_time;
        tft.setTextDatum(BL_DATUM);
        tft.drawString(String(framerate, 0), 2, SCREEN_HEIGHT - 2);
    }
}