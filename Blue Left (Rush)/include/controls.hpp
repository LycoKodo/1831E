#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "main.h"

// Variables to keep track of alliance and selection status
extern char alliance;
extern bool allianceConfirmed;
extern double lady_zero;
extern double lady_err;
extern double lady_pos;

// Function declarations
void setColorSort();
void confirmSelection();
bool ringInspect();
void opcontrol();
void graphOdom();
void Intake_SortedMove(int voltage, float msDelay, int penaltyFactor, bool async);

// Controls
void intake_control();
void mogo_control();
void doinker_control();
void drivetrain_control();
void ladyctl();
void endgame_control();

#endif // CONTROLS_HPP
