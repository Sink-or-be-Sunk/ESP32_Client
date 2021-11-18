idf_component_register(SRCS "ShipManager.cpp" "ShipPosition.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES freertos driver ScreenManager Config)