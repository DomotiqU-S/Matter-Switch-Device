idf_component_register(SRCS "SliderDriver.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES CAP1298 IS31FL3235A driver LightDriver device)
