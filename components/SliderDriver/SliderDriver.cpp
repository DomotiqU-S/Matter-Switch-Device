#include "SliderDriver.hpp"

SliderDriver::SliderDriver()
{
    // Initialize the slider
}

SliderDriver::~SliderDriver()
{
    // Deinitialize the slider
}

HMI_driver_handle_t SliderDriver::slider_init()
{
    // Initialize the slider
    return nullptr;
}

void SliderDriver::slider_set_level(uint8_t level)
{
    m_level = level;
}

uint8_t SliderDriver::slider_get_level()
{
    return m_level;
}

void SliderDriver::slider_set_on_off(bool isOn)
{
    m_isOn = isOn;
}

bool SliderDriver::slider_get_on_off()
{
    return m_isOn;
}