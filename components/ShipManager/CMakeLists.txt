idf_component_register(SRCS "ShipPosition.cpp" "ShipManager.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES freertos driver ScreenManager LEDManager System)