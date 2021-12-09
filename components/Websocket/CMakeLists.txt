idf_component_register(SRCS
                    INCLUDE_DIRS "include"
                    REQUIRES cJSON Messenger esp_websocket_client wifi_provisioning GameState ScreenManager FriendManager LEDManager MotorManager freertos System)
