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



// THIS CODE IS POWERED BY A PROJECT MANAGER AND TWO DORITO FANS POWERED BY 

/*
         __                              ___   __        .ama     ,
      ,d888a                          ,d88888888888ba.  ,88"I)   d
     a88']8i                         a88".8"8)   `"8888:88  " _a8'
   .d8P' PP                        .d8P'.8  d)      "8:88:baad8P'
  ,d8P' ,ama,   .aa,  .ama.g ,mmm  d8P' 8  .8'        88):888P'
 ,d88' d8[ "8..a8"88 ,8I"88[ I88' d88   ]IaI"        d8[         
 a88' dP "bm8mP8'(8'.8I  8[      d88'    `"         .88          
,88I ]8'  .d'.8     88' ,8' I[  ,88P ,ama    ,ama,  d8[  .ama.g
[88' I8, .d' ]8,  ,88B ,d8 aI   (88',88"8)  d8[ "8. 88 ,8I"88[
]88  `888P'  `8888" "88P"8m"    I88 88[ 8[ dP "bm8m88[.8I  8[
]88,          _,,aaaaaa,_       I88 8"  8 ]P'  .d' 88 88' ,8' I[
`888a,.  ,aadd88888888888bma.   )88,  ,]I I8, .d' )88a8B ,d8 aI
  "888888PP"'        `8""""""8   "888PP'  `888P'  `88P"88P"8m"
*/
// AND
/*                                                                                                                                    
                                                                                                                                             
                                              -                                                                              
                                             --                                                                              
                                            --.-                                                                             
                                           -- .:-                                                                            
                                          -- ...:-                                                                           
                                          - .....--                                                                          
                                        -:...... --                                                                         
                                        -  .  ... --                                                                        
                                      -   ...  ..   -                                                                        
                                      -:   ....  ..  -                                                                       
                                     -- .. ....  . . .-                                                             
                                    :- . .  ... ......--                                                              
                                    -... . .... ... .. --                                                            
                                  --:..... ... .. ..... --                                                      
                                  -.....  .... ......... --                                                                 
                                 -- ..   ....... ....  . --                                                                  
                                -- ..   ....  ............:-                                                                 
                               .- .. ..  ....  .... .......--                                                                
                               -.... . ..... ....  . .......--                                                               
                              -..... ............... .... .. --                                                              
                             -:....... ..... ........... .... --                                                             
                            -- .... ....... . ................ -                                                             
                           --  ........ ................. ..... -                                                            
                          ----------------------------------------                                                                                                                                   
*/

// ----------------------------------------------------
// IMPORTANT - Autonomous Selector Routine Declaration
// ----------------------------------------------------

LV_IMG_DECLARE(image); //TKSRC Logo

// TODO - Temporary
void autonwatcher() {
    while (true) {
        // printf("Selected Auton: %i\n", lemlib::selector::auton);
        pros::delay(10);
    }
}


void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(); // calibrate sensors

    // the default rate is 50. however, if you need to change the rate, you
    // can do the following.
    // lemlib::bufferedStdout().setRate(...);
    // If you use bluetooth or a wired connection, you will want to have a rate of 10ms

    // for more information on how the formatting for the loggers
    // works, refer to the fmtlib docs

    // thread to for brain screen and position logging
    pros::Task screenTask([&]() {
        while (true) {
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            // log position telemetry
            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
            // delay to save resources
            pros::delay(50);
        }
    });
}


void disabled() {} // @note -> Potentially for selecting clsrt (dont trust me bro fr)


void competition_initialize() {}

ASSET(GoalMovement_txt);
ASSET(CornerS_txt);

void autonomous() {}

void opcontrol() {
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    // intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);

    // DON'T CHANGE!: Multi-treading for robot controls (To prevent color sort interruption)
    pros::Task intakeTask(intake_control); // Interrupted by color sort
    pros::Task mogoTask(mogo_control);
    pros::Task driveTask(drivetrain_control);
    pros::Task ladyTask(ladyctl);
    pros::Task doinkerTask(doinker_control);
    pros::Task intakeLiftTask(intake_lift_control);
    // pros::Task endgameTask(endgame_control);
}
