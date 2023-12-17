#pragma once
#include <utility>
#include "ELMduino.h"
#include <BluetoothSerial.h>


#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

struct Point {
    int x, y;    
};

enum Data {
    rpm,
    speed,
    temp,
    throt
};