
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

void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    colorSort.set_led_pwm(100);
    // Selecting Alliance Color
    pros::lcd::register_btn0_cb(setColorSort);
    pros::lcd::print(3, "[!] ALLIANCE NOT SELECTED, (MID BTN)");

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


void disabled() 
{
    // womp womp cant walk
}


void competition_initialize() {
}

//OPTIMAL Se-TIME for 24 inch (1 tile): 1900
void autonomous() 
{
    chassis.setPose(0,0,0);
}


void opcontrol() 
{
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    bool intake_spinning = true;
    bool mogo_pis = false;
    bool toggle = false;
    bool toogle_end = false;
    bool latch = false;
    bool latch_end = false;
    
    while (true) {
        chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
        
        // ---------------------------------------
        // Intake Controls
        // ---------------------------------------

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
            intake.move(-127); // Spin out
            intake_spinning = false;
            printf("Intake Activitated\n");
        }
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
            bool passed = ringInspect();
            if (passed) {
                intake.move(127); // spin in
                printf("Intake (INSPECT) Activitated\n");
                printf("    PASSED: True \n");
            }
            else {
                intake.move(-50);
                printf("Intake (INSPECT) COMPROMISED [!]\n");
                printf("    PASSED: False \n");
                pros::delay(50);
            }

            intake_spinning = false;
        }
        else if (intake_spinning == false) {
            intake.brake();
            intake_spinning = true;
        }

        // ---------------------------------------
        // Mobile Goal Mechanism Controls
        // ---------------------------------------

        bool b_button = master.get_digital(pros::E_CONTROLLER_DIGITAL_R2);

        if (b_button && !latch) {
            toggle = !toggle;             // Flip the toggle state
            mogo_mech.set_value(toggle);  // Update mogo_mech based on the new toggle state
            latch = true;                 // Engage latch to prevent repeated toggles
        } else if (!b_button) {
            latch = false;                // Reset latch when button is released
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

        if (chassis_mode_hold) {
            leftMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            chassis.variableCurvature(leftY, rightX, 0.52, false); // Reduced velocity curvature
        } else {
            leftMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            rightMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            chassis.curvature(leftY, rightX);                     // Regular curvature
        }

        pros::delay(10);
    }
}

