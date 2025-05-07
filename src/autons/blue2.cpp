#include "main.h"

#include "pros/motors.h"
#include "robot-config.hpp"
#include "controls.hpp"
#include "lemlib/selector.hpp"
#include "lemlib/smartMotor.hpp"
#include "lemlib/chassis/chassis.hpp"



// ------------------------- //
// Blue 2 Autonomous Routine //
// ------------------------- //

// e.g. normal blue right

int blue2() {
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);
    chassis.setPose(0, 0, 53);

    ladySmart.movePID(-20000, 900, 10, false);
    ladySmart.movePID(10000, 1000, 10, true);
    
    chassis.moveToPose(-17, -46, 33, 1500, {.forwards = false, .minSpeed = 40}, false);

    // chassis.moveToPose(-8.5, -36.8, 33, 1500, {.forwards = false, .minSpeed = 40}, false); // timmy's
    
    mogo_mech.set_value(true);
    
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    pros::delay(400);
    
    chassis.turnToHeading(235, 600);
    
    Intake_SortedMove(127, 13000, -1, true);
    
    chassis.moveToPose(-22.3, -49.51, 190, 1600, {.forwards = true, .lead=0.3, .maxSpeed=60, .minSpeed = 0}, false);
    
    chassis.turnToHeading(243, 500);
    
    chassis.moveToPose(-27.5, -53.5, 243, 800, {.forwards = true, .minSpeed = 0}, false); // minspeed changed from 30
    
    pros::delay(300);
    
    chassis.turnToHeading(-16, 500);
    
    chassis.moveToPose(-29.66, -44.62, -9.96, 1600, {.forwards = true, .minSpeed = 30}, false);

    pros::delay(300); // wiating for 2 stack

    chassis.turnToHeading(57, 800);
    
    lady.get_voltage();
    
    chassis.moveToPose(8, -17, 57, 2000, {.forwards = true, .minSpeed = 30}, true);
    
    pros::delay(700);
    
    sos.set_value(true);
    
    pros::delay(1400);
    
    chassis.moveToPose(6, -19, 57, 2000, {.forwards = true, .minSpeed = 30}, false);
    
    sos.set_value(false);
    
    chassis.turnToHeading(148, 800);
    
    ladySmart.movePID(-17000, 1000, 10, true);
    
    chassis.moveToPose(11, -26, 148, 3000, {.forwards = true, .minSpeed = 70}, false);
    
    return 0;
}


