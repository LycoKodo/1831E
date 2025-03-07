#include "main.h"

#include "robot-config.hpp"
#include "controls.hpp"
#include "lemlib/selector.hpp"
#include "lemlib/smartMotor.hpp"
#include "lemlib/chassis/chassis.hpp"



// ------------------------- //
// Blue 1 Autonomous Routine //
// ------------------------- //

// e.g. normal blue left

int blue1() {
    
    rightMotors.move_voltage(120000);
    leftMotors.move_voltage(-120000);

    return 0;
}


