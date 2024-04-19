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

uint8_t SliderDriver::getLevel(uint8_t max_level)
{
    uint8_t level = getNewTouches();
    uint8_t value = 0;

    if(level == 0) {
        return m_previous_level;
    }

    uint8_t flipped_byte = 0;
    for (int i = 0; i < 8; ++i) {
        flipped_byte |= ((level >> i) & 1) << (7 - i);
    }

    for (int i = 7; i >= 0; --i) {
        if ((flipped_byte >> i) & 1) {
            value = i;
            break;
        }
    }

    for (int i = 0; i < 8; i++) {
        led_level_driver.setPWM(i, 0);
        led_level_driver.setLEDControl(i, false);
        led_level_driver.updatePWM();
    }

    // Manage led level
    uint8_t pwm = 60;
    // remap the value to the led value 1 - 8
    uint8_t led_value = ((float)8 / 7.0) * value;

    for (int i = 7; i >= (7 - led_value); i--) {
        led_level_driver.setPWM(i, pwm);
        led_level_driver.setLEDControl(i, true);
        led_level_driver.updatePWM();
    }

    m_previous_level = ((float)max_level / 7.0) * value;
    return ((float)max_level / 7.0) * value;
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
    ret |= led_level_driver.sendConfig();
    return ret == ESP_OK;
}