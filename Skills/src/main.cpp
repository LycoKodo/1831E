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

    // ** Scoring Alliance Wall Stake ** //

    hook.move(127);
    pros::delay(400);
    hook.move(0);
    chassis.moveToPose(0, 17, 0, 1200, {.forwards=true, .maxSpeed=127, .minSpeed=40}, false); // moving to parallel with goal
    pros::delay(100);
    chassis.turnToHeading(270, 1000);

    // ** GETTING RIGHT GOAL ** //

    chassis.moveToPose(26, 14, 275, 1200, {.forwards=false, .maxSpeed=108, .minSpeed=35}, false); // ASYNC
    mogo_mech.set_value(false);
    pros::delay(400); // delay to let mogo settle

    // ** Scoring with Right Goal ** //

    intake.move(127);

    chassis.turnToHeading(0, 1200);

    chassis.moveToPose(25, 40, 0, 1800, {.forwards=true, .lead=0, .maxSpeed = 108, .minSpeed = 0}, false);
    
    chassis.turnToHeading(90, 1300);

    chassis.moveToPose(50, 38.7, 90, 2000, {.forwards=true, .lead=0, .maxSpeed = 108, .minSpeed = 0}, false);

    chassis.turnToHeading(180, 1300);

    chassis.moveToPose(48.5, 7, 180, 2000, {.forwards=true, .lead=0, .maxSpeed = 108, .minSpeed = 0}, false);
    
    chassis.turnToHeading(305, 1300, {.direction=AngularDirection::CCW_COUNTERCLOCKWISE}, false);

    // Dumping in corner
    chassis.moveToPose(56, 3, 305, 1200, {.forwards=false, .lead=0, .maxSpeed = 100, .minSpeed = 0}, false);

    mogo_mech.set_value(true); // unclamp

    pros::delay(200);

    chassis.moveToPose(43, 13, 300, 1500, {.forwards=true, .lead=0, .maxSpeed = 100, .minSpeed = 0}, false);

    chassis.turnToHeading(90, 1300);

    chassis.moveToPose(-25.5, 14, 90, 5000, {.forwards=false, .maxSpeed=110, .minSpeed=60}, false);

    pros::delay(300);
    
    mogo_mech.set_value(false);

    pros::delay(200);
    
    chassis.turnToHeading(0, 1300);

    chassis.moveToPose(-25.5, 40, 0, 1800, {.forwards=true, .lead=0, .maxSpeed = 108, .minSpeed = 0}, false);

    chassis.turnToHeading(270, 1300);

    chassis.moveToPose(-50, 38.7, 270, 2000, {.forwards=true, .lead=0, .maxSpeed = 108, .minSpeed = 0}, false);

    chassis.turnToHeading(180, 1300);

    chassis.moveToPose(-48.5, 7, 180, 2000, {.forwards=true, .lead=0, .maxSpeed = 108, .minSpeed = 0}, false);

    pros::delay(200);

    chassis.turnToHeading(55, 1300, {.direction=AngularDirection::CW_CLOCKWISE}, false);

    chassis.moveToPose(-56, 3, 55, 1200, {.forwards=false, .lead=0, .maxSpeed = 100, .minSpeed = 0}, false);

    mogo_mech.set_value(true); // unclamp

    pros::delay(200);

    chassis.moveToPose(-43, 13, 60, 1500, {.forwards=true, .lead=0, .maxSpeed = 100, .minSpeed = 0}, false);

    /*
    pros::delay(1700);
    mogo_mech.set_value(false);
    pros::delay(400); // letting goal settle

    // ** Scoring with left goal ** //
    chassis.turnToHeading(275, 900);
    intake.move(127);
    chassis.moveToPose(-55, 25, 275, 5000, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);
    pros::delay(400);
    chassis.moveToPose(-48, 28, 310, 1000, {.forwards=false, .maxSpeed=127, .minSpeed=60}, false);

    // Scoring side rings
    chassis.turnToHeading(180, 900);
    chassis.moveToPose(-48, 10, 180, 5000, {.forwards=true, .maxSpeed=127, .minSpeed=60}, false);
    pros::delay(400);

    // ** Putting left goal in corner ** //
    chassis.turnToHeading(90, 900);
    chassis.moveToPose(-58, 10, 110, 5000, {.forwards=false, .lead=0.3, .maxSpeed=127, .minSpeed=60}, false);
    mogo_mech.set_value(true);
    pros::delay(400); // let it settle

    // chassis.setPose(-157.498, 156.516, 0);

    // chassis.follow(
    //     bigmove_txt,
    //     12, s
    //     10000,
    //     true, 
    //     false
    // );
    */
    pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "................");
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

