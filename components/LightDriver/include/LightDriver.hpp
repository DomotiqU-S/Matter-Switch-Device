#ifndef LIGHT_DRIVER_HPP
#define LIGHT_DRIVER_HPP

#include <esp_err.h>
#include <stdbool.h>
#include <stdint.h>

typedef void *led_driver_handle_t;

typedef enum {
    HAL_ESP = 0,
    HAL_CUSTOM,
    WS_DRIVER,
} driver_t;
typedef struct {
    uint8_t gpio_warm;
    uint8_t gpio_cold;

    uint8_t gpio_red_light;
    uint8_t gpio_green_light;
    uint8_t gpio_blue_light;

    uint8_t gpio_light_data;
} light_gpio_t;

// Abstract class for the light driver
// The specific hardware driver should inherit from this class
class LightDriver
{
public:
    virtual ~LightDriver() = default;
    virtual led_driver_handle_t init() = 0;

    // Getters

    /**
     * @brief Get the Temperature object
     * 
     * @return uint16_t 
     */
    virtual uint16_t get_temperature() = 0;

    /**
     * @brief Get the Intensity object
     * 
     * @return uint8_t 
     */
    virtual uint8_t get_intensity() = 0;

    /**
     * @brief Get the Duty object
     * 
     * @param channel 
     * @return uint32_t 
     */
    virtual uint32_t get_duty(uint8_t channel) = 0;

    /**
     * @brief Get the Hue object
     * 
     * @return uint16_t 
     */
    virtual uint16_t get_hue() = 0;

    /**
     * @brief Get the Saturation object
     * 
     * @return uint8_t 
     */
    virtual uint8_t get_saturation() = 0;

    /**
     * @brief Get the X color value
     * 
     * @return uint16_t 
     */
    virtual uint16_t get_x() = 0;

    /**
     * @brief Get the Y color value
     * 
     * @return uint16_t 
     */
    virtual uint16_t get_y() = 0;

    // Setters

    /**
     * @brief Set the power value representing the state of the light
     * 
     * @param power 
     * @return esp_err_t 
     */
    virtual esp_err_t set_power(bool power) = 0;

    /**
     * @brief Set the brightness value from 0 to 255 - 1
     * 
     * @param brightness 
     * @return esp_err_t 
     */
    virtual esp_err_t set_brightness(uint8_t brightness) = 0;

    /**
     * @brief Set the xy color value
     * 
     * @param x 
     * @param y 
     * @return esp_err_t 
     */
    virtual esp_err_t set_color(uint16_t x, uint16_t y) = 0;

    /**
     * @brief Set the hue value
     * 
     * @param hue 
     * @return esp_err_t 
     */
    virtual esp_err_t set_hue(uint16_t hue) = 0;

    /**
     * @brief Set the saturation value
     * 
     * @param saturation 
     * @return esp_err_t 
     */
    virtual esp_err_t set_saturation(uint8_t saturation) = 0;

    /**
     * @brief Set the temperature value
     * 
     * @param temperature 
     * @return esp_err_t 
     */
    virtual esp_err_t set_temperature(uint32_t temperature) = 0;

    /**
     * @brief The routine is called periodically to update the LED PWM
     * 
     */
    virtual void led_routine() = 0;
};

#endif // LIGHT_DRIVER_HPP
