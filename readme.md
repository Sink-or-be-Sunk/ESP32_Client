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

## WebSocketMsg

to help with abstracting communincation with the server, the WebSocketMsg component is used. This will allow for sending json objects to the server.

### Use Case

```c
char *msg = websocket_msg_fn_call(); //ex: create_new_game_req();
wifi_send(msg);

//or can combine to one line
wifi_send(create_new_game_req());
```

`NOTE`: This API is a wip and needs to add functions for positioning ships, user ready up, etc...

## TODO

- [x] WiFi provisioning
- [x] Websockets
- [ ] Write to non-volatile storage (nvs)
  - This will be used to store the user's username so that the device has access to user account (friends, ect...)
- [ ] GPIO digital control
  - [ ] Input Interrupts
  - [ ] Output Digital (mux sel)
- [ ] ADC monitoring (ship monitoring)
- [ ] DAC (speaker)
- [ ] SPI (LCD)

## Future Integration Ideas

- Web Wifi Provisioning integration with [Improv](https://www.improv-wifi.com/)
