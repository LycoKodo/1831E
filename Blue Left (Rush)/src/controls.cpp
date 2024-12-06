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
#include "ladyPid.hpp"

char alliance = 'N';
bool allianceConfirmed = false;

double lady_zero;
double lady_err;
double lady_pos;

void graphOdom() {
    do {
        Pose position = chassis.getPose();
        float x = position.x;
        float y = position.y;

        printf("X: %f| Y: %f \n", x, y);
        pros::delay(10);
    }
    while (chassis.isInMotion());
}

void setColorSort() {
    static bool pressed = false;
    pressed = !pressed;
    if (pressed) {
        pros::lcd::clear_line(3);
        pros::lcd::set_text(3, "[CLS] Alliance: RED");
        alliance = 'R';
    } else {
        pros::lcd::clear_line(3);
        pros::lcd::set_text(3, "[CLS] Alliance: BLUE");
        alliance = 'B';
    }
}

void confirmSelection() {
    static bool pressed = false;
    pressed = !pressed;

    if (pressed) {
        allianceConfirmed = true;
    }
}

bool ringInspect() {
    const float blue_lim_low = 150, blue_lim_high = 250;
    const float red_lim_low = 0, red_lim_high = 50;
    double hue = colorSort.get_hue();

    if (alliance == 'B') {
        return !(red_lim_low < hue && hue < red_lim_high);
    } else if (alliance == 'R') {
        return !(blue_lim_low < hue && hue < blue_lim_high);
    }
    return true; // Default: passes for any other color
}

void Intake_SortedMove(int voltage, float msDelay, int penaltyFactor, bool async) {
    // Get the start time
    if (async) {
        pros::Task task([&]() { Intake_SortedMove(voltage, msDelay, penaltyFactor, false); });
        pros::delay(10); // delay to give the task time to start
        return;
    }

    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = startTime + std::chrono::milliseconds(static_cast<int>(msDelay));

    // While the elapsed time is less than msDelay
    while (std::chrono::high_resolution_clock::now() < endTime) {
        if (voltage > 0) {
            // Intaking
            bool passed = ringInspect();
            if (passed) {
                intake.move(voltage);
            } else {
                intake.move(voltage * penaltyFactor);
            }
        } else {
            // Outtaking or stopping
            intake.move(voltage);
        }
        pros::delay(10);
    }

    // Stop intake after delay
    intake.move(0);
}

void ladyMoveToSTDPose(float target, float acceptableRange) {
    float error = target - lady_rotation.get_angle();
    while (!(error < error + acceptableRange && error > error - acceptableRange)) {
        if (error > 0) { // when error is positive
            lady.move(50);
        }
        else { // when error is negative
            lady.move(-50);
        }
    }
    lady.brake();
}

void ladyctl() {
    bool spinning = false;
    lady.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    int counter = 0;

    while (true) {
        // lady_pos = lady.get_position() - lady_zero;

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
            lady.move(-100);
            spinning = true;
        }
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT)) {
            lady.move(100);
            spinning = true;
        }
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {
            lady.move(100);
            pros::delay(400);
            lady.move(0);
            lady_rotation.set_position(0);
            LadyMovePID(-900, 2000, false);
        }
        else if (spinning == true) {
            lady.brake();
            spinning = false;
        }
        pros::delay(10);
    }
}

void intake_control() {
    const double stuck_lim_low = 0.0;
    const double stuck_lim_high = 0.0;
    const int reverse_time = 200;
    const int forward_delay = 100;

    intake.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    bool intake_spinning = true;
    
    while (true) {
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
            intake.move(-127); // Spin out
            intake_spinning = false;
            printf("Intake Activitated\n");
        }
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
            bool passed = ringInspect();
            if (passed) {
                double velocity = intake.get_actual_velocity();
                if (false) {// velocity < stuck_lim_low && velocity > -stuck_lim_high
                    // Reverse the motor briefly
                    intake.move(-127);
                    pros::delay(reverse_time);
                    // Resume forward spin
                    intake.move(127);
                    pros::delay(forward_delay);
                } else {
                    intake.move(127); // Spin in normally
                }
                printf("Intake (INSPECT) Activitated\n");
                printf("    PASSED: True \n");
            }
            else {
                printf("Intake (INSPECT) COMPROMISED [!]\n");
                printf("    PASSED: False \n");
                intake.move(127);
                pros::delay(220); // TODO - Tune delay accordingly to intake
                intake.brake();
                pros::delay(200);
            }
            intake_spinning = false;
        }
        else if (intake_spinning == false) {
            intake.move(0);
            intake_spinning = true;
        }
        pros::delay(10);
    }
}

void mogo_control() {
    bool toggle = true;
    bool latch = false;

    while (true) {
        bool b_button = master.get_digital(pros::E_CONTROLLER_DIGITAL_R2);

        if (b_button && !latch) {
            toggle = !toggle;             // Flip the toggle state
            mogo_mech.set_value(toggle);  // Update mogo_mech based on the new toggle state
            latch = true;                 // Engage latch to prevent repeated toggles
        } else if (!b_button) {
            latch = false;                // Reset latch when button is released
        }
        pros::delay(10);
    }
}

void doinker_control() {
    bool toggle = true;
    bool latch = false;

    while (true) {
        bool b_button = master.get_digital(pros::E_CONTROLLER_DIGITAL_X);

        if (b_button && !latch) {
            toggle = !toggle;             // Flip the toggle state
            doinker.set_value(toggle);  // Update mogo_mech based on the new toggle state
            latch = true;                 // Engage latch to prevent repeated toggles
        } else if (!b_button) {
            latch = false;                // Reset latch when button is released
        }
        pros::delay(10);
    }
}

void endgame_control() {
    bool toggle = true;
    bool latch = false;

    while (true) {
        bool b_button = master.get_digital(pros::E_CONTROLLER_DIGITAL_Y);

        if (b_button && !latch) {
            toggle = !toggle;             // Flip the toggle state
            endgame.set_value(toggle);  // Update mogo_mech based on the new toggle state
            latch = true;                 // Engage latch to prevent repeated toggles
        } else if (!b_button) {
            latch = false;                // Reset latch when button is released
        }
        pros::delay(10);
    }
}
// ------------------------------
// TODO: Finish these functions

// void lady_smart_control() {
//     lady.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

//     while (true) {
//         bool lower = master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN);
//         bool raise = master.get_digital(pros::E_CONTROLLER_DIGITAL_UP);

//         if (lower) {
//             LadyMovePID(90, 127, 0, 50, 2000, 1);
//         }
//         else if (raise) {
//             LadyMovePID(0, 127, 0, 50, 2000, 1);
//         }
//         else lady.brake();
//     }
// }

// void lady_dumb_control() {

// }

// END TODO
// ------------------------------

void drivetrain_control() {
    while (true) {
        chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
        
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
            if (chassis_mode_hold) pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "..");
            else pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "-");

        } else if (!r1_button) {
            latch_dt = false;                           // Reset latch when button is released
        }

        if (chassis_mode_hold) {
            leftMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            chassis.variableCurvature(leftY, rightX, 0.75, false); // Reduced velocity curvature
        } else {
            leftMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            rightMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            chassis.curvature(leftY, rightX);                     // Regular curvature
        }

        pros::delay(10);
    }
}