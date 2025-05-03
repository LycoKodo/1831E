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

    /* ───────────── MIRRORED-FOR-RIGHT-SIDE AUTON ───────────── */

    chassis.setPose(0, 0, -51);

    // setColorSort();

    ladySmart.movePID(-20000, 900, 10, false);
    ladySmart.movePID(10000, 1000, 10, true);

    /* ── leave start box, back up under crossbar ── */
    chassis.moveToPose(  8,  -8,  -51,  800,
                        {.forwards = false, .minSpeed = 40}, false);

    chassis.turnToHeading(-108, 800);

    sos.set_value(true);
    roller.move_velocity(80);

    /* ── swing out to roller ── */
    chassis.moveToPose( -8,  -9, -103, 1800,
                        {.forwards = true,  .minSpeed = 40}, false);

    pros::delay(400);
    sos.set_value(false);

    /* ── drop to corner ── */
    chassis.moveToPose( 11, -40,  -28, 2000,
                        {.forwards = false, .minSpeed = 40}, false);

    mogo_mech.set_value(true);
    intake.move_velocity(127);
    Intake_SortedMove(127, 10000, -1, true);

    /* ── rotate to face bar side ── */
    chassis.turnToHeading(-270, 900);

    /* ── drive along bar ── */
    chassis.moveToPose( 27, -35, -270, 2000,
                        {.forwards = true,  .minSpeed = 25}, false);

    pros::delay(1500);

    /* ── square to field wall ── */
    chassis.turnToHeading(-180, 1000);

    mogo_mech.set_value(false);

    /* ── pull back a little ── */
    chassis.moveToPose( 27, -44, -180, 2000,
                        {.forwards = true,  .minSpeed = 25}, false);

    doinker.set_value(true);
    pros::delay(400);

    /* ── shove game-object home ── */
    chassis.moveToPose( 27, -30, -270, 2000,
                        {.forwards = false, .minSpeed = 25}, false);

    pros::delay(500);
    doinker.set_value(false);

    /* ── straighten for final run ── */
    chassis.turnToHeading(0, 1000);

    /* ── retreat to safe zone ── */
    chassis.moveToPose( 27, -47,  345, 3000,
                        {.forwards = false, .minSpeed = 25}, false);

    /* ───────────────────────── END OF AUTON ───────────────────────── */

    return 0; // success, 1 for fail
}


