idf_component_register(SRCS "Websocket.c"
                    INCLUDE_DIRS "include"
                    REQUIRES cJSON WebSocketMsg esp_websocket_client wifi_provisioning)
