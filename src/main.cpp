#include "main.h"
#include "lemlib/api.hpp"
#include "robot-config.hpp"

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

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
void autonomous() 
{
    auton_init();
    chassis.moveToPose(100, 100, 60, 100);
}

/**
 * Runs the operator control code. This function will be started in its own task
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
    bool latch = false;
    
    while (true) {

        // --------------- //
        // intake controls //
        // --------------- //

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2))
        {
            intake.move(127); // Spin forward
            intake_spinning = false;
            pros::delay(10);
        }
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L1))
        {
            intake.move(-127); // Spin reverse
            intake_spinning = false;
            pros::delay(10);
        }
        else if (intake_spinning == false)
        {
            intake.brake();
            intake_spinning = true;
            pros::delay(10);
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
        //  Intake Lifter  //
        // --------------- //









        // ---------------- //
        //  Controller Ctl  //
        // ---------------- //

        controller_controls();

        // --------------- // 
        pros::delay(10);
    }
}

