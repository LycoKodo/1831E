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
#include "controls.hpp"
#include "ladyPid.hpp"

/*
    TODO:
    [x] Tune controls.cpp Color sort wait time
    [ ] Test Intake_sortedmove's async parameter
    [ ] Validate all refractored files and merge to main
 */

void initialize() {
    // -----------------------
    // Initialise Core Systems
    // -----------------------
    pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(); // calibrate sensors

    // -----------------------
    // Initialise Sensors
    // -----------------------
    colorSort.set_led_pwm(100);
    lady_rotation.set_position(0);

    // Selecting Alliance Color
    pros::lcd::register_btn1_cb(setColorSort);
    pros::lcd::print(3, "[!] ALLIANCE NOT SELECTED, (MID BTN)");

    // -----------------------
    // Start Screen Task
    // -----------------------
    pros::Task screenTask([]() {
        while (true) {
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            pros::lcd::print(4, "Lady Pos: %ld", lady_rotation.get_position());

            // master.clear_line(0);
            // master.set_text(0, 0, "ALS: " + alliance);

            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
            pros::delay(50);
        }
    });

    // -----------------------
    // Initialise Subsystems
    // -----------------------
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);
    mogo_mech.set_value(true);
    doinker.set_value(false);
}


void disabled() 
{

}


void competition_initialize() {
}

ASSET(bigmove_txt);

void autonomous() 
{
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);
    const float goalHeight = 12;
    chassis.setPose(0, 0, 0); 

    // Yeah timmy get back to programming grrrrrrr
        // and quinn ur house literally has mustard gas frfr bro T^T

    // Anyways happy coding
    //Quinn: it's not that bad 😭
    //Tim: real

    hook.move(127);
    pros::delay(400);
    hook.move(0);
    chassis.moveToPose(0, 17, 0, 1200, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);
    pros::delay(100);
    chassis.turnToHeading(270, 1000);

    chassis.moveToPose(35, 14, 275, 1200, {.forwards=false, .maxSpeed=115, .minSpeed=60}, true);
    pros::delay(700);
    mogo_mech.set_value(false);

    chassis.moveToPose(32, 14, 275, 1200, {.forwards=true, .maxSpeed=127, .minSpeed=20}, true);

    pros::delay(400);
    chassis.turnToHeading(89, 1200);
    intake.move(127);
    chassis.moveToPose(50, 14, 90, 1200, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);
    pros::delay(400);
    chassis.moveToPose(60, 14, 90, 1200, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);
    pros::delay(400);
    chassis.moveToPose(45, 17, 90, 1200, {.forwards=false, .lead=0.1, .maxSpeed=127, .minSpeed=60}, false);
    chassis.turnToHeading(180, 900);
    pros::delay(400);
    chassis.moveToPose(45, 5, 180, 1200, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);
    pros::delay(400);
    chassis.turnToHeading(-66, 900);
    chassis.moveToPose(57, 4, 300, 1200, {.forwards=false, .lead=0.2, .maxSpeed=127, .minSpeed=60}, false);
    mogo_mech.set_value(true);


    pros::delay(400);
    chassis.moveToPose(55, 14, 0, 1200, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);
    intake.move(0);
    chassis.turnToHeading(90, 900);


    chassis.moveToPose(-27.5, 21, 90, 5000, {.forwards=false, .maxSpeed=127, .minSpeed=60}, true);
    pros::delay(1700);
    mogo_mech.set_value(false);

    pros::delay(400);
    chassis.turnToHeading(275, 900);
    intake.move(127);
    chassis.moveToPose(-55, 25, 275, 5000, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);
    pros::delay(400);
    chassis.moveToPose(-48, 28, 310, 1000, {.forwards=false, .maxSpeed=127, .minSpeed=60}, false);
    chassis.turnToHeading(180, 900);
    chassis.moveToPose(-48, 10, 180, 5000, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);
    pros::delay(400);
    chassis.turnToHeading(90, 900);
    chassis.moveToPose(-58, 10, 110, 5000, {.forwards=false, .lead=0.3, .maxSpeed=127, .minSpeed=60}, false);
    mogo_mech.set_value(true);
    pros::delay(400);
    chassis.moveToPose(-56, 11, 110, 2000, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);

    chassis.turnToHeading(360, 1800, {}, false);

    // chassis.setPose(-157.498, 156.516, 0);

    // chassis.follow(
    //     bigmove_txt,
    //     12, 
    //     10000,
    //     true, 
    //     false
    // );

    // LadyMovePID(25000, 1200, false);
    // chassis.moveToPose(-48, 50, 0, 5000, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);
    // //chassis.moveToPose(29, 14, 275, 1200, {.forwards=false, .maxSpeed=127, .minSpeed=60}, true);
    // /*chassis.moveToPose(26, 20, 225, 3000, {.forwards=false, .lead=0.4, .maxSpeed=127, .minSpeed=60}, false);
    // mogo_mech.set_value(false);
    // pros::delay(600);
    // chassis.turnToHeading(100, 2000, {.maxSpeed=100, .minSpeed=30});
    // intake.move(127);
    // chassis.moveToPose(53, 17, 100, 3000, {.forwards=true, .lead=0.2, .maxSpeed=127, .minSpeed=60}, false);*/

}

void opcontrol() 
{
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    
    // DON'T CHANGE!: Multi-treading for robot controls (To prevent color sort interruption)
    pros::Task intakeTask(intake_control); // Interrupted by color sort
    pros::Task mogoTask(mogo_control);
    pros::Task driveTask(drivetrain_control);
    pros::Task ladyTask(ladyctl);
    pros::Task doinkerTask(doinker_control);
    pros::Task endgameTask(endgame_control);
}

