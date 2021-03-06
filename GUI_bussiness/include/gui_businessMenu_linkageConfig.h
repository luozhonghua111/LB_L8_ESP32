/**
 * @file gui_businessMenu_linkageConfig.h
 *
 */

#ifndef GUI_BUSINESSMENU_LINKAGECONFIG_H
#define GUI_BUSINESSMENU_LINKAGECONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "iot_lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a test screen with a lot objects and apply the given theme on them
 * @param th pointer to a theme
 */
void lvGui_businessMenu_linkageConfig(lv_obj_t * obj_Parent);

void guiDispTimeOut_pageLinkageCfg(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*GUI_BUSINESSMENU_LINKAGECONFIG_H*/


