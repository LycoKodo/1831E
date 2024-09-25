#ifndef SELECTOR_H
#define SELECTOR_H

#include "api.h"
#include "liblvgl/core/lv_event.h"
#include "liblvgl/lvgl.h"
#include "liblvgl/core/lv_disp.h"
#include "liblvgl/extra/widgets/tabview/lv_tabview.h"
#include "liblvgl/misc/lv_anim.h"
#include "liblvgl/misc/lv_area.h"
#include "liblvgl/misc/lv_types.h"
#include "liblvgl/widgets/lv_btnmatrix.h"

namespace lemlib::selector {

extern int auton;                     // Active autonomous mode
extern int autonCount;                // Number of autonomous modes
extern const char* btnmMap[];         // Array of button matrix labels

extern lv_obj_t* tabview;             // Tab view object
extern lv_obj_t* redBtnm;             // Red button matrix
extern lv_obj_t* blueBtnm;            // Blue button matrix

extern pros::task_t tabWatcher_task;   // Task for monitoring tab changes

// Function prototypes
void init(int hue, int default_auton, const char** autons);
void destroy();
int tabWatcher();
void redBtnmAction(lv_event_t* e);
void blueBtnmAction(lv_event_t* e);
void skillsBtnAction(lv_event_t* e);

} // namespace lemlib::selector

#endif // SELECTOR_H
