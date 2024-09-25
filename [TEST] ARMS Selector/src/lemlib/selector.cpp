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
const char* btnmMap[] = {"", "", "", "", "", "",
                         "", "", "", "", ""}; // up to 10 autons

lv_obj_t* tabview;
lv_obj_t* redBtnm;
lv_obj_t* blueBtnm;

pros::task_t tabWatcher_task = (pros::task_t)NULL;

/*
void redBtnmAction(lv_obj_t* btnm, const char* txt) {
	printf("red button: %s released\n", txt);

	for (int i = 0; i < autonCount; i++) {
		if (strcmp(txt, btnmMap[i]) == 0) {
			auton = i + 1;
		}
	}

	// return LV_RES_OK; // return OK because the button matrix is not deleted
}
*/

void redBtnmAction(lv_event_t* e) {
    lv_obj_t* btnm = lv_event_get_target(e); // Get the button matrix object
    const char* txt = lv_msgbox_get_active_btn_text(btnm); // Get the active button text
    
    printf("Red button: %s released\n", txt);

    for (int i = 0; i < autonCount; i++) {
        if (strcmp(txt, btnmMap[i]) == 0) {
            auton = i + 1;
        }
    }
}

/*
void blueBtnmAction(lv_obj_t* btnm, const char* txt) {
	printf("blue button: %s released\n", txt);

	for (int i = 0; i < autonCount; i++) {
		if (strcmp(txt, btnmMap[i]) == 0) {
			auton = -(i + 1);
		}
	}

	// return LV_RES_OK; // return OK because the button matrix is not deleted
}
*/

void blueBtnmAction(lv_event_t* e) {
    lv_obj_t* btnm = lv_event_get_target(e); // Get the button matrix object
    const char* txt = lv_msgbox_get_active_btn_text(btnm); // Get the active button text
    
    printf("Blue button: %s released\n", txt);

    for (int i = 0; i < autonCount; i++) {
        if (strcmp(txt, btnmMap[i]) == 0) {
            auton = -(i + 1);
        }
    }
}


// void skillsBtnAction(lv_obj_t* btn) {
// 	printf("skills pressed"); // debug msg

// 	auton = 0;

// 	// return LV_RES_OK; // return OK because the button matrix is not deleted
// }

void skillsBtnAction(lv_event_t* e) {
    printf("Skills button pressed\n");

    auton = 0;
}


int tabWatcher() {
	int activeTab = lv_tabview_get_tab_act(tabview);
	while (1) {
		int currentTab = lv_tabview_get_tab_act(tabview);

		if (currentTab != activeTab) {
			activeTab = currentTab;
			if (activeTab == 0) {
				if (auton == 0)
					auton = 1;
				auton = abs(auton);

                lv_btnmatrix_set_btn_ctrl(redBtnm, abs(auton) - 1, LV_BTNMATRIX_CTRL_CHECKABLE);
                    // It seems "lv_btnm_set_toggle" is deprecated, using "lv_btnm_set_btn_ctrl"
				        // OLD LINE - lv_btnm_set_toggle(redBtnm, true, abs(auton) - 1); 

			} else if (activeTab == 1) {
				if (auton == 0)
					auton = -1;
				auton = -abs(auton);
				// lv_btnm_set_toggle(blueBtnm, true, abs(auton) - 1);
                    // Same reason as line 60
                lv_btnmatrix_set_btn_ctrl(blueBtnm, abs(auton) - 1, LV_BTNMATRIX_CTRL_CHECKABLE);
			} else {
				auton = 0;
			}
		}

		pros::delay(10);
	}
}

