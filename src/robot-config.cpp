#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/device.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include <sys/wait.h>

// ---------------------------------------
// Drivetrain & Chassis
// ---------------------------------------

pros::MotorGroup leftMotors({-2, -3, -7}, pros::MotorGearset::blue); // left motor group - ports 3 (reversed), 4, 5 (reversed)
pros::MotorGroup rightMotors({8, 9, 10}, pros::MotorGearset::blue); // right motor group - ports 6, 7, 9 (reversed)

// PID settings
lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              15.2, // 11.6 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 3"25' omnis
                              450, // drivetrain rpm is 200 (green direct)
                              4 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// ---------------------------------------
// Subsystem Configuration
// ---------------------------------------

pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::MotorGroup intake({1, -6}, pros::MotorGearset::green); // front 1, back 6
pros::Motor lady(5);
pros::adi::DigitalOut mogo_mech (8);
// pros::adi::DigitalOut lift ();

// ---------------------------------------
// Sensors (Miscellaneous)
// ---------------------------------------

pros::Imu imu(11);
pros::Optical colorSort (18);
//Hollow8KnightCM
pros::Rotation lady_rotation (12);

// ---------------------------------------
// Odometry
// ---------------------------------------

pros::Rotation horizontal_encoder(-1); // Change to the "A" tagged encoder

pros::Rotation vertical_encoder(-17);

lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_encoder, lemlib::Omniwheel::NEW_2, -1.25);

lemlib::TrackingWheel vertical_tracking_wheel(&vertical_encoder, lemlib::Omniwheel::NEW_2, +1.25);

lemlib::OdomSensors sensors(nullptr, // vertical tracking wheel
                            nullptr, // vertical tracking wheel 2, set to nullptr as we don't have a second one
                            nullptr, // &horizontal_tracking_wheel
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu     // inertial sensor &imu
);

// ---------------------------------------
// PID Controller
// ---------------------------------------

lemlib::ControllerSettings linearController(  8.5, // proportional gain (kP)
                                              0.00, // integral gain (kI) 0.42
                                              0.0, // derivative gain (kD) 0.3
                                              0, // anti windup
                                              1, // small error range, in inches
                                              1200, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              2000, // large error range timeout, in milliseconds
                                              127 // maximum acceleration (slew)
);

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

// ---------------------------------------
// ExpoDrive Constraints
// ---------------------------------------

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