idf_component_register(SRCS "IODriver.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES SliderDriver device esp_matter PWMDriver)
