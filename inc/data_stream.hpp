#pragma once
#include "BluetoothSerial.h"
#include "ELMduino.h"

BluetoothSerial serial_bt;
ELM327 elm327;

class Getter {
public:
    virtual float operator()() = 0;
};

class RPMGetter : public Getter {
    ELM327& elm;
public:
    RPMGetter(ELM327& elm_in) : elm(elm_in) {}
    float operator()() override {
        return elm.rpm();
    }
};

class DataStream {
// give access to derived classes but not outside
// NOTE: there might be a better way to do this (this is not the EECS280 way)
protected:
    double data;
public:
    DataStream(double min_in, double max_in)
        : min(min_in), max(max_in) {}
    // returns data 
    double get() const { return data; }
    // updates (and returns) data
    virtual double update() = 0;
    // min and max values
    double min, max;
};

class SinData : public DataStream {
    uint64_t time = 0;
    double amplitude;
    double mean;
    double speed;

public:
    SinData(double a, double m, double s) 
        : amplitude(a), mean(m), speed(s), 
          DataStream(m - a, m + a) {}
    double update() override {
        return data = amplitude * sin((time++ * 1.0) * speed) + mean;        
    }
};

class ELMStream : public DataStream {
    //float (*query)();
    Getter* g;
public:
    ELMStream(Getter* get, double min, double max)
        : g(get), DataStream(min, max) {}
    double update() override {
        // by default, ELMDuino is non blocking, so we make it blocking
        while (true) {
            double tempVal = (*g)();

            if (elm327.nb_rx_state == ELM_SUCCESS) {
                data = tempVal;
                return data;
            } else if (elm327.nb_rx_state != ELM_GETTING_MSG) {
                elm327.printError();
                return data;
            }
        }
    }
};


// class RPMStream : public DataStream {
//     bool waiting = false;
//     float tempRPM;
// public:
//     RPMStream() 
//         : DataStream(0, 9000) {}
//     double update() override {
        
//         // we've been waitig for a response, and we just got it
//         if (waiting && elm327.nb_rx_state == ELM_SUCCESS) {
//             return data = elm327.rpm();
//         }
//     }
// };