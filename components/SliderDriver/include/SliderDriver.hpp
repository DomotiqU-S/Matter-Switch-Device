#ifndef SLIDER_DRIVER_HPP
#define SLIDER_DRIVER_HPP

#include <esp_err.h>
#include <stdbool.h>
#include <stdint.h>

typedef void *HMI_driver_handle_t;

class SliderDriver
{
private:
    bool m_isOn;
    uint8_t m_level;
    // CAP1298 i2c driver
public:
    SliderDriver();
    ~SliderDriver();
    HMI_driver_handle_t slider_init();
    void slider_set_level(uint8_t level);
    uint8_t slider_get_level();
    void slider_set_on_off(bool isOn);
    bool slider_get_on_off();
};

#endif // SLIDER_DRIVER_HPP