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
    //Red Right Rush    
    chassis.setPose(62.596, 9.546, 360 - 214); // Mirrored pose

    lady.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    LadyMovePID(-13000, 1200, false); // Adjusted for mirrored x

    chassis.follow(
        RushMovement_txt,
        10,
        3000, 
        false,
        false
    );
    // chassis.moveToPose(16.61, 26, 360 - 192.34, 3000, {.forwards=false, .lead=0.15, .maxSpeed=115, .minSpeed=30}, false);
    mogo_mech.set_value(false);
    pros::delay(300);
    chassis.turnToHeading(360 - 48.29, 1500); // Mirrored heading

    intake.move(127);
    chassis.moveToPose(13.21, 37.26, 360 - 48.29, 1200, {.forwards=true, .lead=0, .maxSpeed=115, .minSpeed=30}, false);

    pros::delay(400);

    chassis.moveToPose(28, 27.7, 360 - 2.34, 1200, {.forwards=false, .lead=0, .maxSpeed=127, .minSpeed=30}, false);
    intake.move(127);

    chassis.moveToPose(25, 40, 360, 1600, {.forwards=true, .lead=0, .maxSpeed=127, .minSpeed=30}, false);

    pros::delay(500);
    
    chassis.moveToPose(25, 14, 360 - 154, 20000, {.forwards=true, .lead=0.2, .maxSpeed=127, .minSpeed=40}, false);
    intake.move(127);

    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);

    //Red Left Side
    /*lady.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    chassis.setPose(0, 0, 300);

    
    chassis.turnToPoint(22, -11.534, 250, {.forwards=false}, false);
    chassis.moveToPose(22.6, -11.734, 295.63, 1300, {.forwards = false, .lead=0.5, .maxSpeed=120}, false);
    LadyMovePID(-5564, 200);
    mogo_mech.set_value(false);
    pros::delay(300);
    intake.move(127);
    pros::delay(500);
    intake.move(-127);
    pros::delay(300);

    chassis.turnToHeading(400, 800, {.maxSpeed = 120}, false);
    intake.move(127);
    chassis.moveToPose(41, 6.5, 404, 2000, {.forwards = true,.lead=0.7, .maxSpeed=120, .minSpeed=30}, false);

    chassis.swingToHeading(358, DriveSide::LEFT, 2000, {.maxSpeed=120, .minSpeed=40}, false);
    chassis.moveToPose(39, 21, 358, 2000, {.forwards = true, .lead=0, .maxSpeed=120, .minSpeed=60}, false);
    pros::Motor(-6).move(-127);
    chassis.moveToPose(
        25.8,
        15.52,
        337.8,
        2000,
        {.forwards=true,.lead=0.8, .maxSpeed=115, .earlyExitRange=8},
        false
    );

    pros::Motor(-6).move(127);

    // 13 24 208
    intake_lift.set_value(false);
    chassis.moveToPose(
        7,
        -33,
        118,
        4000,
        {.forwards=true,.lead=0.5, .maxSpeed=127, .minSpeed = 60},
        false
    );
    intake_lift.set_value(true);

    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);*/
    
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
}

