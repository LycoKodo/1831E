#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/device.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include <sys/wait.h>

#include "lemlib/selector.hpp" // For auton selector
#include "robot-config.hpp"
#include "controls.hpp"
#include "autons.hpp"



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
    // ------------------------------------------
    // Displaying TKSRC Logo (During calibration)
    // ------------------------------------------
    lv_obj_t *img = lv_img_create(lv_scr_act()); // Add to screen
    lv_img_set_src(img, &image); // Link to source image
    lv_obj_align(img, LV_ALIGN_CENTER, 0, -20);

    // -----------------------
    // Initialise Chassis
    // -----------------------
    // @removed - LLEMU CANNOT BE USED WITH LVGL | pros::lcd::initialize(); // initialize brain screen

    chassis.calibrate(); // calibrate sensors

    ladySmart.reset();
    // // -----------------------
    // // Initialise Sensors
    // // -----------------------
    // colorSort.set_led_pwm(100);
    // lady_rotation.set_position(0);

    // // -----------------------
    // // Initialise Subsystems
    // // -----------------------
    // intake.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    // mogo_mech.set_value(true);
    doinker.set_value(false);
    sos.set_value(false);

    lv_obj_del(img); // stop displaying TKSRC Logo as Calibration has ended

    // -----------------------
    // TODO - LVGL INIT
    // -----------------------


    static const char* redBtnmMap[] = {
        "R-Auton1", "R-Auton2", "R-Auton3", "\n",
        "R-Auton4", "R-Auton5", "R-Auton6", nullptr
    };
    static const char* redBtnmDesc[] = {
        "R-Auton1", "R-Auton2", "R-Auton3",
        "R-Auton4", "R-Auton5", "R-Auton6", nullptr
    };

    static const char* blueBtnmMap[] = {
        "B-Auton1", "B-Auton2", "B-Auton3", "\n",
        "B-Auton4", "B-Auton5", "B-Auton6", nullptr
    };
    static const char* blueBtnmDesc[] = {
        "R-Auton1", "R-Auton2", "R-Auton3",
        "R-Auton4", "R-Auton5", "R-Auton6", nullptr
    };

    static const char* skillsBtnmMap[] = {
        "Auton", "Preload", "Skills 1", "\n",  
        "Skills 2", "Skills 3", "Skills 4", nullptr
    };
    static const char* skillsBtnmDesc[] = {
        "Auton", "Preload", "Skills 1",  
        "Skills 2", "Skills 3", "Skills 4", nullptr
    };

    lemlib::selector::init(999, redBtnmMap, blueBtnmMap, skillsBtnmMap, redBtnmDesc, blueBtnmDesc, skillsBtnmDesc); // declaring default auton

    // TODO - temporary 
    pros::Task watcher(autonwatcher);

}


void disabled() {} // @note -> Potentially for selecting clsrt (dont trust me bro fr)


void competition_initialize() {}

ASSET(GoalMovement_txt);
ASSET(CornerS_txt);

