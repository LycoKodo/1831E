#ifndef LADY_MOVE_PID_HPP
#define LADY_MOVE_PID_HPP

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

// PID tuning constants
extern float kP;
extern float kI;
extern float kD;

/**
 * @brief Sets the PID constraints (kP, kI, kD).
 * 
 * @param kp Proportional gain
 * @param ki Integral gain
 * @param kd Derivative gain
 */
void setConstraints(float kp, float ki, float kd);

/**
 * @brief Executes the PID control loop for a motor movement.
 * 
 * @param target The target position to move to.
 * @param maxSpeed The maximum output speed. Default is 127.
 * @param minSpeed The minimum output speed. Default is 0.
 * @param maxIntegral The maximum value for the integral term to prevent windup.
 * @param timeOut The maximum time (in milliseconds) for the operation before timeout.
 * @param acceptableError The threshold of error to consider the target reached.
 */
void LadyMovePID(float target, 
                 float maxSpeed = 127, 
                 float minSpeed = 0, 
                 float maxIntegral = 50, 
                 float timeOut = 5000, 
                 float acceptableError = 1);

#endif // LADY_MOVE_PID_HPP
