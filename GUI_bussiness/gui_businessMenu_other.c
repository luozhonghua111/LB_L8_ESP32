/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>

// /* freertos includes */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_freertos_hooks.h"

#include "sdkconfig.h"

/* lvgl includes */
#include "iot_lvgl.h"

#include "devDriver_manage.h"

#include "bussiness_timerSoft.h"

#include "gui_businessMenu_other.h"
#include "gui_businessHome.h"

LV_FONT_DECLARE(lv_font_consola_17);
LV_FONT_DECLARE(lv_font_consola_19);

LV_FONT_DECLARE(iconMenu_funBack_arrowLeft);
LV_IMG_DECLARE(imageBtn_feedBackNormal);

static const char strInfoWeek_tab[7][6] = {

	"Mon ", "Tues ", "Wed ", "Thur ", "Fri ", "Sat ", "Sun ",
};

static lv_obj_t *page_sysInfo = NULL;
static lv_obj_t *labelSysInfo_devTime = NULL;
static lv_obj_t *labelSysInfo_devSoftVersion = NULL;
static lv_obj_t *labelSysInfo_devReserveHeap = NULL;
static lv_obj_t *labelSysInfo_devMac = NULL;
static lv_obj_t *labelSysInfo_devParentMac = NULL;
static lv_obj_t *labelSysInfo_devParensRssi = NULL;
static lv_obj_t *labelSysInfo_devMeshNodeNum = NULL;
static lv_obj_t *labelSysInfo_devMeshLayer = NULL;
static lv_obj_t *labelSysInfo_devMutualGroupEnum = NULL;
static lv_obj_t *labelSysInfo_devTemprature = NULL;
static lv_obj_t *labelSysInfo_devPower = NULL;
static lv_obj_t *labelSysInfo_devElecSum = NULL;
static lv_obj_t *labelSysInfo_routerSSID = NULL;
static lv_obj_t *labelSysInfo_routerPSD = NULL;
static lv_obj_t *menuBtnChoIcon_fun_back = NULL;
static lv_obj_t *objText_menuCurrentTitle = NULL;

static lv_style_t labelStyle_sysInfo;
static lv_style_t stylePage_sysInfo;
static lv_style_t styleText_menuLevel_A;

static void currentGui_elementClear(void){

	lv_obj_del(page_sysInfo);
}

static lv_res_t funCb_btnActionClick_menuBtn_funBack(lv_obj_t *btn){

	currentGui_elementClear();
	lvGui_usrSwitch(bussinessType_Menu);

	return LV_RES_OK;
}

static lv_res_t funCb_btnActionPress_menuBtn_funBack(lv_obj_t *btn){

	lv_obj_t *btnFeedBk = lv_img_create(btn, NULL);
	lv_img_set_src(btnFeedBk, &imageBtn_feedBackNormal);

	return LV_RES_OK;
}

void lvGuiOther_styleApplicationInit(void){

	lv_style_copy(&styleText_menuLevel_A, &lv_style_plain);
	styleText_menuLevel_A.text.font = &lv_font_dejavu_20;
	styleText_menuLevel_A.text.color = LV_COLOR_WHITE;

	lv_style_copy(&labelStyle_sysInfo, &lv_style_plain);
	labelStyle_sysInfo.text.font = &lv_font_consola_17;

	lv_style_copy(&stylePage_sysInfo, &lv_style_plain);
	stylePage_sysInfo.body.main_color = LV_COLOR_WHITE;
	stylePage_sysInfo.body.grad_color = LV_COLOR_WHITE;
}