void init(int hue, int default_auton, const char** autons) {

	int i = 0;
	do {
		memcpy(&btnmMap[i], &autons[i], sizeof(&autons));
		i++;
	} while (strcmp(autons[i], "") != 0);

	autonCount = i;
	auton = default_auton;

	// lvgl theme

    /// OLD DEPRECATED LVGL CODE
            // lv_theme_t* th = lv_theme_alien_init(
            //     hue, NULL); // Set a HUE value and keep font default RED

    lv_theme_t* th = lv_theme_default_init(
        NULL, lv_color_hsv_to_rgb(hue, 100, 100), lv_color_black(), true, NULL);


    // TODO: display set to null, potential issues
    lv_disp_set_theme(NULL, th);
        // lv_theme_set_current(th);

    

	// create a tab view object

    // TODO: Check tab size and position

    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
        // tabview = lv_tabview_create(lv_scr_act(), NULL);

	// add 3 tabs (the tabs are page (lv_page) and can be scrolled
	lv_obj_t* redTab = lv_tabview_add_tab(tabview, "Red");
	lv_obj_t* blueTab = lv_tabview_add_tab(tabview, "Blue");
	lv_obj_t* skillsTab = lv_tabview_add_tab(tabview, "Skills");

	// set default tab
	if (auton < 0) {
        lv_tabview_set_act(tabview, 1, LV_ANIM_OFF);
		    // lv_tabview_set_tab_act(tabview, 1, LV_ANIM_NONE);

	} else if (auton == 0) {
        lv_tabview_set_act(tabview, 2, LV_ANIM_OFF);
		    // lv_tabview_set_tab_act(tabview, 2, LV_ANIM_NONE);
	}

	// add content to the tabs
	// button matrix

    redBtnm = lv_btnmatrix_create(redTab);
        // redBtnm = lv_btnm_create(redTab, NULL);

    lv_btnmatrix_set_map(redBtnm, btnmMap);
	    // lv_btnm_set_map(redBtnm, btnmMap);

    lv_obj_add_event_cb(redBtnm, redBtnmAction, LV_EVENT_SHORT_CLICKED, NULL);
	    // lv_btnmatrix_set_action(redBtnm, redBtnmAction);

    lv_btnmatrix_set_btn_ctrl(redBtnm, abs(auton) - 1, LV_BTNMATRIX_CTRL_CHECKABLE);
        // lv_btnm_set_toggle(redBtnm, true, abs(auton) - 1); // 3

	lv_obj_set_size(redBtnm, 450, 50);
	lv_obj_set_pos(redBtnm, 0, 100);

    lv_obj_align(redBtnm, LV_ALIGN_CENTER, 0, 0);
	    // lv_obj_align(redBtnm, NULL, LV_ALIGN_CENTER, 0, 0);

	// blue tab
	redBtnm = lv_btnmatrix_create(blueTab);
        // blueBtnm = lv_btnm_create(blueTab, NULL);

    lv_btnmatrix_set_map(blueTab, btnmMap);
	    // lv_btnm_set_map(blueBtnm, btnmMap);

    lv_obj_add_event_cb(blueBtnm, blueBtnmAction, LV_EVENT_SHORT_CLICKED, NULL);
	    // lv_btnm_set_action(blueBtnm, *blueBtnmAction);

    lv_btnmatrix_set_btn_ctrl(redBtnm, abs(auton) - 1, LV_BTNMATRIX_CTRL_CHECKABLE);
	    // lv_btnm_set_toggle(blueBtnm, true, abs(auton) - 1);

	lv_obj_set_size(blueBtnm, 450, 50);
	lv_obj_set_pos(blueBtnm, 0, 100);

    lv_obj_align(blueBtnm, LV_ALIGN_CENTER, 0, 0);
	    // lv_obj_align(blueBtnm, NULL, LV_ALIGN_CENTER, 0, 0);

	// skills tab

	lv_obj_t* skillsBtn = lv_btn_create(skillsTab);
        //lv_obj_t* skillsBtn = lv_btn_create(skillsTab, NULL);

	lv_obj_t* label = lv_label_create(skillsBtn);
        // lv_obj_t* label = lv_label_create(skillsBtn, NULL);

	lv_label_set_text(label, "Skills");

    lv_obj_add_event_cb(skillsBtn, skillsBtnAction, LV_EVENT_SHORT_CLICKED, NULL);


        // lv_btn_set_action(skillsBtn, LV_BTN_ACTION_CLICK, *skillsBtnAction);

    // lv_btn_set_state(skillsBtn, LV_BTN_STATE_TGL_REL);
    

	lv_obj_set_size(skillsBtn, 450, 50);
	lv_obj_set_pos(skillsBtn, 0, 100);
    
    lv_obj_align(skillsBtn, LV_ALIGN_CENTER, 0, 0);
	// lv_obj_align(skillsBtn, NULL, LV_ALIGN_CENTER, 0, 0);

	// start tab watcher
	tabWatcher_task = pros::task_t(tabWatcher);
}

void destroy() {
	if (tabWatcher_task != (pros::task_t)NULL) {
		lv_obj_del(tabview);
		tabWatcher_task = (pros::task_t)NULL;
	}
}
    
} //namespace lemlib::selector