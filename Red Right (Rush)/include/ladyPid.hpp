#ifndef LADY_PID_HPP
#define LADY_PID_HPP

#include "pros/device.hpp"  // For device handling (e.g., motor, sensors)
#include "pros/misc.h"      // For miscellaneous utilities like delay
#include "pros/motors.h"    // For motor control
#include "pros/rtos.hpp"    // For real-time operating system functions

#include "lemlib/api.hpp"    // For LemLib API (presumably your library for robotic control)
#include "lemlib/chassis/chassis.hpp"  // For chassis control (if needed)
#include "lemlib/pid.hpp"    // For PID controller class
#include "robot-config.hpp"  // For robot configuration (motors, sensors, etc.)
#include "controls.hpp"      // For control-related code (e.g., joystick inputs)

// Using lemlib namespace for convenience
using namespace lemlib;

// Declare a PID controller for your system
extern PID ladypid;  // PID controller instance

// Forward declarations of the functions
int LadyMovePID(float target, float timeout, bool async);   // Move to a target position using PID
float calculateControlSignal(float error, float kP, float kI, float kD); // Helper function to calculate control signal

#endif // PID_CONTROL_HPP
