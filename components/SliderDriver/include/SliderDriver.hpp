#ifndef SLIDER_DRIVER_HPP
#define SLIDER_DRIVER_HPP

#include "CAP1298.hpp"
#include "IS31FL3235A.hpp"
#include "LightDriver.hpp"
#include "driver/gpio.h"
#include <esp_err.h>
#include <stdbool.h>
#include <stdint.h>

typedef void *HMI_driver_handle_t;

class SliderDriver : public LightDriver
{
private:
    bool m_isOn;
    uint8_t m_level;
    uint8_t m_previous_level = 0;
    CAP1298 capacitance_touch;
    IS31FL3235A led_level_driver;

    esp_err_t m_flag;

public:
    SliderDriver() : capacitance_touch(GPIO_NUM_10, GPIO_NUM_11), led_level_driver(GPIO_NUM_10, GPIO_NUM_11, IS31FL3235A_ADDR) {
        m_flag = capacitance_touch.begin();
        m_flag = led_level_driver.begin();
    }
    ~SliderDriver();
    bool start();
    HMI_driver_handle_t init();
    esp_err_t getFlag();
    uint8_t getLevel(uint8_t max_level);
    bool newTouches();
    uint8_t getNewTouches();
    void updateTouchStatus();

    // LightDriver Methods
    uint16_t get_temperature() override;
    uint8_t get_intensity() override;
    uint32_t get_duty(uint8_t channel) override;
    uint32_t get_hue() override;
    uint8_t get_saturation() override;
    uint16_t get_x() override;
    uint16_t get_y() override;

    esp_err_t set_power(bool power) override;
    esp_err_t set_brightness(uint8_t brightness) override;
    esp_err_t set_color(uint16_t x, uint16_t y) override;
    esp_err_t set_hue(uint16_t hue) override;
    esp_err_t set_saturation(uint8_t saturation) override;
    esp_err_t set_temperature(uint32_t temperature) override;

    void led_routine() override;
};

#endif // SLIDER_DRIVER_HPP