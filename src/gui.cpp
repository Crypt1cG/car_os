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
                                        "timing adv", "maf rate", "air temp", "run time"};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                tft.drawRect(15 + i * 155, 8 + j * 104, 140, 96, TFT_WHITE);
                tft.drawString(labels[i * 3 + j], 85 + i * 155, 56 + j * 104);
            }
        }
    }
}

void GUI::update(double val, int i) {
    data[i].update(val);
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

        // if -1, just change first one
        to_change = max(0, to_change);
        Point p = data[to_change].getPos();
        int r = data[to_change].getSize();

        // switch (menu_index) {
        //     case 0:
        //         // data[to_change] = Datum("rpm", TFT_RED, 0, 10000, &ELM327::rpm, 0);
        //         data[to_change] = Datum("rpm");
        //         break;
        //     case 1:
        //         // data[to_change] = Datum("speed", TFT_BLUE, 0, 100, &ELM327::mph);
        //         data[to_change] = Datum("speed");
        //         break;
        //     case 2:
        //         // data[to_change] = Datum("temp", TFT_GREEN, 0, 120, &ELM327::engineCoolantTemp);
        //         data[to_change] = Datum("temp");
        //         break;
        //     case 3:
        //         // data[to_change] = Datum("throt", TFT_CYAN, 0, 100, &ELM327::throttle);
        //         data[to_change] = Datum("throt");
        //         break;
        //     case 4:
        //         // data[to_change] = Datum("tank", TFT_YELLOW, 0, 100, &ELM327::fuelLevel);
        //         data[to_change] = Datum("fp");
        //         break;
        //     case 5:
        //         // data[to_change] = Datum("rate", TFT_PURPLE, 0, 20, &ELM327::fuelRate);
        //         data[to_change] = Datum("ip");
        //         break;
        //     case 6:
        //         // data[to_change] = Datum("load", TFT_MAGENTA, 0, 100, &ELM327::engineLoad);
        //         data[to_change] = Datum("load");
        //         break;
        //     case 7:
        //         // data[to_change] = Datum("maf", TFT_ORANGE, 0, 700, &ELM327::mafRate);
        //         data[to_change] = Datum("maf");
        //         break;
        //     case 8:
        //         // data[to_change] = Datum("oil", TFT_BROWN, -10, 150, &ELM327::oilTemp);
        //         data[to_change] = Datum("time");
        //         break;
        // }
        data[to_change] = Datum((Data)menu_index);

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

float GUI::get(int i) {
#ifdef TESTING
    return data[i].getter();
#else
    return (elm.*(data[i].getter))();
#endif
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