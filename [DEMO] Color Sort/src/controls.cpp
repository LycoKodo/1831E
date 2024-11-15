#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/device.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include <sys/wait.h>

#include "robot-config.hpp"

char alliance = 'N';
bool allianceConfirmed = false;

void setColorSort() {
    static bool pressed = false;
    pressed = !pressed;
    if (pressed) {
        pros::lcd::clear_line(3);
        pros::lcd::set_text(3, "[CLS] Alliance: RED");
        alliance = 'R';
    } else {
        pros::lcd::clear_line(3);
        pros::lcd::set_text(3, "[CLS] Alliance: BLUE");
        alliance = 'B';
    }
}

void confirmSelection() {
    static bool pressed = false;
    pressed = !pressed;

    if (pressed) {
        allianceConfirmed = true;
    }
}

bool ringInspect() {
    const float blue_lim_low = 150, blue_lim_high = 250;
    const float red_lim_low = 0, red_lim_high = 30;
    double hue = colorSort.get_hue();

    if (alliance == 'B') {
        return !(red_lim_low < hue && hue < red_lim_high);
    } else if (alliance == 'R') {
        return !(blue_lim_low < hue && hue < blue_lim_high);
    }
    return true; // Default: passes for any other color
}