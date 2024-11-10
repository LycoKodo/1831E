#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/device.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include <sys/wait.h>

// motor groups
pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::MotorGroup leftMotors({-14, -13, -11}, pros::MotorGearset::blue); // left motor group - ports 3 (reversed), 4, 5 (reversed)
pros::MotorGroup rightMotors({10, 2, 15}, pros::MotorGearset::blue); // right motor group - ports 6, 7, 9 (reversed)

// ------------ //
// Other Motors //
// ------------ //

pros::MotorGroup intake({-16, 7}, pros::MotorGearset::blue);

// pros:: Motor smthing else(+-PORT, MotorGearset);

// ---------- //
// PNEUMATICS //
// ---------- //

pros::adi::DigitalOut mogo_mech (8);

// DEPRECATED - pros::adi::DigitalOut endgame (7);

// Inertial Sensor on port 9

pros::Imu imu(9);

// ------------ //
// ODOM SENSORS //
// ------------ //

//TODO: ------ Initialise odometry sensors and configure odometry ------------------------------------ //

pros::Rotation horizontal_encoder(-1); // Change to the "A" tagged encoder

pros::Rotation vertical_encoder(-17);

lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_encoder, lemlib::Omniwheel::NEW_2, -1.25);

lemlib::TrackingWheel vertical_tracking_wheel(&vertical_encoder, lemlib::Omniwheel::NEW_2, +1.25);
// Full chassis:
    // Across: 17.5
    // Vertical: 14.5

// Tracking Center:
    // X = 8.75
    // y = 7.25

// horizontal odom wheel
    // x = 10
    // y = 6

// vertical odom wheel
    // x = 5.7

// ------------------------------------------------------------------------------------------------ //


// sensors for odometry (No need for change timmy :D)
lemlib::OdomSensors sensors(&vertical_tracking_wheel, // vertical tracking wheel
                            nullptr, // vertical tracking wheel 2, set to nullptr as we don't have a second one
                            &horizontal_tracking_wheel, // &horizontal_tracking_wheel
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu     // inertial sensor &imu
);
    
// PID settings
lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              15.2, // 11.6 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 3"25' omnis
                              360, // drivetrain rpm is 200 (green direct)
                              5 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// ------------------------------------------------------------------------------------------------- // 

// TODO: Tune PID

// lateral motion controller (DrivePID)
lemlib::ControllerSettings linearController(  8.5, // proportional gain (kP)
                                              0.00085, // integral gain (kI) 0.42
                                              0.6, // derivative gain (kD) 0.3
                                              0, // anti windup
                                              1, // small error range, in inches
                                              1200, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              2000, // large error range timeout, in milliseconds
                                              127 // maximum acceleration (slew)
);

// Now same thing for turning
// lateral motion controller (TurnPID)
lemlib::ControllerSettings angularController(1.9, // proportional gain (kP)
                                             0.000020, // integral gain (kI)
                                             7.6, // derivative gain (kD)
                                             0, // anti windup
                                             0.5, // small error range, in degrees
                                             1000, // small error range timeout, in milliseconds
                                             2, // large error range, in degrees
                                             5000, // large error range timeout, in milliseconds
                                             127 // maximum acceleration (slew)
); 

// ------------------------------------------------------------------------------------------------- // 

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttleCurve(0, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1 // expo curve gain 1.019 (LOG 0.9991)
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steerCurve(0, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.010 // expo curve gain org 1.019
);

lemlib::Chassis chassis(drivetrain, linearController, angularController, sensors, &throttleCurve, &steerCurve);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
bool battery_cap_low = false;

void initialize() {
    pros::lcd::initialize(); // initialize brain screen  d
    chassis.calibrate(); // calibrate sensors
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    pros::Task screenTask([]() {
        while (true) {
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
            pros::delay(50);
        }
    });
    mogo_mech.set_value(true);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() 
{
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off. 
 */

//OPTIMAL Se-TIME for 24 inch (1 tile): 1900

ASSET(switch_txt);

void autonomous() 
{
    chassis.setPose(0,0,0);

    mogo_mech.set_value(true);

    chassis.moveToPose(15, 0, 0, 2000, {.forwards = false, .lead=0, .maxSpeed=90}, false);

    mogo_mech.set_value(false);

    intake.move(-127);

    pros::delay(800);

    chassis.turnToHeading(0, 800);

    chassis.moveToPose(15, 0, 0, 2000, {.forwards = false, .lead=0, .maxSpeed=90}, false);

    


    // //-- Scoring Preload --//

    // chassis.moveToPose(5, 30, 120, 3000, {.forwards = false}, false);

    // pros::delay(500);

    
    // mogo_mech.set_value(false);

    // pros::delay(500);

    // intake.move(-127);

    // pros::delay(2000);

    // //-- Getting 2nd Ring --//

    // chassis.turnToHeading(270, 2000);

    // chassis.moveToPose(30, 30, 270, 2200, {.forwards = true, .lead=0}, false);

    // pros::delay(4000);

    // intake.move(0);


    //-- Option 2 --//

    // // Getting Goal // 

    pros::delay(2000);
    pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "..");
}



/**
 * Runs the operator control code. This fun ction will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */


/**
 * Runs in driver control
 */
void opcontrol() 
{
    // controller
    // loop to continuously update motors

    // pros::Controller master(pros::E_CONTROLLER_MASTER);
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);

    // leftMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    // rightMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    bool intake_spinning = true;
    bool mogo_pis = false;
    bool toggle = false;
    bool toogle_end = false;
    bool latch = false;
    bool latch_end = false;
    
    while (true) {

        chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);

        // --------------- //
        // intake controls //
        // --------------- //

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L1))
        {
            intake.move(127); // Spin forward
            intake_spinning = false;
        }
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2))
        {
            intake.move(-127); // Spin reverse
            intake_spinning = false;
        }
        else if (intake_spinning == false)
        {
            intake.brake();
            intake_spinning = true;
        }
        // --------------- // 



        // --------------- //
        //  Mogo Mech Ctl  //
        // --------------- //

        // When button pressed
            // If button released, run following

        bool b_button = master.get_digital(pros::E_CONTROLLER_DIGITAL_R2);

        if (toggle)
        {
            mogo_mech.set_value(true); // turns clamp solenoid on
        }
        else 
        {
            mogo_mech.set_value(false); // turns clamp solenoid off
        }

        if (b_button) 
        {
            if(!latch)
            { // if latch is false, flip toggle one time and set latch to true
                toggle = !toggle;
                latch = true;
            }
        }
        else 
        {
            latch = false; //once button is released then release the latch too
        }

          // ------------------------------- //
         //  EXPERIMENTAL - INSTA BREAK    //
        // ----------------------------- //

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
            leftMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        }

        // ---------------- //
        //  Controller Ctl  //
        // ---------------- //

        int leftY = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        // move the chassis with curvature drive
        chassis.curvature(leftY, rightX);


        // --------------- //
        //   Endgame Ctl   //
        // --------------- //

        // bool c_button = master.get_digital(pros::E_CONTROLLER_DIGITAL_A);

        // if (toogle_end)
        // {
        //     endgame.set_value(true); // turns clamp solenoid on
        // }
        // else 
        // {
        //     endgame.set_value(false); // turns clamp solenoid off
        // }

        // if (c_button) 
        // {
        //     if(!latch_end)
        //     { // if latch is false, flip toggle one time and set latch to true
        //         toogle_end = !toogle_end;
        //         latch_end = true;
        //     }
        // }
        // else 
        // {
        //     latch_end = false; //once button is released then release the latch too
        // }

        // --------------- // 
        pros::delay(10);
    }
}

