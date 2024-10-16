
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
    // See "Timmy's Guide" for more info

pros::Rotation horizontal_encoder(3); // Change to the "A" tagged encoder

lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_encoder, lemlib::Omniwheel::NEW_2, +3.75);

/*
    Line 47 Breakdown:
        &horizontal_encoder - adding new tracking wheel that uses encoder declared on line 45
        lemlib::Omniwheel::NEW_2 - just the diameter/type of wheel, shouldn't need changing
    **** IMPORTANT ****
        +3.75 - the distance of the wheel from the TRACKING CENTER
*/

// basically same thing again for vertical wheel (should be the wheel with less friction)

// TODO: Change tracking center value for second wheel 

pros::Rotation vertical_encoder(4); // Change to the "A" tagged encoder

lemlib::TrackingWheel vertical_tracking_wheel(&vertical_encoder, lemlib::Omniwheel::NEW_2, +3.75); 

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
                              lemlib::Omniwheel::NEW_325, // using new 4" omnis
                              360, // drivetrain rpm is 200 (green direct)
                              5 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// ------------------------------------------------------------------------------------------------- // 

// TODO: Tune PID

// lateral motion controller (DrivePID)
lemlib::ControllerSettings linearController(  8, // proportional gain (kP)
                                              0, // integral gain (kI) 0.42
                                              0, // derivative gain (kD) 1.5
                                              0.1, // anti windup
                                              0.5, // small error range, in inches
                                              2000, // small error range timeout, in milliseconds
                                              2, // large error range, in inches
                                              1500, // large error range timeout, in milliseconds
                                              127 // maximum acceleration (slew)
);

    // Explaination: 
        /**
         * @brief ControllerSettings constructor
         *
         * The constants are stored in a class so that they can be easily passed to the chassis class
         * Set a constant to 0 and it will be ignored
         *
         * @param kP proportional gain
         * @param kI integral gain
         * @param kD derivative gain
         * @param antiWindup integral anti windup range. If error is within this range, integral is set to 0
         * @param smallError range of error at which the chassis controller will exit if the error is within this range
         * for an amount of time determined by smallErrorTimeout
         * @param smallErrorTimeout the time the chassis controller will wait before exiting if error is within a
         * certain range determined by smallError
         * @param largeError range of error at which the chassis controller will exit if the error is within this range
         * for an amount of time determined by largeErrorTimeout
         * @param largeErrorTimeout the time the chassis controller will wait before exiting if error is within a
         * certain range determined by largeError
         * @param slew maximum acceleration
         *
         * @b Example
         * @code {.cpp}
         * lemlib::ControllerSettings lateralSettings(10, // proportional gain (kP)
         *                                            0, // integral gain (kI), set to 0 to disable
         *                                            3, // derivative gain (kD), set to 3
         *                                            3, // integral anti windup range, set to 0 to disable
         *                                            1, // small error range, in inches
         *                                            100, // small error range timeout, in milliseconds
         *                                            3, // large error range, in inches
         *                                            500, // large error range timeout, in milliseconds
         *                                            5); // maximum acceleration (slew)
         * @endcode
         */

// Now same thing for turning
// lateral motion controller (TurnPID)
lemlib::ControllerSettings angularController(4, // proportional gain (kP)
                                             0, // integral gain (kI)
                                             0, // derivative gain (kD)
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
    chassis.setPose(0,0,0, false);
    mogo_mech.set_value(true);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() 
{
    chassis.setPose(0,0,0, false);
    mogo_mech.set_value(true);
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
    // set position to x:0, y:0, heading:0
    chassis.setPose(0,0,0, false);
    mogo_mech.set_value(true);
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
void autonomous() 
{
    // ------------------ //
    // FOR LINEAR TUNING

    chassis.moveToPoint(0, 48, 3000);
    chassis.moveToPoint(0, 24, 2000, {.forwards=false});
    chassis.moveToPoint(0, 0, 2000, {.forwards=false});

    // ------------------ // 


    // // ------------------ //
    // // FOR ANGULAR TUNING

    // chassis.turnToHeading(180, 2500);
    // chassis.turnToHeading(0, 2500);

    // // ------------------ // 


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

