idf_component_register(SRCS "Websocket.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES cJSON Messenger esp_websocket_client wifi_provisioning GameState ScreenManager FriendManager)
