#ifndef SELECTOR_HPP
#define SELECTOR_HPP

#include <iostream>
#include <stdexcept>
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

/*
    SECTION 1 - INITIALISING BUTTON MATRIX
*/

extern int auton;
extern int autonCount;

// Function prototype for tabWatcher
int tabWatcher(void* param);

extern pros::rtos::Task* tabWatcher_task;

extern const char* btnmMap[]; // Declaration of btnmMap array

extern lv_obj_t* tabview;
extern lv_obj_t* redBtnm;
extern lv_obj_t* blueBtnm;

// Traceback logging helper function
void log_error(const std::string& func_name, const std::string& msg);

void redBtnmAction(lv_event_t* e);
void blueBtnmAction(lv_event_t* e);
void skillsBtnAction(lv_event_t* e);
int tabWatcher(void* param);
void init(int default_auton, const char** autons);
void destroy();

} // namespace lemlib::selector

#endif // SELECTOR_HPP