void autonomous() {  

    // chassis.setPose(0, 0, -53);
    
    // ladySmart.movePID(-20000, 900, 10, false);
    // ladySmart.movePID(10000, 1000, 10, true);

    // chassis.moveToPose(17, -46, -33, 1500, {.forwards = false, .minSpeed = 40}, false);

    // mogo_mech.set_value(true);

    // pros::delay(400);

    // chassis.turnToHeading(-235, 600);

    // Intake_SortedMove(127, 13000, -1, true);

    // chassis.moveToPose(22.3, -49.51, -190, 1600, {.forwards = true, .lead=0.3, .maxSpeed=60, .minSpeed = 0}, false);
    
    // chassis.turnToHeading(-243, 500);

    // chassis.moveToPose(27.5, -53.5, -243, 800, {.forwards = true, .minSpeed = 30}, false);

    // pros::delay(300);

    // chassis.turnToHeading(16, 500);

    // chassis.moveToPose(31, -47, 16, 1000, {.forwards = true, .minSpeed = 30}, false);

    // chassis.turnToHeading(-57, 800);

    // lady.get_voltage();

    // chassis.moveToPose(-8, -17, -57, 2000, {.forwards = true, .minSpeed = 30}, true);

    // pros::delay(700);

    // doinker.set_value(true);

    // pros::delay(1400);

    // chassis.moveToPose(-6, -19, -57, 2000, {.forwards = true, .minSpeed = 30}, false);

    // doinker.set_value(false);

    // chassis.turnToHeading(-148, 800);

    // ladySmart.movePID(-17000, 1000, 10, true);

    // chassis.moveToPose(-10, -25, -148, 2000, {.forwards = true, .minSpeed = 30}, false);

    //Red Negative ^

    // chassis.swingToHeading(90, DriveSide::LEFT, 1000, {.maxSpeed = 100, .minSpeed = 0}, false);





//     chassis.setPose(0, 0, 51);

//     setColorSort();

//     ladySmart.movePID(-20000, 900, 10, false);
//     ladySmart.movePID(10000, 1000, 10, true);

//     chassis.moveToPose(-8, -8, 51, 800, {.forwards = false, .minSpeed = 40}, false);

//     chassis.turnToHeading(108, 800);

//     doinker.set_value(true);

//     roller.move_velocity(80);

//     chassis.moveToPose(8, -9, 103, 1800, {.forwards = true, .minSpeed = 40}, false);

//     pros::delay(400);

//     doinker.set_value(false);

//     chassis.moveToPose(-11, -40, 28, 2000, {.forwards = false, .minSpeed = 40}, false);

//     mogo_mech.set_value(true);
    
//     intake.move_velocity(127);

//     Intake_SortedMove(127, 10000, -1, true);

//     chassis.turnToHeading(270, 900);

//     chassis.moveToPose(-27, -35, 270, 2000, {.forwards = true, .minSpeed = 25}, false);

//     pros::delay(1500);

//     chassis.turnToHeading(180, 1000);

//     mogo_mech.set_value(false);

//     chassis.moveToPose(-27, -44, 180, 2000, {.forwards = true, .minSpeed = 25}, false);

//     sos.set_value(true);
    
//     pros::delay(400);

//     chassis.moveToPose(-27, -30, 270, 2000, {.forwards = false, .minSpeed = 25}, false);

//     pros::delay(500);

//     sos.set_value(false);
// //2644347
//     chassis.turnToHeading(0, 1000);

//     chassis.moveToPose(-27, -47, -345, 3000, {.forwards = false, .minSpeed = 25}, false);

    //RED POSITIVE CODE ^^






    chassis.setPose(0, 0, 0);
    setColorSort();

    chassis.moveToPose(-1.5, 41.5, 0, 2000, {.forwards = true, .lead = 0.03, .minSpeed = 40}, false);

    chassis.turnToHeading(-62, 700);

    ladySmart.movePID(-17500, 1000, 10, true);

    pros::delay(900);
    
    ladySmart.movePID(8000, 2000, 10, true);

    chassis.turnToHeading(-132, 700);

    roller.move_velocity(127);
    
    chassis.moveToPose(-34, 9, -132, 2000, {.forwards = true, .minSpeed = 20}, false);

    chassis.turnToHeading(-222, 700);

    roller.move_velocity(-50);

    chassis.moveToPose(-44, 31, -222, 2000, {.forwards = false, .minSpeed = 20}, true);

    pros::delay(200);
    
    roller.move_velocity(10);
    
    pros::delay(600);

    mogo_mech.set_value(true);

    Intake_SortedMove(127, 10000, -1, true);

    chassis.moveToPose(0, -8, -230, 2000, {.forwards = true, .lead = 0.5, .minSpeed = 10}, false);

    pros::delay(600);

    chassis.moveToPose(-44, 31, -222, 2000, {.forwards = false, .minSpeed = 10}, false);

    doinker.set_value(true);

    // mogo_mech.set_value(false);

    chassis.moveToPose(-60, 12, -125, 1800, {.forwards = true, .minSpeed = 10}, false);
    
    pros::delay(150);
    doinker.set_value(false);

    // lady.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    // lady.move(-100);
    // pros::delay(100);
    // lady.move(0);


    // lady.brake();

    chassis.moveToPose(-52, 19, -125, 800, {.forwards = false, .minSpeed = 10}, false);

    // chassis.moveToPose(-54, -3, -137, 2500, {.forwards = true}, false);

    // lady.move(-127);

    // pros::delay(700);
    // lady.move(50);

    chassis.moveToPose(-52, 19, -40, 800, {.forwards = true, .minSpeed = 10}, false);

    lady.move(-100);



    // sos.set_value(true);         5

    // roller.move_velocity(127);
    
    // chassis.moveToPose(-12, 44, -1, 2500, {.forwards = true, .minSpeed = 30}, false);

    // pros::delay(1000);

    // ladySmart.movePID(-15500, 900, 10, true);

    // pros::delay(700);

    // chassis.turnToHeading(110, 800);

    // roller.move(-110);

    // pros::delay(700);

    // roller.move(0);

    // ladySmart.movePID(15000, 900, 10, true);

    // chassis.moveToPose(-38, 31, 90, 2500, {.forwards = false, .lead = 0.3, .minSpeed = 20}, false);

    // mogo_mech.set_value(true);

    // pros::delay(400);

    // Intake_SortedMove(127, 5000, -1, true);

    // chassis.turnToHeading(232, 800);

    // doinker.set_value(true);

    // roller.move(50);

    // chassis.moveToPose(-58, 15, 230, 2500, {.forwards = true, .lead = 0.3, .minSpeed = 20}, false);

    // pros::delay(1000);

    // sos.set_value(true);
    
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);

    // int auton = lemlib::selector::auton;

    // lemlib::selector::autonStarted = true;
    
    // if (auton >= 1 && auton <= 100) {
    //     switch (auton) {
    //         case 1: red1(); break;
    //         case 2: red2(); break;
    //         case 3: red3(); break;
    //         case 4: red4(); break;
    //         case 5: red5(); break;
    //         case 6: red6(); break;
    //         // add more red cases as needed up to 100
    //     }
    // }
    // else if (auton >= -100 && auton <= -1) {
    //     switch (auton) {
    //         case -1: blue1(); break;
    //         case -2: blue2(); break;
    //         case -3: blue3(); break;
    //         case -4: blue4(); break;
    //         case -5: blue5(); break;
    //         case -6: blue6(); break;
    //         // add more blue cases as needed up to -100
    //     }
    // }
    // else if (auton >= 101 && auton <= 201) {
    //     switch (auton) {
    //         case 101: skills1(); break;
    //         case 102: skills2(); break;
    //         case 103: skills3(); break;
    //         case 104: skills4(); break;
    //         case 105: skills5(); break;
    //         case 106: skills6(); break;
    //         // add more skills cases as needed up to 106
    //     }
    // }
    // else {
    //     // Default auton
    //     auton1ring();
    // }


}

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
