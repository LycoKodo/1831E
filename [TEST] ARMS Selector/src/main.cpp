#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/selector.h"
#include "pros/device.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include <sys/wait.h>

// motor groups
pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::MotorGroup leftMotors({-11, -13, -15}, pros::MotorGearset::blue); // left motor group - ports 3 (reversed), 4, 5 (reversed)
pros::MotorGroup rightMotors({12, 14, 16}, pros::MotorGearset::blue); // right motor group - ports 6, 7, 9 (reversed)

// ------------ //
// Other Motors //
// ------------ //

pros::MotorGroup intake({2, -8}, pros::MotorGearset::blue);

// pros:: Motor smthing else(+-PORT, MotorGearset);

// ---------- //
// PNEUMATICS //
// ---------- //

pros::adi::DigitalOut mogo_mech (8);

pros::adi::DigitalOut endgame (7);

// Inertial Sensor on port 10
pros::Imu imu(1);

// ------------ //
// ODOM SENSORS //
// ------------ //


pros::Rotation horizontal_encoder(3);

// Command to initiate odometry tracking wheel
    // lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_encoder, lemlib::Omniwheel::NEW_325, +3.75);


/* Specific to 1831E Robot V1.0
    tracking center:
        8.75, 7.25

    total chassis
        17.5, 14.5

    horizontal wheel
        7.6, 11
*/    

// drivetrain settings
lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              15.2, // 11.6 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 4" omnis
                              360, // drivetrain rpm is 200 (green direct)
                              5 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// TODO: DrivePID NEEDS TUING
// lateral motion controller
lemlib::ControllerSettings linearController(15, // proportional gain (kP)
                                              0.3, // integral gain (kI)
                                              3, // derivative gain (kD)
                                              7, // anti windup
                                              0.5, // small error range, in inches
                                              1000, // small error range timeout, in milliseconds
                                              2, // large error range, in inches
                                              1000, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);
// angular motion controller

// TODO: TUNE KI, Others should be fine, KI might be too big now but the previous value of 0.01 was too small

lemlib::ControllerSettings angularController(4.0, // proportional gain (kP)
                                             0.30, // integral gain (kI)
                                             21.3, // derivative gain (kD)
                                             7, // anti windup
                                             0.5, // small error range, in degrees
                                             1000, // small error range timeout, in milliseconds
                                             2, // large error range, in degrees
                                             1000, // large error range timeout, in milliseconds
                                             0 // maximum acceleration (slew)
); 

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttleCurve(3, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steerCurve(3, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

// sensors for odometry
lemlib::OdomSensors sensors(nullptr, // vertical tracking wheel
                            nullptr, // vertical tracking wheel 2, set to nullptr as we don't have a second one
                            nullptr, // &horizontal_tracking_wheel
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu // inertial sensor &imu
);

lemlib::Chassis chassis(drivetrain, linearController, angularController, sensors, &throttleCurve, &steerCurve);

void controller_controls()
{
    int leftY = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    int rightX = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
    // move the chassis with curvature drive
    chassis.curvature(leftY, rightX);
}

/**
 * - - - - - - - - - - - - - - - - - - - - - *
 * PRE-MATCH INITIALIZATION - DO NOT DELETE  |
 * - - - - - - - - - - - - - - - - - - - - - *

 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

#define AUTONS "Left", "Right", "Do Nothing" // Names of autons, up to 10

void initialize() {
    // pros::lcd::initialize(); // initialize brain screen | REMOVED FOR SAFETY
    chassis.calibrate(); // calibrate sensors
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);

    const char* b[] = {AUTONS, ""}; // Names of autons, up to 10
    lemlib::selector::init(0, 1, b);

    // pros::Task screenTask([]() {
    //     while (true) {
    //         pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
    //         pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
    //         pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
    //         lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
    //         pros::delay(50);
    //     }
    // }); // COMMENTED FOR SAFETY 
}

/**
 * - - - - - - - - - - - - - - - - - - - - - - *
 * FIELD DISABLED INITIALISE - DO NOT DELETE   |
 * - - - - - - - - - - - - - - - - - - - - - - *
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() 
{}

/**
 * - - - - - - - - - - - - - - - - - - - - - - - - *
 * IMMEDIATE PRE-AUTON INITIALISE - DO NOT DELETE  |
 * - - - - - - - - - - - - - - - - - - - - - - - - *
 
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */

void competition_initialize() {
    // set position to x:0, y:0, heading:0
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);
    chassis.setPose(0, 0, 0);
}

/**
 * - - - - - - - - - - - - - - - - - - - - - *
 * FIELD CONTROLLER ELEMENT - DO NOT DELETE  |
 * - - - - - - - - - - - - - - - - - - - - - *

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

void RedLeft() {
    chassis.moveToPose(24,24,90,2000);
}

void RedRight() {
    chassis.moveToPose(24,24,270,2000);
}

void BlueLeft() {
    chassis.moveToPose(24,0,90,2000);
}

void BlueRight() {
    chassis.moveToPose(24,0,270,2000);
}

void Skills() {
    chassis.turnToHeading(355, 2000);
}

void autonomous() 
{
    switch (lemlib::selector::auton) {
    case 1:
      RedLeft(); //runs red side red left code
      break;
    case 2:
      RedRight(); //runs red side autonomous 2 code
      break;
    case -1:
      BlueLeft(); //runs blue side autonomous 1 code
      break;
    case -2:
      BlueRight(); //runs blue side autonomous 2 code
      break;
    case 0:
      Skills(); //runs skills autonomous code
      break;
  }
}

/**
 * - - - - - - - - - - - - - - - - - - - - - *
 * DRIVER CONTROL ELEMENT - DO NOT DELETE    |
 * - - - - - - - - - - - - - - - - - - - - - *

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
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    bool intake_spinning = true;

    bool mogo_pis = false;
    bool toggle = false;

    bool toogle_end = false;


    bool latch = true;
    bool latch_end = false;
    
    while (true) {

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

        // TODO: Check MaoMao's Toggle code

        
        // bool R2_toggle = master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2);
        // if (R2_toggle) {
        //     toggle = !toggle;
        //     mogo_mech.set_value(toggle);
        // }

        // while (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) // Mogo Mech
        // {
        //     if (mogo_pis)
        //     {
        //         mogo_mech.set_value(true); // Retract piston
        //         mogo_pis = !mogo_pis;
        //         pros::delay(400);
        //     }
        //     else
        //     {
        //         mogo_mech.set_value(false); // Extend piston
        //         mogo_pis = !mogo_pis;
        //         pros::delay(400);
        //     }            
        // }
        
        // --------------- // 


        // --------------- //
        //   Endgame Ctl   //
        // --------------- //

        
        // When button pressed
            // If button released, run following

        bool c_button = master.get_digital(pros::E_CONTROLLER_DIGITAL_A);

        if (toogle_end)
        {
            endgame.set_value(true); // turns clamp solenoid on
        }
        else 
        {
            endgame.set_value(false); // turns clamp solenoid off
        }

        if (c_button) 
        {
            if(!latch_end)
            { // if latch is false, flip toggle one time and set latch to true
                toogle_end = !toogle_end;
                latch_end = true;
            }
        }
        else 
        {
            latch_end = false; //once button is released then release the latch too
        }

        // ---------------- //
        //  Controller Ctl  //
        // ---------------- //

        controller_controls();

        // --------------- // 
        pros::delay(10);
    }
}
