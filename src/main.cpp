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
        printf("Selected Auton: %i\n", lemlib::selector::auton);
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
    
    setColorSort();
    
    //red positive
    
    // chassis.setPose(0, 0, 45);

    // ladySmart.movePID(-20000, 900, 10, false);
    // ladySmart.movePID(9000, 2000, 10, true);

    // chassis.moveToPose(-6, -6, 45, 1000, {.forwards = false}, false);

    // chassis.turnToHeading(100, 500);
    
    // sos.set_value(true);

    // roller.move_velocity(80);

    // chassis.moveToPose(10, -9, 100, 800, {.forwards = true, .minSpeed = 20}, false);

    // pros::delay(150);

    // sos.set_value(false);

    // chassis.moveToPose(0, -4, 100, 800, {.forwards = false}, false);

    // pros::delay(100);

    // //chassis.moveToPose(-12, -46, 25, 1300, {.forwards = false}, false);

    // chassis.moveToPose(-6, -37, 25, 1400, {.forwards = false}, false);

    // chassis.moveToPose(-16, -47, 25, 850, {.forwards = false}, false);

    // mogo_mech.set_value(true);
    
    // pros::delay(200);

    // chassis.turnToHeading(260, 900);

    // Intake_SortedMove(127, 6000, -1, true); // TODO - TEMPORARY

    // chassis.moveToPose(-29, -36, 260, 1000, {.forwards = true, .minSpeed = 20}, false);

    // pros::delay(1350);

    // mogo_mech.set_value(false);

    // intake.move(-127);

    // chassis.turnToHeading(0, 800);

    // chassis.moveToPose(-27, -60, 15, 18000, {.forwards = false, .minSpeed = 20}, false);

    // chassis.moveToPose(-27.5, -63, 15, 800, {.forwards = false, .minSpeed = 20}, false);

    // mogo_mech.set_value(false);

    // pros::delay(1400);

    // chassis.moveToPose(-46, -10, 45, 2500, {.forwards = true}, false);

    chassis.setPose(0, 0, -53);
    
    ladySmart.movePID(-20000, 900, 10, false);
    ladySmart.movePID(10000, 1000, 10, true);

    chassis.moveToPose(17, -46, -33, 2000, {.forwards = false, .minSpeed = 30}, false);


    mogo_mech.set_value(true);

    pros::delay(400);

    chassis.turnToHeading(160, 800);

    intake.move_velocity(127);

    chassis.moveToPose(38, -56, -256, 4000, {.forwards = true}, false);

    
    pros::delay(1000);
    
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    
    /*chassis.setPose(0, 0, 34); 

    chassis.moveToPose(-15, -23, 33, 1100, {.forwards = false}, false);

    mogo_mech.set_value(true);
    pros::delay(150);
    
    chassis.turnToHeading(160, 800);

    Intake_SortedMove(127, 10000, -1, true); // TODO - TEMPORARY

    chassis.moveToPose(0, -38, 130, 1500, {.forwards = true}, false);

    chassis.moveToPose(17, -37, 110, 1400, {.forwards = true, .minSpeed = 20}, false);

    chassis.turnToHeading(330, 800);

    chassis.moveToPose(0, -25, 270, 2700, {.forwards = true, .minSpeed = 20}, false);
    
    chassis.turnToHeading(290, 800);

    chassis.moveToPose(-43, 2, 290, 3000, {.forwards = true, .minSpeed = 20}, true);

    pros::delay(1150);

    sos.set_value(true);

    chassis.turnToHeading(0, 900);

    lady.move(100);
    pros::delay(400);
    sos.set_value(false);
    lady.move(0);
    lady_rotation.set_position(0);
    ladySmart.movePID(-1400, 1050, 0.5, true);
    lady.brake();

    intake.move(127);

    chassis.moveToPose(-38, 10, 0, 1300, {.forwards = true, .minSpeed = 60}, false);

    pros::delay(400);

    intake.move(0);

    lady.move(-100);
    lady.move(-100);*/

    // STUFF
    // chassis.setPose(0, 0, 47); 
    
    // lady.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    // ladySmart.movePID(-19529, 800, 10, false);

    // TODO

    // lady.move_velocity(-127);
    // pros::delay(1000);
    // lady.move_velocity(40);

    // chassis.moveToPose(-12, -12, 47, 800, {.forwards = false, .minSpeed = 20}, false);

    // chassis.turnToHeading(90, 1200);

    // sos.set_value(true);

    // roller.move_velocity(127);

    // chassis.moveToPose(23, -9, 90, 1000, {.forwards = true, .minSpeed = 20}, false);

    // sos.set_value(false);

    // lady.move_velocity(0);

    // chassis.moveToPose(-8, -32, 40, 1500, {.forwards = false, .minSpeed = 20}, false);

    // chassis.moveToPose(-10, -35, 40, 500, {.forwards = false, .minSpeed = 20}, false);

    // mogo_mech.set_value(true);
    
    // pros::delay(100);
    
    // Intake_SortedMove(127, 10000, -1, true); // TODO - TEMPORARY

    // pros::delay(200);

    // chassis.turnToHeading(240, 800);

    // chassis.moveToPose(20, -35, 240, 2000, {.forwards = true, .minSpeed = 20}, false);

    



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
