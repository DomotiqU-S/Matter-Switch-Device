#ifndef IS31FL3235A_H
#define IS31FL3235A_H

#include "I2CController.hpp"
#include "driver/gpio.h"
#include <stdio.h>
#include <esp_log.h>
#include <esp_err.h>

#define IMAX 0.038
#define RESOLUTION 256.0

#define IS31FL3235A_ADDR 0x3C
#define IS31FL3235A_REG_CONFIG 0x00
#define IS31FL3235A_REG_PWM_UPDATE 0x25
#define IS31FL3235A_REG_GLOBAL_CTRL 0x4A
#define IS31FL3235A_REG_FREQUENCY 0x4B
#define IS31FL3235A_REG_RESET 0x4F

#define IS31FL3235A_PWM_CHANNEL_0 0x05
#define IS31FL3235A_PWM_CHANNEL_1 0x06
#define IS31FL3235A_PWM_CHANNEL_2 0x07
#define IS31FL3235A_PWM_CHANNEL_3 0x08
#define IS31FL3235A_PWM_CHANNEL_4 0x09
#define IS31FL3235A_PWM_CHANNEL_5 0x0A
#define IS31FL3235A_PWM_CHANNEL_6 0x0B
#define IS31FL3235A_PWM_CHANNEL_7 0x0C
#define IS31FL3235A_PWM_CHANNEL_8 0x0D
#define IS31FL3235A_PWM_CHANNEL_9 0x0E
#define IS31FL3235A_PWM_CHANNEL_10 0x0F
#define IS31FL3235A_PWM_CHANNEL_11 0x10
#define IS31FL3235A_PWM_CHANNEL_12 0x11
#define IS31FL3235A_PWM_CHANNEL_13 0x12
#define IS31FL3235A_PWM_CHANNEL_14 0x13
#define IS31FL3235A_PWM_CHANNEL_15 0x14
#define IS31FL3235A_PWM_CHANNEL_16 0x15
#define IS31FL3235A_PWM_CHANNEL_17 0x16
#define IS31FL3235A_PWM_CHANNEL_18 0x17
#define IS31FL3235A_PWM_CHANNEL_19 0x18
#define IS31FL3235A_PWM_CHANNEL_20 0x19
#define IS31FL3235A_PWM_CHANNEL_21 0x1A
#define IS31FL3235A_PWM_CHANNEL_22 0x1B
#define IS31FL3235A_PWM_CHANNEL_23 0x1C
#define IS31FL3235A_PWM_CHANNEL_24 0x1D
#define IS31FL3235A_PWM_CHANNEL_25 0x1E
#define IS31FL3235A_PWM_CHANNEL_26 0x1F
#define IS31FL3235A_PWM_CHANNEL_27 0x20

#define IS31FL3235A_LED_CONTROL_CHANNEL_0 0x2A
#define IS31FL3235A_LED_CONTROL_CHANNEL_1 0x2B
#define IS31FL3235A_LED_CONTROL_CHANNEL_2 0x2C
#define IS31FL3235A_LED_CONTROL_CHANNEL_3 0x2D
#define IS31FL3235A_LED_CONTROL_CHANNEL_4 0x2E
#define IS31FL3235A_LED_CONTROL_CHANNEL_5 0x2F
#define IS31FL3235A_LED_CONTROL_CHANNEL_6 0x30
#define IS31FL3235A_LED_CONTROL_CHANNEL_7 0x31
#define IS31FL3235A_LED_CONTROL_CHANNEL_8 0x32
#define IS31FL3235A_LED_CONTROL_CHANNEL_9 0x33
#define IS31FL3235A_LED_CONTROL_CHANNEL_10 0x34
#define IS31FL3235A_LED_CONTROL_CHANNEL_11 0x35
#define IS31FL3235A_LEDIS31FL3235ACONTROL_CHANNEL_22 0x40
#define IS31FL3235A_LED_CONTROL_CHANNEL_23 0x41
#define IS31FL3235A_LED_CONTROL_CHANNEL_24 0x42
#define IS31FL3235A_LED_CONTROL_CHANNEL_25 0x43
#define IS31FL3235A_LED_CONTROL_CHANNEL_26 0x44
#define IS31FL3235A_LED_CONTROL_CHANNEL_27 0x45

class IS31FL3235A
{
private:
    I2CController *m_bus;

    uint8_t m_address = 0;
public:
    /**
     * @brief Construct a new IS31FL3235A object
     * 
     * @param sda_pin The SDA pin
     * @param scl_pin The SCL pin
     * @param address The device address
     */
    IS31FL3235A(gpio_num_t sda_pin, gpio_num_t scl_pin, uint8_t address);

    ~IS31FL3235A();

    /**
     * @brief Start the I2C bus
     * 
     * @return esp_err_t 
     */
    esp_err_t begin();

    /**
     * @brief Reset the device
     * 
     * @return esp_err_t 
     */
    esp_err_t reset();

    /**
     * @brief Set the PWM value for a channel in a temporary buffer
     * 
     * @param channel the channel to set the PWM value for
     * @param pwm the PWM value to set between 0 and 100
     * @return esp_err_t 
     */
    esp_err_t setPWM(uint8_t channel, uint8_t pwm);

    /**
     * @brief Set the state of a channel in a temporary buffer
     * 
     * @param channel the channel to set the state for
     * @param state the state to: true for on, false for off
     * @return esp_err_t 
     */
    esp_err_t setLEDControl(uint8_t channel, bool state);
    
    /**
     * @brief Write the temporary buffer to the device
     * This should be call after all the PWM values and LED control states have been set
     * @return esp_err_t 
     */
    esp_err_t updatePWM();

    /**
     * @brief Send the configuration to the device
     * 
     * @return esp_err_t 
     */
    esp_err_t sendConfig();
};

#endif