void lvGuiOther_devInfoRefresh(void){

	stt_localTime devParamTemp_infoData = {0};
	uint8_t sta_mac[MWIFI_ADDR_LEN] = {0};
	mesh_addr_t parent_bssid        = {0};
	mesh_assoc_t mesh_assoc 		= {0x0};
	uint8_t mutualGroupInsert_temp[DEVICE_MUTUAL_CTRL_GROUP_NUM] = {0};
	wifi_config_t wifi_config		= {0};

	const uint8_t textTempSize = 64;
	char textDisp_temp[64] = {0};

	usrAppDevCurrentSystemTime_paramGet(&devParamTemp_infoData);
	esp_wifi_get_mac(ESP_IF_WIFI_STA, sta_mac);
	esp_mesh_get_parent_bssid(&parent_bssid);
	esp_wifi_vnd_mesh_get(&mesh_assoc);
	devMutualCtrlGroupInfo_groupInsertGet(mutualGroupInsert_temp);
	esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "System time: \n  #0080FF %04d/%02d/%02d -%s %02d:%02d:%02d#\n", devParamTemp_infoData.time_Year,
																	     			 		devParamTemp_infoData.time_Month,
																	     		 			devParamTemp_infoData.time_Day,
																	     		 			strInfoWeek_tab[devParamTemp_infoData.time_Week - 1],
																	     		 			devParamTemp_infoData.time_Hour,
																	     		 			devParamTemp_infoData.time_Minute,
																	     		 			devParamTemp_infoData.time_Second);
	lv_label_set_text(labelSysInfo_devTime, textDisp_temp);
	
	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Version: \n  #808080 Lanbon-L8 3244678975#");
	lv_label_set_text(labelSysInfo_devSoftVersion, textDisp_temp);
	
	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Free heap: \n  #00A2E8 %d# Bytes", esp_get_free_heap_size());
	lv_label_set_text(labelSysInfo_devReserveHeap, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Device mac: \n  #FF8027 "MACSTR"#", MAC2STR(sta_mac));
	lv_label_set_text(labelSysInfo_devMac, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Parent bssid: \n  #FF8027 "MACSTR"#", MAC2STR(parent_bssid.addr));
	lv_label_set_text(labelSysInfo_devParentMac, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Parent RSSI: \n  #8000FF %d# dB", mesh_assoc.rssi);
	lv_label_set_text(labelSysInfo_devParensRssi, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Mesh node num: \n  #00A2E8 %d#", esp_mesh_get_total_node_num());
	lv_label_set_text(labelSysInfo_devMeshNodeNum, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Mesh layer: \n  #B97A57 %d#", esp_mesh_get_layer());
	lv_label_set_text(labelSysInfo_devMeshLayer, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Device mutual group: \n  #00FF00 [%d], [%d], [%d]#", mutualGroupInsert_temp[0],
																		mutualGroupInsert_temp[1],
																		mutualGroupInsert_temp[2]);
	lv_label_set_text(labelSysInfo_devMutualGroupEnum, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Environment temperature: \n  #408080 %.02f# \"C", devDriverBussiness_temperatureMeasure_get());
	lv_label_set_text(labelSysInfo_devTemprature, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Device power: \n  #408080 %.01f# W", devDriverBussiness_elecMeasure_valElecPowerGet());
	lv_label_set_text(labelSysInfo_devPower, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Electricity consumption: \n  #408080 %.01f# kWh", devDriverBussiness_elecMeasure_valElecConsumGet());
	lv_label_set_text(labelSysInfo_devElecSum, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Router SSID: \n  #B5E61D %s#", wifi_config.sta.ssid);
	lv_label_set_text(labelSysInfo_routerSSID, textDisp_temp);

	memset(textDisp_temp, 0, sizeof(char) * textTempSize);
	sprintf(textDisp_temp, "Router password: \n  #B5E61D %s#", wifi_config.sta.password);
	lv_label_set_text(labelSysInfo_routerPSD, textDisp_temp);
}

void lvGui_businessMenu_other(lv_obj_t * obj_Parent){

	const uint8_t textHorizontalDistance = 30;
	const uint8_t textVerticalDistance = 55;

	lvGuiOther_styleApplicationInit();

	objText_menuCurrentTitle = lv_label_create(obj_Parent, NULL);
	lv_label_set_text(objText_menuCurrentTitle, "system info");
	lv_obj_set_pos(objText_menuCurrentTitle, 40, 45);
	lv_obj_set_style(objText_menuCurrentTitle, &styleText_menuLevel_A);

	menuBtnChoIcon_fun_back = lv_imgbtn_create(obj_Parent, NULL);
	lv_imgbtn_set_src(menuBtnChoIcon_fun_back, LV_BTN_STATE_REL, &iconMenu_funBack_arrowLeft);
	lv_imgbtn_set_src(menuBtnChoIcon_fun_back, LV_BTN_STATE_PR, &iconMenu_funBack_arrowLeft);
	lv_obj_set_pos(menuBtnChoIcon_fun_back, 8, 45);
	lv_btn_set_action(menuBtnChoIcon_fun_back, LV_BTN_ACTION_CLICK, funCb_btnActionClick_menuBtn_funBack);
	lv_btn_set_action(menuBtnChoIcon_fun_back, LV_BTN_ACTION_PR, funCb_btnActionPress_menuBtn_funBack);

	page_sysInfo = lv_page_create(lv_scr_act(), NULL);
	lv_obj_set_size(page_sysInfo, 280, 320);
	lv_obj_set_pos(page_sysInfo, 0, 75);
	lv_page_set_style(page_sysInfo, LV_PAGE_STYLE_SB, &stylePage_sysInfo);
	lv_page_set_style(page_sysInfo, LV_PAGE_STYLE_BG, &stylePage_sysInfo);
	lv_page_set_sb_mode(page_sysInfo, LV_SB_MODE_HIDE);
	lv_page_set_scrl_fit(page_sysInfo, false, false); //key opration
	lv_page_set_scrl_width(page_sysInfo, 320); //scrl尺寸必须大于set size尺寸 才可以进行拖拽
	lv_page_set_scrl_height(page_sysInfo, 960); //scrl尺寸必须大于set size尺寸 才可以进行拖拽
	lv_page_set_scrl_layout(page_sysInfo, LV_LAYOUT_CENTER);

	labelSysInfo_devTime = lv_label_create(page_sysInfo, NULL);
	lv_label_set_style(labelSysInfo_devTime, &labelStyle_sysInfo);
	lv_label_set_align(labelSysInfo_devTime, LV_LABEL_ALIGN_LEFT);
	lv_obj_set_size(labelSysInfo_devTime, 240, 40);
	lv_label_set_recolor(labelSysInfo_devTime, true);
	lv_obj_set_protect(labelSysInfo_devTime, LV_PROTECT_POS);
	lv_obj_align(labelSysInfo_devTime, 			page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 1);
	labelSysInfo_devSoftVersion = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devSoftVersion, 	page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 2);
	labelSysInfo_devReserveHeap = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devReserveHeap, 	page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 3);
	labelSysInfo_devMac = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devMac, 			page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 4);
	labelSysInfo_devParentMac = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devParentMac,		page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 5);
	labelSysInfo_devParensRssi = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devParensRssi, 	page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 6);
	labelSysInfo_devMeshNodeNum = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devMeshNodeNum, 	page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 7);
	labelSysInfo_devMeshLayer = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devMeshLayer, 	page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 8);
	labelSysInfo_devMutualGroupEnum = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devMutualGroupEnum, page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 9);
	labelSysInfo_devTemprature = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devTemprature, 	page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 10);
	labelSysInfo_devPower = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devPower, 		page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 11);
	labelSysInfo_devElecSum = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_devElecSum, 		page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 12);
	labelSysInfo_routerSSID = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_routerSSID, 		page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 13);
	labelSysInfo_routerPSD = lv_label_create(page_sysInfo, labelSysInfo_devTime);
	lv_obj_align(labelSysInfo_routerPSD, 		page_sysInfo, LV_ALIGN_IN_TOP_LEFT, textHorizontalDistance, textVerticalDistance * 14);

	lvGuiOther_devInfoRefresh();
}
