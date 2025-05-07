#include "main.h"

#include "pros/motors.h"
#include "robot-config.hpp"
#include "controls.hpp"
#include "lemlib/selector.hpp"
#include "lemlib/smartMotor.hpp"
#include "lemlib/chassis/chassis.hpp"



// ------------------------- //
// Red 1  Autonomous Routine //
// ------------------------- //

// e.g. red right

int red1() {
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);

    chassis.setPose(0, 0, 51);

    ladySmart.movePID(-20000, 900, 10, false);
    ladySmart.movePID(10000, 1000, 10, true);
    
    chassis.moveToPose(-8, -8, 51, 800, {.forwards = false, .minSpeed = 0}, false);
    
    chassis.turnToHeading(108, 800);
    
    sos.set_value(true);
    
    roller.move_velocity(80);
    
    chassis.moveToPose(8, -9, 103, 1800, {.forwards = true, .minSpeed = 0}, false);
    
    pros::delay(200);

    sos.set_value(false);
    
    chassis.moveToPose(-6, -36, 29, 2000, {.forwards = false, .minSpeed = 0}, false);

    mogo_mech.set_value(true);

    Intake_SortedMove(127, 10000, -1, true);

    chassis.turnToHeading(269, 900);

    chassis.moveToPose(-23.90, -36.33, 269, 2000, {.forwards = true, .minSpeed = 0}, false);

    pros::delay(500);

    ladySmart.movePID(-15000, 900, 10, true);

    chassis.moveToPose(9.7, -48.47, 135.6, 2000, {.forwards = true, .minSpeed = 0}, false);

    ladySmart.movePID(-15000, 900, 10, true);

    // todo- BREAK
    // pros::delay(1500);

    // chassis.turnToHeading(180, 1000);

    // pros::delay(400);
    // mogo_mech.set_value(false);

    // chassis.moveToPose(-27, -44, 180, 2000, {.forwards = true, .minSpeed = 0}, false);

    // doinker.set_value(true);
    
    // pros::delay(400);

    // chassis.moveToPose(-27, -30, 180, 3000, {.forwards = false, .minSpeed = 0}, false);

    // pros::delay(500);

    // doinker.set_value(false);

    // chassis.turnToHeading(0, 1000);

    // chassis.moveToPose(-27, -47, -345, 3000, {.forwards = false, .minSpeed = 25}, false);
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    return 0;
}


