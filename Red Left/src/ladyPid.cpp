#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/device.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include "lemlib/pid.hpp"
#include <sys/wait.h>

#include "robot-config.hpp"
#include "controls.hpp"

using namespace lemlib;

PID ladypid(10, 0.4, 50, 0, false); // declare PID (constraints, tuning etc)

// float kP, float kI, float kD, float windupRange, bool signFlipReset

// Calculate Errror

int LadyMovePID(float target, float timeout, bool async) {
    // Record the time when the movement starts

    if (async) {
        pros::Task asynctask([=]() {
            LadyMovePID(target, timeout, false); // Call the synchronous version
        });
        pros::delay(10); // Delay to give the task time to start
        return 2;
    }

    unsigned long start_time = pros::millis();

    // Loop until the target is reached or the timeout occurs
    while (true) {
        // Get the current position from the system
        float current_pos = lady_rotation.get_position();

        // Calculate the error between the target and current position
        float error = target - current_pos;

        // If the error is within an acceptable range, exit the loop
        if (fabs(error) < 0.01) {  // You can adjust the threshold as needed
            break;
        }

        // Check if the timeout has expired
        if (pros::millis() - start_time > timeout) {
            break;
        }

        // Update the PID controller with the error to get the control signal
        float control_signal = ladypid.update(error) / 200;

        // Send the control signal to the movement system (motor/actuator)
        lady.move(control_signal);
        
        // Optionally, add a small delay to prevent too frequent updates (e.g., 10ms)
        printf("PID-Signal: %f | Current: %f | Target: %f \n", control_signal, current_pos, target);
        pros::delay(10);  // Adjust as needed based on your system's requirements
    }

    // After the loop ends (either target reached or timeout), stop the motor
    lady.brake();

    return (fabs(target - lady_rotation.get_position()) < 0.01) ? 1 : 0;  // Return 1 if target reached, 0 if timed out
}










// to be called continuously and calculate velocity at state
// float calculateControlSignal(float error, float kp, float ki, float kd) 

