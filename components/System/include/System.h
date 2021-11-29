#pragma once

/*************************************************/
/***************** SYSTEM FUNCTIONS **************/
/*************************************************/
void get_device_ssid(char result[SSID_MAX_LEN]);

void get_device_id(char *service_name);

/*************************************************/
/************** CONFIGURATION OPTIONS ************/
/*************************************************/
// NOTE: THESE CONFIGS ARE CONSOLIDATED FROM VARIOUS
// Kconfig.projbuild files

// ifdef (comment to toggle values)
#define DEVELOPEMENT_PINS
#define STORAGE_RESET_ALL
#define WIFI_RESET_PROV_MGR_ON_FAILURE
// #define WIFI_PROV_SHOW_QR
// #define WIFI_RESET_PROVISIONED

// number
#define WIFI_PROV_MGR_MAX_RETRY_CNT 5
#define WIFI_NO_CONNECT_RETRY_CNT 5

// string
#define WEBSOCKET_URI "ws://sink-or-be-sunk.herokuapp.com"

/*************************************************/
/************* DEVICE PIN DEFINITIONS ************/
/*************************************************/
#ifdef DEVELOPEMENT_PINS
/*************** BUTTON MANAGER ******************/
#define ROW_0_PIN GPIO_NUM_33
#define ROW_1_PIN GPIO_NUM_32
#define ROW_2_PIN GPIO_NUM_39
#define ROW_3_PIN GPIO_NUM_34

#define COL_0_PIN GPIO_NUM_23
#define COL_1_PIN GPIO_NUM_22
#define COL_2_PIN GPIO_NUM_27
#define COL_3_PIN GPIO_NUM_26

/*************** DISPLAY MANAGER ******************/
#define PIN_NUM_MISO GPIO_NUM_12
#define PIN_NUM_MOSI GPIO_NUM_13
#define PIN_NUM_SCLK GPIO_NUM_14
#define PIN_NUM_CS GPIO_NUM_15

/*************** LED MANAGER ******************/
#define LED_PIN GPIO_NUM_2

/*************** MOTOR MANAGER ******************/
#define MOTOR_DRIVER_PIN GPIO_NUM_25

/*************** SHIP MANAGER ******************/
#define MUX_ROW_SEL_0 GPIO_NUM_18
#define MUX_ROW_SEL_1 GPIO_NUM_19
#define MUX_ROW_SEL_2 GPIO_NUM_21
#define MUX_COL_SEL_0 GPIO_NUM_17
#define MUX_COL_SEL_1 GPIO_NUM_4
#define MUX_COL_SEL_2 GPIO_NUM_21
#define BOAT_INPUT GPIO_NUM_5

#else
/*************** BUTTON MANAGER ******************/
#define ROW_0_PIN GPIO_NUM_33
#define ROW_1_PIN GPIO_NUM_32
#define ROW_2_PIN GPIO_NUM_35
#define ROW_3_PIN GPIO_NUM_34

#define COL_0_PIN GPIO_NUM_23
#define COL_1_PIN GPIO_NUM_22
#define COL_2_PIN GPIO_NUM_27
#define COL_3_PIN GPIO_NUM_26

/*************** DISPLAY MANAGER ******************/
#define PIN_NUM_MISO GPIO_NUM_12
#define PIN_NUM_MOSI GPIO_NUM_13
#define PIN_NUM_SCLK GPIO_NUM_14
#define PIN_NUM_CS GPIO_NUM_15

/*************** LED MANAGER ******************/
#define LED_PIN GPIO_NUM_2

/*************** MOTOR MANAGER ******************/
#define MOTOR_DRIVER_PIN GPIO_NUM_22

/*************** SHIP MANAGER ******************/
#define MUX_ROW_SEL_0 GPIO_NUM_18
#define MUX_ROW_SEL_1 GPIO_NUM_19
#define MUX_ROW_SEL_2 GPIO_NUM_21
#define MUX_COL_SEL_0 GPIO_NUM_17
#define MUX_COL_SEL_1 GPIO_NUM_4
#define MUX_COL_SEL_2 GPIO_NUM_16

#define BOAT_INPUT GPIO_NUM_5
#endif
