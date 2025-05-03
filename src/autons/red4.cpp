#include "main.h"
#include "robot-config.hpp"
#include "controls.hpp"
#include "lemlib/selector.hpp"
#include "lemlib/smartMotor.hpp"
#include "lemlib/chassis/chassis.hpp"



// ------------------------- //
// Red 4  Autonomous Routine //
// ------------------------- //

// e.g. normal rush
ASSET(GoalMovement_txt);
ASSET(CornerS_txt);

int red4() {
    
    chassis.setPose(-62.596,-9.546,326);

    // chassis.moveToPoint(0, 48, 4000, {.forwards = true}, false);
    
    // SCORE 1 -> Alliance Wall Stake
    ladySmart.movePID(-20000, 1200, 10, false);
    ladySmart.movePID(-9000, 2000, 10, true);

    chassis.follow(
        GoalMovement_txt,
        10,
        5000, 
        false,
        false
    );
    mogo_mech.set_value(true);

    chassis.turnToHeading(319, 800);
    sos.set_value(true);

    Intake_SortedMove(127, 7000, -1, true);

    chassis.moveToPose(-42.6, -3.2, 319, 1200, {.forwards = true, .lead=0.1, .minSpeed = 30}, false);

    chassis.moveToPoint(-39.7, -6.2, 700, {.forwards = false, .maxSpeed=110, .minSpeed = 40}, false);
    pros::delay(700);
    sos.set_value(false);

    
    // Returning to goal pose
    chassis.moveToPose(-27, -23, 319, 1200, {.forwards = false, .lead=0.2, .minSpeed = 10}, false);
    chassis.turnToHeading(186, 800); // face 2 stack

    chassis.moveToPose(-30, -42, 186, 1500, {.forwards = true, .lead=0.1, .minSpeed = 10}, false);

    intake.move(0);

    // approach 2 stack

    // chassis.moveToPose(-48, 0, 0, 1500, {.forwards = true, .lead=0.6}, false);

    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);

    Intake_SortedMove(127, 10000, -1, true); // TODO - TEMPORARY

    chassis.setPose(-23.614, -51.314, 180);

    chassis.follow(
        CornerS_txt,
        10,
        5000, 
        true,
        false
    );

    return 0;
}


