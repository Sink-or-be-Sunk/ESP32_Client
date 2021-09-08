idf_component_register(SRCS "Websocket.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES cJSON WebSocketMsg esp_websocket_client wifi_provisioning)
