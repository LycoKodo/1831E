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
ASSET(RushMovement_txt)
ASSET(LadyRUsh_txt)
//OPTIMAL Se-TIME for 24 inch (1 tile): 1900
void autonomous() 
{
    // Red Right Rush Mirrored
    chassis.setPose(-62.596, 9.546, 214); // Mirrored pose

    lady.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    LadyMovePID(-13000, 1200, false); // Adjusted for mirrored x

    LadyMovePID(-9000, 2000, true);

    chassis.follow(
        RushMovement_txt,
        10,
        3000, 
        false,
        false
    );
    // chassis.moveToPoint(-14, 29.35, 500, {.forwards=false, .maxSpeed=127, .minSpeed=25}, true); // *** USED ONLY FOR BAD MOGO *** //

    mogo_mech.set_value(false);

    pros::delay(100);
    chassis.turnToHeading(71.3, 1500); // Mirrored heading

    intake.move(127);
    chassis.moveToPose(-13.21, 38, 42, 3000, {.forwards=true, .lead=0.05, .maxSpeed=127, .minSpeed=50}, false);
    
    chassis.swingToHeading(53, DriveSide::RIGHT, 1000, {.maxSpeed=100, .minSpeed=45}, false); // Minspeed issue
    pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "..");
    pros::delay(600);



    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);

    chassis.moveToPose(-28, 27.7, 2.34, 1200, {.forwards=false, .lead=0, .maxSpeed=127, .minSpeed=35}, false);
    intake.move(127);

    chassis.moveToPose(-28.5, 37.5, -4, 2200, {.forwards=true, .lead=0, .maxSpeed=127, .minSpeed=60}, false);

    pros::delay(100);

    // ** COMMENTED TO TEST POSITION *** //

    chassis.moveToPose(-25, 14, 154, 20000, {.forwards=true, .lead=0.2, .maxSpeed=127, .minSpeed=40}, false);
    intake.move(127);

    LadyMovePID(-18000, 2000, true);    
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

