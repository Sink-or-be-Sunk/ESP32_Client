idf_component_register(SRCS "WiFi.c"
                    INCLUDE_DIRS "include"
                    REQUIRES cJSON WebSocketMsg esp_websocket_client wifi_provisioning)
