# Sink or be Sunk ESP32 Client

The client side code for the ESP32 microcontroller responsible for driving the user input hardware and relaying those messages through a websocket to the game server.

## Kconfig.projbuild

These files allow for easy configuration of a project with various `#define` statements within the project

A user can change from the default behavior using ESP IDF menuconfig

In VSCode:

- go to the command pallet <kbd>CTRL</kbd> + <kbd>P</kbd>
- search: `>ESP-IDF: SDK Configuration Editor`
- the ESP-IDF extension will open a new tab that contains the project config
- Look for the custom configuration name given at the top of a Kconfig.projbuild such as `WiFi Configuration`
- Editing this in the config editor will change the behavior of the program
