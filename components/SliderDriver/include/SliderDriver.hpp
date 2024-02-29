#ifndef SLIDER_DRIVER_HPP
#define SLIDER_DRIVER_HPP

#include "CAP1298.hpp"
#include "driver/gpio.h"
#include <esp_err.h>
#include <stdbool.h>
#include <stdint.h>

typedef void *HMI_driver_handle_t;

class SliderDriver
{
private:
    bool m_isOn;
    uint8_t m_level;
    CAP1298 capacitance_touch;

    esp_err_t m_flag;

public:
    SliderDriver() : capacitance_touch(GPIO_NUM_16, GPIO_NUM_15) {
        m_flag = capacitance_touch.begin();
    }
    ~SliderDriver();
    HMI_driver_handle_t init();
    esp_err_t getFlag();
    uint8_t getLevel();
    bool newTouches();
    uint8_t getNewTouches();
    void updateTouchStatus();

    /**
     * @brief The function read the value of the slider
     * When new touches are detected, it checks if the slider is moving up or down in the given time
     * If the touch is too long on a single position, it will be considered as not moving
     * The function will triger the IsMoving function
     * 
     * @param interval The time in milliseconds to check if the slider is moving (default 100ms)
     * @param threshold The threshold to consider the slider as moving in interval time multiple (default 5 times)
     */
    void loop(uint8_t interval = 100, uint8_t threshold = 5);
};

#endif // SLIDER_DRIVER_HPP