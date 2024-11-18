
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

float kP = 0;
float kI = 0;
float kD = 0;


void setConstraints(float kp, float ki, float kd) {
    kP = kp;
    kI = ki;
    kD = kd;
}

// to be called continuously and calculate velocity at state
// float calculateControlSignal(float error, float kp, float ki, float kd) 

void LadyMovePID(float target, float maxSpeed=127, float minSpeed=0, float maxIntegral, float timeOut, float acceptableError) {
    float current_pos;

    float error;
    float previousError = 0;

    float integral;
    float derivative;

    float output;

    float startTime = pros::millis(); // Start time for timeout check

    while (true) {
        current_pos = lady.get_position(); // Update current position

        // Calculate PID components
        error = target - current_pos;
        integral += error;

        // Anti-windup: Clamp the integral term
        if (integral > maxIntegral) integral = maxIntegral;
        else if (integral < -maxIntegral) integral = -maxIntegral;

        derivative = error - previousError;

        // Calculate output
        output = (kP * error) + (kI * integral) + (kD * derivative);

        // Clamp output to motor limits
        if (output > maxSpeed) output = maxSpeed;
        else if (output < minSpeed) output = minSpeed;

        // Move the motor
        lady.move(output);

        // Check for convergence within acceptable error
        if (fabs(error) <= acceptableError) break;

        // Check for timeout
        if ((pros::millis() - startTime) >= timeOut) break;

        previousError = error;

        pros::delay(20); // Loop delay for consistent timing
    }

    lady.brake();
}