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
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    mogo_mech.set_value(true);
    doinker.set_value(false);
}


void disabled() 
{
    // womp womp cant walk
}


void competition_initialize() {
}

//OPTIMAL Se-TIME for 24 inch (1 tile): 1900
void autonomous() 
{
    const float goalHeight = 12;
    //Red Right Rush    
    chassis.setPose(0, 0, 0); 

    // intake.move(127);
    // pros::delay(800);
    //chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    
    chassis.moveToPose(0, 32, 0,  900, {.forwards=true, .lead=0, .maxSpeed=127, .minSpeed=30}, false);
    doinker.set_value(true);
    pros::delay(400);
    chassis.moveToPoint(0, 24, 1000, {.forwards=false, .maxSpeed=110, .minSpeed=40}, false);
    doinker.set_value(false);
    chassis.turnToHeading(180, 1000, {.maxSpeed=100});
    chassis.moveToPose(-13, 43, 105,  950, {.forwards=false, .lead=0.2, .maxSpeed=85, .minSpeed=5}, false);
    mogo_mech.set_value(false);
    pros::delay(600);
    LadyMovePID(-14000, 2000, true);
    intake.move(127);
    pros::delay(600);
    intake.move(0);
    roller.move(127);
    chassis.turnToHeading(235, 1000, {.maxSpeed=100});
    chassis.moveToPose(-10, 35, 235, 890, {.forwards=true, .maxSpeed=127, .minSpeed=40}, false);
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
}

