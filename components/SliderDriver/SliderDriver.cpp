#include "SliderDriver.hpp"

SliderDriver::~SliderDriver()
{
    // Deinitialize the slider
}

HMI_driver_handle_t SliderDriver::init()
{
    // Initialize the slider
    return nullptr;
}

esp_err_t SliderDriver::getFlag()
{
    // Get the flag
    return m_flag;
}

uint8_t SliderDriver::getLevel()
{
    // Get the level
    return 0; //capacitance_touch.getLevel();
}