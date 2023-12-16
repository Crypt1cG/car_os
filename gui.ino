#include "inc/gui.hpp"

void GUI::fill_data(const std::array<Datum, 4>& data_in) {
    for (int i = 0; i < 4; i++) {
        int x = SCREEN_WIDTH / 4 + (i % 2) * (SCREEN_WIDTH / 2);
        int y = SCREEN_HEIGHT / 4 + (i / 2) * (SCREEN_HEIGHT / 2);
        data.push_back(data_in[i]);
        data[i].move({x, y});
        data[i].set_radius(SCREEN_HEIGHT / 4 - 5);
    }
}

// NOTE: might be better (faster) to like combine sprites from each gauge and then
// draw all at once
void GUI::draw() {
    if (focused == -1)
        for (auto& d : data)
            d.draw(tft);
    else
        data[focused].drawDetailed(tft);
}

void GUI::update() {
    for (auto& d : data) {
        d.update();
    }
}

void GUI::on_touch(Point p) {
    if (focused != -1) {
        focused = -1;
        // clear screen to get rid of graph
        tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_BLACK);
    } else {
        for (int i = 0; i < 4; i++)
            if (data[i].contains(p))
                focused = i;
    }
}