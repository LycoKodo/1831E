
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
}


void disabled() 
{
    // womp womp cant walk
}


void competition_initialize() {
}

ASSET(RedGoalRush_txt)
ASSET(BlueLeftMiddle_txt)
//OPTIMAL Se-TIME for 24 inch (1 tile): 1900
void autonomous() 
{
    //Red Right Rush
    /*chassis.setPose(-51.763, -60, 270);
    chassis.moveToPose(-3, -60, 270, 1000, {.forwards = false, .maxSpeed=200}, false);
    chassis.turnToHeading(210, 500);
    chassis.moveToPose(0, -33, 210, 500, {.forwards = false, .maxSpeed=200}, false);
    pros::delay(200);
    mogo_mech.set_value(false);
    pros::delay(700);
    LadyMovePID(-7000, 100);
    intake.move(127);
    chassis.moveToPose(-20, -60, 210, 1000, {.forwards = true, .maxSpeed=200}, false);*/

    chassis.setPose(0, 0, 300);
    chassis.moveToPose(35, -17.5, 300, 1000, {.forwards = false, .maxSpeed=200}, false);
    mogo_mech.set_value(false);
    pros::delay(1000);
    LadyMovePID(-9000, 500);
    intake.move(127);
    pros::delay(400);
    intake.move(0);
    pros::delay(500);
    chassis.turnToHeading(80, 800, {.maxSpeed = 127}, false);
    chassis.setPose(-23.716, 24.605, 50);
    intake.move(127);
    chassis.follow(BlueLeftMiddle_txt, 6, 5000);
    

    
}


void opcontrol() 
{
    lady_rotation.set_position(0);
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    
    // DON'T CHANGE!: Multi-treading for robot controls (To prevent color sort interruption)
    
    pros::Task intakeTask(intake_control); // Interrupted by color sort
    pros::Task mogoTask(mogo_control);
    pros::Task driveTask(drivetrain_control);
    pros::Task ladyTask(ladyctl);


}

