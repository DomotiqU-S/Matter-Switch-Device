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

uint8_t SliderDriver::getLevelRaw() {
    if(this->m_level == 0)
        return this->m_previous_level;
    return this->m_level;
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

    for (int i = 7; i > -1; --i) {
        if ((flipped_byte >> i) & 1) {
            value = i;
            break;
        }
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
    esp_err_t ret = capacitance_touch.begin();
    ret |= led_level_driver.sendConfig();

    for(uint8_t i = 0; i < 8; i++) {
        led_level_driver.setPWM(i, 0);
        led_level_driver.setLEDControl(i, false);
        led_level_driver.updatePWM();
    }

    gpio_isr_handler_add((gpio_num_t)CONFIG_TRIAC_SYNC, gpio_isr_handler, (void *)this);
    
    return true;
}

esp_err_t SliderDriver::set_power(bool power)
{
    // Update the GPIO value
    m_isOn = power;

    if(m_isOn) {
        // getLevel(m_level);

        // this->set_brightness(m_level);
        this->alarm_config.alarm_count = this->interval;
        gptimer_set_alarm_action(timer_handle, &alarm_config);
    }
    else {
        this->set_brightness(0);
        // getLevel(0);

        // Update the interval
        this->alarm_config.alarm_count = this->interval;
        gptimer_set_alarm_action(timer_handle, &alarm_config);
    }

    return ESP_OK;
}

/**
 * @brief 
 * 
 * @param level 
 */
void SliderDriver::set_level_led(uint8_t level)
{
    uint8_t n_led = 8 * level / 100;
    bool led_status = n_led > 0;

    if(level == 0) {
        n_led = 8;
    }

    for(uint8_t i = 0; i < n_led; i++) {
        led_level_driver.setPWM(7 - i, 100);
        led_level_driver.setLEDControl(7 - i, led_status);
        led_level_driver.updatePWM();
    }

    for(uint8_t i = n_led; i < 8; i++) {
        led_level_driver.setPWM(7 - i, 0);
        led_level_driver.setLEDControl(7 - i, !led_status);
        led_level_driver.updatePWM();
    }
}

void SliderDriver::set_front_led(bool power)
{
    // Set the channel 9 change the color of the front led
    led_level_driver.setLEDControl(9, power);
    led_level_driver.setPWM(9, power ? 40 : 0);
    led_level_driver.setLEDControl(8, !power);
    led_level_driver.setPWM(8, !power ? 40 : 0);
    led_level_driver.updatePWM();
}

esp_err_t SliderDriver::set_brightness(uint8_t brightness)
{
    ESP_LOGI("SLIDER_DRIVER", "Brightness: %d", brightness);
    int32_t actual_level = INTENSITY2TIME(m_level);

    this->m_previous_level = m_level;
    // Set the brightness
    m_level = brightness;
    // start_fade = true;

    if(brightness == 1) {
        m_level = 0;
    }

    if(m_level > 1) {

        // Update the brightness
        #if FADE_ENABLE
            this->interval = INTENSITY2TIME(m_level);
            this->reminder = this->interval % (FADE_RESOLUTION - 1);
            this->interval /= (FADE_RESOLUTION - 1);
        #else
            this->interval = INTENSITY2TIME(m_level);
        #endif

        #if DEBUG_SLIDER
            ESP_LOGI("SLIDER_DRIVER", "Interval: %" PRId32, this->interval);
        #endif
    }
    else {
        #if FADE_ENABLE
            // Update the interval
            this->interval = -INTENSITY2TIME(actual_level);
            this->reminder = -(this->interval % (FADE_RESOLUTION - 1));
            this->interval /= (FADE_RESOLUTION - 1);
        #else
            this->interval = MAX_INTERVAL;
        #endif
    }

    return ESP_OK;
}