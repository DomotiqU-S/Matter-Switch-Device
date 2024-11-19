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

    // if(level == 0) {
    //     return m_previous_level;
    // }

    // uint8_t flipped_byte = 0;
    // for (int i = 0; i < 8; ++i) {
    //     flipped_byte |= ((level >> i) & 1) << (7 - i);
    // }

    // for (int i = 7; i >= 0; --i) {
    //     if ((flipped_byte >> i) & 1) {
    //         value = i;
    //         break;
    //     }
    // }

    // for (int i = 0; i < 8; i++) {
    //     led_level_driver.setPWM(i, 0);
    //     led_level_driver.setLEDControl(i, false);
    //     led_level_driver.updatePWM();
    // }

    // // Manage led level
    // uint8_t pwm = 60;
    // // remap the value to the led value 1 - 8
    // uint8_t led_value = ((float)8 / 7.0) * value;

    // for (int i = 7; i >= (7 - led_value); i--) {
    //     led_level_driver.setPWM(i, pwm);
    //     led_level_driver.setLEDControl(i, true);
    //     led_level_driver.updatePWM();
    // }

    m_previous_level = ((float)max_level / 7.0) * value;
    return ((float)max_level / 7.0) * value;
}

bool SliderDriver::newTouches()
{
    // Check if there are new touches
    //return capacitance_touch.touchStatusChanged();

    return false;
}

uint8_t SliderDriver::getNewTouches()
{
    // Get the new touches
    //return capacitance_touch.getNewTouches();

    return 0;
}

void SliderDriver::updateTouchStatus()
{
    // Update the touch status
    //capacitance_touch.updateTouchStatus();
}

bool SliderDriver::start()
{
    //esp_err_t ret = capacitance_touch.begin();
    //ret |= led_level_driver.sendConfig();
    gpio_isr_handler_add(GPIO_NUM_12, gpio_isr_handler, (void *)this);
    return ESP_OK;
}

esp_err_t SliderDriver::set_power(bool power)
{
    // Set the channel 9 change the color of the front led
    // led_level_driver.setLEDControl(9, power);
    // led_level_driver.setPWM(9, power ? 40 : 0);
    // led_level_driver.setLEDControl(8, !power);
    // led_level_driver.setPWM(8, !power ? 40 : 0);
    // led_level_driver.updatePWM();

    // Update the GPIO value
    m_isOn = power;

    // Set the power
    return ESP_OK;
}

esp_err_t SliderDriver::set_brightness(uint8_t brightness)
{
    // Set the brightness
    m_level = brightness;
    start_fade = true;

    if(brightness == 1) {
        m_level = 0;
    }

    if(m_level > 1) {
        // Update the brightness
        float level_norm = (float)m_level / 2.54;
        uint32_t interval = MAX_INTERVAL * (100 - level_norm) / 100;

        // Update the interval
        this->alarm_config.alarm_count = interval;
        gptimer_set_alarm_action(timer_handle, &alarm_config);

        this->set_power(true);
    }
    else {

        this->set_power(false);
    }

    return ESP_OK;
}