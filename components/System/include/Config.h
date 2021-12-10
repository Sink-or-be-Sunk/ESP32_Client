#pragma once

// NOTE: THESE CONFIGS ARE CONSOLIDATED FROM VARIOUS
// Kconfig.projbuild files

/**
 * Toggles between Production and Development Pin Configuration
 * @type: bool
 * Comment this to set pins to production
 */
#define PRODUCTION_PINS

/**
 * Websocket Server URI
 * @type: string
 * changing this uri will change the endpoint that the device
 * attempts to connect for gameplay, registration, etc...
 */
#define PRODUCTION_SERVER
#ifdef PRODUCTION_SERVER
#define WEBSOCKET_URI "ws://sink-or-be-sunk.herokuapp.com"
#else
#define WEBSOCKET_URI "ws://beijing-comprehensive-downloadable-origin.trycloudflare.com"
#endif

/**
 * Hardcode ship positions for development purposes
 * @type: bool
 * Sets the ship positions to hardcoded values to allow for
 * testing the system without physical hardware for ship
 * detection
 */
// #define HARDCODE_SHIP_POSITIONS

/**
 * Check individual row/col pins are functioning properly
 * @type: bool
 * Use the arrow keys to cycle through the rows and column
 * selects from the mux array to check the conditions of each
 * pin
 */
// #define DEBUG_SHIP_POSITIONS

#if defined(HARDCODE_SHIP_POSITIONS) && defined(DEBUG_SHIP_POSITIONS)
#error YOU CANNOT HAVE THESE TWO CONDITIONS DEFINED AT THE SAME TIE
#endif

/**
 * Add LED rainbow demo in project for testing the led strips
 * @type: bool
 * Pressing button 1 turns on the led light show
 * Pressing button 2 turns the lights off
 */
#define LED_RAINBOW_DEMO

/**
 * Add Motor Rumble Demo to Project
 * @type: bool
 * pressing button 3 will cause the motor to rumble
 */
// #define MOTOR_RUMBLE_DEMO

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
 * Number of websocket packets that can be dropped before causing a
 * system restart
 * @type: int
 * Changes the number of packets that are dropped when trying to send
 * the connection handshake packet.  When >= THIS packets are dropped
 * the system will restart to attempt to reestablish a connection
 */
#define WS_PACKET_DROPPED_LIMIT 10

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
#define WIFI_NO_CONNECT_RETRY_CNT 12
