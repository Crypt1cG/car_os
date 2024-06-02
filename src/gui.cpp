#include "gui.hpp"
#include <array>

//void GUI::fill_data(const std::array<Datum, 4>& data_in) {
void GUI::fill_data(Datum* data_in) {
    for (int i = 0; i < 4; i++) {
        int x = SCREEN_WIDTH / 4 + (i % 2) * (SCREEN_WIDTH / 2);
        int y = SCREEN_HEIGHT / 4 + (i / 2) * (SCREEN_HEIGHT / 2);
        data.push_back(data_in[i]);
        data[i].move({x, y});
        data[i].resize(SCREEN_HEIGHT / 4 - 5);
        selected_data[i] = data[i].getDataType();
    }
}

// NOTE: might be better (faster) to like combine sprites from each gauge and then
// draw all at once
void GUI::draw() {
    if (!menu_showing) {
        if (focused == -1)
            for (auto& d : data)
                d.draw(tft);
        else
            data[focused].drawDetailed(tft);
    } else {
        // consider using sprite
        // TFT_eSprite spr = TFT_eSprite(&tft);
        // spr.setColorDepth(8);
        // spr.setTextColor(TFT_WHITE, TFT_BLACK);
        // spr.setFreeFont(&FreeMono9pt7b);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setFreeFont(&FreeMono12pt7b);
        tft.setTextDatum(MC_DATUM);
        // std::array<String, 9> labels = {"rpm", "speed", "temp", "throt", "fuel level",
        //                                 "fuel rate", "engine load", "maf rate", "oil temp"};
        std::array<String, 9> labels = {"rpm", "speed", "temp", "throt", "engine load",
                                        "timing adv", "maf rate", "air temp", "MPG"};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                tft.drawRect(15 + i * 155, 8 + j * 104, 140, 96, TFT_WHITE);
                tft.drawString(labels[i * 3 + j], 85 + i * 155, 56 + j * 104);
            }
        }
    }
}

void GUI::update(float val, int i) {
    Datum::data_arr[selected_data[i]] = val;
    data[i].update();
    if (!menu_showing) {
        // update just this one
        if (focused == -1)
            data[i].draw(tft);
        else if (focused == i)
            data[i].drawDetailed(tft);
    }
}

void GUI::on_touch(Point p) {
    if (menu_showing) {
        menu_showing = false;
        tft.fillScreen(TFT_BLACK);
        // technically not quite right, but close enough (not centered right)
        int col = p.x / 160;
        int row = p.y / 107;
        int menu_index = col * 3 + row;
        // could go out of range if calibration is bad
        if (menu_index < 0 || menu_index > 9) return;

        // NOTE: This isn't needed anymore??
        // if -1, just change first one
        to_change = max(0, to_change);
        Point p = data[to_change].getPos();
        int r = data[to_change].getSize();

        data[to_change] = Datum((Data)menu_index);
        selected_data[to_change] = (Data)menu_index;

        data[to_change].move(p);
        data[to_change].resize(r);
    } else {
        if (focused != -1) {
            focused = -1;
            // clear screen to get rid of graph
            tft.fillScreen(TFT_BLACK);
        } else {
            focused = which_datum(p);
        }
    }
    draw();
}

void GUI::show_menu(Point p) {
    if (menu_showing) return;
    int i = which_datum(p);
    if (i == -1) return;
    menu_showing = true;
    tft.fillScreen(TFT_BLACK);
    draw();
    to_change = which_datum(p);
}

int GUI::which_datum(Point p) {
    for (int i = 0; i < 4; i++)
        if (data[i].contains(p))
            return i;
    return -1;
}