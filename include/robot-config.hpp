#ifndef ROBOT_CONFIG_HPP
#define ROBOT_CONFIG_HPP

#include "main.h"
#include "lemlib/api.hpp"

// motor groups
extern pros::MotorGroup leftMotors;
extern pros::MotorGroup rightMotors;

// Inertial Sensor
extern pros::Imu imu;

// drivetrain settings
extern lemlib::Drivetrain drivetrain;

// lateral motion controller
extern lemlib::ControllerSettings linearController;

// angular motion controller
extern lemlib::ControllerSettings angularController;

// input curve for throttle input during driver control
extern lemlib::ExpoDriveCurve throttleCurve;

// input curve for steer input during driver control
extern lemlib::ExpoDriveCurve steerCurve;

// sensors for odometry
extern lemlib::OdomSensors sensors;

extern lemlib::Chassis chassis;

extern pros::MotorGroup intake;

extern pros::adi::DigitalOut mogo_mech;

extern pros::adi::DigitalOut endgame;

void initialize();
void disabled();
void competition_initialize();
void auton_init();
void controller_controls();

#endif // ROBOT_CONFIG_HPP
