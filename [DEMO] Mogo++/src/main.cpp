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
                                             0.000015, // integral gain (kI)
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

// ------- ///
 /**
  * Additional Sensors
  *     Mogo - Optical Sensor for mogo_scan()
  */

pros::Distance mogo_dist(1);
pros::Motor testing_mogo (3);

bool mogo_scan() {
    float proximity_lim_low = 20;
    float proximity_lim_high = 55;

    int proximity = mogo_dist.get();

    bool mogo_proximity = (proximity > proximity_lim_low && proximity < proximity_lim_high);

    if (mogo_proximity) {
        return true;
    }

    return false;
}

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
            bool mogo_in_range = mogo_scan();
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            pros::lcd::print(3, "MogoProx: %i", mogo_dist.get());
            if (mogo_in_range) {pros::lcd::print(4, "GoalInRange: True");}
            else {pros::lcd::print(4, "GoalInRange: False");}
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
    // womp womp cant walk
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
void autonomous() 
{
    chassis.setPose(0,0,0);

    chassis.turnToHeading(180, 2500);
    chassis.turnToHeading(0, 2500);



    // chassis.moveToPose(0, 50, 0, 3000);
    // chassis.moveToPose(0, 25, 0, 3000, {.forwards=false});
    // chassis.moveToPose(0, 0, 0, 5000, {.forwards=false});

    // chassis.moveToPoint(0, 50, 3000);
    // chassis.moveToPoint(0, 25, 3000, {.forwards=false});
    // chassis.moveToPoint(0, 0, 3000, {.forwards=false});




    // float goal_location = 12;

    // ///////////////////////

    // chassis.setPose(0,0,0);

    // mogo_mech.set_value(true);

    // intake.move(-127);

    // pros::delay(1500);

    // intake.move(0);

    // // Getting Line

    // chassis.moveToPoint(0, goal_location+1, 1500, {.forwards = true}, false);

    // chassis.turnToHeading(270, 1500);

    // // Getting Mogo

    // chassis.moveToPose(24, goal_location, 270, 1500, {.forwards = false, .lead=0}, false);

    // mogo_mech.set_value(false); // get mogo

    // pros::delay(600);

    // ////////////////

    // chassis.turnToHeading(90, 1500);

    // intake.move(-127);

    // chassis.moveToPose(45, goal_location+1, 90, 2700, {.forwards = true, .lead=0, .maxSpeed=90}, false);

    // intake.move(127);
    // pros::delay(1200);

    // intake.move(-127);
    // pros::delay(300);

    // chassis.moveToPose(62, goal_location+1, 90, 2000, {.forwards = true, .lead=0, .maxSpeed=50}, false);
    // // Putt ing in corner

    // chassis.turnToHeading(345, 1500);

    // chassis.moveToPose(67, -10, 345, 1800, {.forwards = false}, false);

    // mogo_mech.set_value(true); // release mogo

    // pros::delay(700);
    
    // intake.move(50);

    // chassis.moveToPose(60, goal_location + 1, 0, 2500, {.forwards = true}, false); // move out


    // // // Getting Goal 2

    // chassis.turnToHeading(90, 1500);

    // chassis.moveToPose(-25, goal_location - 1, 90, 3000, {.forwards = false, .maxSpeed=100}, false);

    // mogo_mech.set_value(false); // clamp mogo
    // pros::delay(600);

    // chassis.turnToHeading(270, 1500);

    // intake.move(-127);

    // chassis.moveToPose(-53, goal_location - 1, 270, 2800, {.forwards = true}, false);

    // intake.move(127);
    // pros::delay(1200);

    // intake.move(-127);
    // pros::delay(300);

    // chassis.moveToPose(-58, goal_location - 1, 270, 2800, {.forwards = true}, false);

    // chassis.turnToHeading(15, 1500);
    

    // chassis.moveToPose(-65, -10, 15, 2500, {.forwards = false}, false);

    // mogo_mech.set_value(true);
    // pros::delay(600);

    // pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, ".........");
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

        // Check the state of the R2 button on the controller
        bool b_button = master.get_digital(pros::E_CONTROLLER_DIGITAL_R2);

        if (b_button && !latch) {
            if (toggle) {
                if (mogo_scan()) {
                    mogo_mech.set_value(toggle); // Set mogo_mech based on toggle state
                    testing_mogo.move(127); // Move the motor forward at full speed
                    pros::delay(100); // Wait for 100 milliseconds

                    latch = true;  // Engage latch to prevent repeated toggles
                    toggle = !toggle; // Flip the toggle state
                }
            } else {
                mogo_mech.set_value(toggle); // Set mogo_mech based on toggle state
                testing_mogo.move(-60); // Move the motor backward at partial speed
                pros::delay(100); // Wait for 100 milliseconds

                latch = true;  // Engage latch to prevent repeated toggles
                toggle = !toggle; // Flip the toggle state
            }
        } else if (!b_button) {
            latch = false; // Reset latch when button is released
        }


          // ------------------------------- //
         //  EXPERIMENTAL - Variable Ctl   //
        // ----------------------------- //

        int leftY = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        // move the chassis with curvature drive

        // Variable to track the toggle state (true = hold mode, false = coast mode)
        static bool chassis_mode_hold = false;
        static bool latch_dt = false;

        // Check if the R1 button is pressed2
        bool r1_button = master.get_digital(pros::E_CONTROLLER_DIGITAL_R1);

        if (r1_button && !latch_dt) {
            chassis_mode_hold = !chassis_mode_hold;  // Toggle chassis mode
            latch_dt = true;                            // Engage latch to prevent repeated toggles
            // Notifying Driver of change

            if (chassis_mode_hold) {
                pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "..");
            }
            else {
                pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "-");
            }

        } else if (!r1_button) {
            latch_dt = false;                           // Reset latch when button is released
        }

        // Set modes based on the current toggle state
        if (chassis_mode_hold) {
            leftMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            chassis.variableCurvature(leftY, rightX, 0.52, false); // Reduced velocity curvature
        } else {
            leftMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            rightMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            chassis.curvature(leftY, rightX);                     // Regular curvature
        }

        



        //TODO - Make into toggle
        


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
