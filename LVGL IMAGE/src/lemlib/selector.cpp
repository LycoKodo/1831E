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

namespace lemlib::selector{

/*
    SECTION 1 - INITIALISING BUTTON MATRIX
*/

int auton;
int autonCount;

void tabWatcher(void* param);

pros::rtos::Task* tabWatcher_task = nullptr;

const char* btnmMap[] = {"", "", "", "", "", "",
                         "", "", "", "", ""}; // up to 10 autons

lv_obj_t* tabview;
lv_obj_t* redBtnm;
lv_obj_t* blueBtnm;

// Traceback logging helper function
void log_error(const std::string& func_name, const std::string& msg) {
    std::cerr << "[ERROR] in function " << func_name << ": " << msg << std::endl;
}

/*
    SECTION 2 - BUTTON ACTION HANDLERS
*/


void redBtnmAction(lv_event_t* e) {
    try {
        lv_obj_t* btnm = lv_event_get_target(e); // Get the button matrix object

		// gettig button id
		uint16_t btn_id = lv_btnmatrix_get_selected_btn(btnm);
		
		const char* txt = lv_btnmatrix_get_btn_text(btnm, btn_id);

        if (txt == nullptr) throw std::runtime_error("No active button text found!");

        printf("Red button: %s released\n", txt);

        for (int i = 0; i < autonCount; i++) {
            if (strcmp(txt, btnmMap[i]) == 0) {
                auton = i + 1;
            }
        }
    } catch (const std::exception& ex) {
        log_error("redBtnmAction", ex.what());
    }
}

void blueBtnmAction(lv_event_t* e) {
    try {
        lv_obj_t* btnm = lv_event_get_target(e); // Get the button matrix object
		
		uint16_t btn_id = lv_btnmatrix_get_selected_btn(btnm);

		const char* txt = lv_btnmatrix_get_btn_text(btnm, btn_id);

        if (txt == nullptr) throw std::runtime_error("No active button text found!");

        printf("Blue button: %s released\n", txt);

        for (int i = 0; i < autonCount; i++) {
            if (strcmp(txt, btnmMap[i]) == 0) {
                auton = -(i + 1);
            }
        }
    } catch (const std::exception& ex) {
        log_error("blueBtnmAction", ex.what());
    }
}

void skillsBtnAction(lv_event_t* e) {
    try {
        printf("Skills button pressed\n");
        auton = 0;
    } catch (const std::exception& ex) {
        log_error("skillsBtnAction", ex.what());
    }
}

/*
    SECTION 3 - [RAN IN THREAD] TABWATCHER REFRESH FUNCTION
*/

void tabWatcher(void* param) {
    try {
        int activeTab = lv_tabview_get_tab_act(tabview);
        while (1) {
			int currentTab = lv_tabview_get_tab_act(tabview);

			if (currentTab != activeTab) {
				activeTab = currentTab;
				if (activeTab == 0) {
					if (auton == 0)
						auton = 1;
					auton = abs(auton);
					lv_btnmatrix_set_btn_ctrl(redBtnm, abs(auton) - 1, LV_BTNMATRIX_CTRL_CHECKED);


					// lv_btnm_set_toggle(redBtnm, true, abs(auton) - 1);
				} else if (activeTab == 1) {
					if (auton == 0)
						auton = -1;
					auton = -abs(auton);
					lv_btnmatrix_set_btn_ctrl(blueBtnm, abs(auton) - 1, LV_BTNMATRIX_CTRL_CHECKED);
					// lv_btnm_set_toggle(blueBtnm, true, abs(auton) - 1);
				} else {
					auton = 0;
				}
			}


            pros::delay(10);
        }
    } catch (const std::exception& ex) {
        log_error("tabWatcher", ex.what());
    }
}

int btn_height = 100;

/*
    SECTION 4 - INITIALISING BUTTON MATRIX
*/

void init(int default_auton, const char** autons) {
    try {
        int i = 0;
        do {
            btnmMap[i] = autons[i];  // Copy string reference instead of memcpy
            i++;
        } while (strcmp(autons[i], "") != 0);

        autonCount = i;
        auton = default_auton;

        lv_theme_t* th = lv_theme_default_init(
            NULL,                        // Pointer to display or NULL for the default display
            lv_color_hsv_to_rgb(255, 255, 255), // Primary color
            lv_color_hsv_to_rgb(255, 0, 255), // Secondary color
            false,                        // Dark mode
            NULL                         // Font, set to NULL for the default font
        );


        lv_disp_set_theme(NULL, th);

        // create a tab view object
        tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);

        // add 3 tabs (the tabs are page (lv_page) and can be scrolled
        lv_obj_t* redTab = lv_tabview_add_tab(tabview, "Red");
        lv_obj_t* blueTab = lv_tabview_add_tab(tabview, "Blue");
        lv_obj_t* skillsTab = lv_tabview_add_tab(tabview, "Skills");

        if (auton < 0) {
            lv_tabview_set_act(tabview, 1, LV_ANIM_OFF); // RED
        } else if (auton == 0) {
            lv_tabview_set_act(tabview, 2, LV_ANIM_OFF);
        }

        redBtnm = lv_btnmatrix_create(redTab);
        lv_btnmatrix_set_map(redBtnm, btnmMap);
        lv_obj_set_size(redBtnm, 450, btn_height);
        lv_obj_set_pos(redBtnm, 0, 100);

        lv_btnmatrix_set_btn_ctrl(redBtnm, 1, LV_BTNMATRIX_CTRL_CHECKABLE);
        lv_btnmatrix_set_btn_ctrl(redBtnm, 2, LV_BTNMATRIX_CTRL_CHECKABLE);
        lv_obj_align(redBtnm, LV_ALIGN_CENTER, 0, 0);

        lv_obj_add_event_cb(redBtnm, redBtnmAction, LV_EVENT_SHORT_CLICKED, NULL);


        blueBtnm = lv_btnmatrix_create(blueTab);
        lv_btnmatrix_set_map(blueBtnm, btnmMap);
        lv_obj_add_event_cb(blueBtnm, blueBtnmAction, LV_EVENT_SHORT_CLICKED, NULL);

        lv_btnmatrix_set_btn_ctrl(blueBtnm, -2, LV_BTNMATRIX_CTRL_CHECKABLE);
        lv_btnmatrix_set_btn_ctrl(blueBtnm, -1, LV_BTNMATRIX_CTRL_CHECKABLE);
        lv_obj_set_size(blueBtnm, 450, btn_height);
        lv_obj_set_pos(blueBtnm, 0, 100);
        lv_obj_align(blueBtnm, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t* skillsBtn = lv_btn_create(skillsTab);
        lv_obj_t* label = lv_label_create(skillsBtn);
        lv_label_set_text(label, "Skills");
        lv_obj_add_event_cb(skillsBtn, skillsBtnAction, LV_EVENT_SHORT_CLICKED, NULL);

        lv_obj_set_size(skillsBtn, 450, btn_height);
        lv_obj_set_pos(skillsBtn, 0, 100);
        lv_obj_align(skillsBtn, LV_ALIGN_CENTER, 0, 0);

        // tabWatcher_task = pros::task_t(tabWatcher); // TODO

		tabWatcher_task = new pros::rtos::Task(tabWatcher, nullptr, "tabWatcher");

    } catch (const std::exception& ex) {
        log_error("init", ex.what());
    }
}

void destroy() {
    if (tabWatcher_task != nullptr) {
        tabWatcher_task->remove(); // Stop and delete the task
        delete tabWatcher_task;
        tabWatcher_task = nullptr;
    }
}



} //namespace lemlib::selector
