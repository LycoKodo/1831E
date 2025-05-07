#include "main.h"

#include "pros/motors.h"
#include "robot-config.hpp"
#include "controls.hpp"
#include "lemlib/selector.hpp"
#include "lemlib/smartMotor.hpp"
#include "lemlib/chassis/chassis.hpp"



// ------------------------- //
// Blue 1 Autonomous Routine //
// ------------------------- //

// Blue Positive RUSH

int blue1() {
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);

    setColorSort();
    /* ───────────── MIRRORED-FOR-RIGHT-SIDE AUTON ───────────── */

    chassis.setPose(0, 0, -51);

    // setColorSort();

    ladySmart.movePID(-20000, 900, 10, false);
    ladySmart.movePID(10000, 1000, 10, true);

    /* ── leave start box, back up under crossbar ── */
    chassis.moveToPose(  8,  -8,  -51,  1000,
                        {.forwards = false, .minSpeed = 40}, false);

    chassis.turnToHeading(-108, 800);

    sos.set_value(true);
    roller.move_velocity(80);

    // /* ── swing out to roller ── */
    chassis.moveToPose( -7.3,  -8, -108.09, 1400,
                        {.forwards = true,  .minSpeed = 0}, false);

    pros::delay(300);
    sos.set_value(false);

    chassis.moveToPose( 7, -36,  -39, 1300,
                        {.forwards = false, .lead=0.3, .maxSpeed = 127, .minSpeed = 0}, false);

    mogo_mech.set_value(true);
    pros::delay(80);
    Intake_SortedMove(127, 10000, -1, true);
    
    pros::delay(400);
    // /* ── rotate to face bar side ── */
    chassis.turnToHeading(98.1, 900);

    /* ── face 2 stack ── */
    chassis.moveToPose( 27, -40, 98.1, 1500,
                        {.forwards = true, .lead=0.1,  .minSpeed = 0}, false);


    pros::delay(230);
    
    chassis.moveToPose(1.6, -43.55, 234.27, 2000, {.forwards = true, .lead=0.2, .minSpeed=0}, false);

    ladySmart.movePID(-30000, 1000,0.5, false);

    // chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    // // /* ── square to field wall ── */
    // chassis.turnToHeading(185, 1000, {.minSpeed = 15});
    
    // pros::delay(400);
    // mogo_mech.set_value(false);
    // //     // break

    // // /* ── get mid ── */
    // chassis.moveToPose( 37, -47, 185, 2000,
    //                     {.forwards = true,  .minSpeed = 25}, false);

    // doinker.set_value(true);
    // pros::delay(400);

    // // /* ── shove game-object home ── */
    // chassis.moveToPose( 37, -37, 185, 2000,
    //                     {.forwards = false, .lead=0.1, .minSpeed = 0}, false);

    // pros::delay(500);
    // doinker.set_value(false);

    // // /* ── turn for clamp ── */
    // chassis.turnToHeading(365, 1000);

    // // /* ── retreat to safe zone ── */

    // // chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    // chassis.moveToPose( 34, -52.6,  389, 3000,
    //                     {.forwards = false, .minSpeed = 25}, false);

    
    // mogo_mech.set_value(true);

    // pros::delay(300);
    
    // Intake_SortedMove(127, 2000, -1, false);
    // /* ───────────────────────── END OF AUTON ───────────────────────── */

    return 0; // success, 1 for fail
}


