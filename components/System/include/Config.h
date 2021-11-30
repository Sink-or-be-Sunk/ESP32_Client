#pragma once

// NOTE: THESE CONFIGS ARE CONSOLIDATED FROM VARIOUS
// Kconfig.projbuild files

/**
 * Toggles between Production and Development Pin Configuration
 * @type: bool
 * Comment this to set pins to production
 */
#define DEVELOPEMENT_PINS

/**
 * Reset Non-Volatile Storage to Factory Defaults
 * @type: bool
 * This erases the NVS that corresponds with saved user
 * preferences
 */
// #define STORAGE_RESET_ALL

/**
 * Reset provisioned credentials and state machine after
 * session failure
 * @type: bool
 * Enable reseting provisioned credentials and state machine
 * after session failure.  This will restart the provisioning
 * service after retries are exhausted.
 */
#define WIFI_RESET_PROV_MGR_ON_FAILURE

/**
 * Max retries before reseting provisioning state machine
 * @type: int
 * Set the Maximum retry to avoid reconnecting to an inexistent
 * AP or if credentials are misconfigured. Provisioned
 * credentials are erased and internal state machine is reset
 * after this threshold is reached.
 */
#define WIFI_PROV_MGR_MAX_RETRY_CNT 5

/**
 * Show provisioning QR code
 * @type: bool
 * Show the QR code for provisioning in serial terminal
 */
// #define WIFI_PROV_SHOW_QR

/**
 * Reset provisioned status of the device
 * @type: bool
 * This erases the NVS to reset provisioned status of the
 * device on every reboot.  Provisioned status is determined by
 * the Wi-Fi STA configuration, saved on the NVS.
 */
// #define WIFI_RESET_PROVISIONED

/**
 * Max retries at connecting to network before resetting wifi
 * provising
 * @type: int
 * Set the Maximum retry to avoid attempting to connect to a
 * network that isn't available.  Provisioned credentials are
 * erased and internal state machine is reset after this
 * threshold is reached.
 */
#define WIFI_NO_CONNECT_RETRY_CNT 5

/**
 * Websocket Server URI
 * @type: string
 * changing this uri will change the endpoint that the device
 * attempts to connect for gameplay, registration, etc...
 */
// #define WEBSOCKET_URI "ws://sink-or-be-sunk.herokuapp.com"
#define WEBSOCKET_URI "ws://prep-exceed-conservation-scenarios.trycloudflare.com/"
