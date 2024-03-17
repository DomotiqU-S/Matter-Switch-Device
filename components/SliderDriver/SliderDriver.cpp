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

bool SliderDriver::newTouches()
{
    // Check if there are new touches
    return capacitance_touch.touchStatusChanged();
}

uint8_t SliderDriver::getNewTouches()
{
    // Get the new touches
    return capacitance_touch.getNewTouches();
}

void SliderDriver::updateTouchStatus()
{
    // Update the touch status
    capacitance_touch.updateTouchStatus();
}

bool SliderDriver::start()
{
    esp_err_t ret = capacitance_touch.init();
    return ret == ESP_OK;
}