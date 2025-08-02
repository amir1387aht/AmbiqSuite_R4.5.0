/**
 * @file scrollable_menu.h
 * @brief Vertical scrollable menu with icons and labels
 */
#ifndef SCROLLABLE_MENU_H
#define SCROLLABLE_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL FUNCTIONS
 **********************/
static void home_action(void);

/**
 * Initialize and create the vertical scrollable menu
 * @return Pointer to the created menu container object
 */
lv_obj_t * screen_menu_create(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SCROLLABLE_MENU_H */