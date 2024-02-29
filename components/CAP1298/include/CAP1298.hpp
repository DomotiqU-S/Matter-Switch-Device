#ifndef CAP1298_H
#define CAP1298_H

#include "I2CController.hpp"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <esp_err.h>


// CAP1298 I2C address
#define CAP1298_I2C_ADDRESS (uint8_t)(0x28)

// CAP1298 registers (see datasheet)
#define CAP1298_MAIN_CONTROL 0x00
#define CAP1298_GENERAL_STATUS 0x02
#define CAP1298_INPUT_STATUS 0x03
#define CAP1298_NOISE_FLAG_STATUS 0x0A
#define CAP1298_SENSOR_INPUT1_DELTA_COUNT_BASE 0x10
#define CAP1298_SENSOR_INPUT2_DELTA_COUNT_BASE 0x11
#define CAP1298_SENSOR_INPUT3_DELTA_COUNT_BASE 0x12
#define CAP1298_SENSOR_INPUT4_DELTA_COUNT_BASE 0x13
#define CAP1298_SENSOR_INPUT5_DELTA_COUNT_BASE 0x14
#define CAP1298_SENSOR_INPUT6_DELTA_COUNT_BASE 0x15
#define CAP1298_SENSOR_INPUT7_DELTA_COUNT_BASE 0x16
#define CAP1298_SENSOR_INPUT8_DELTA_COUNT_BASE 0x17
#define CAP1298_SENSITIVITY_CONTROL 0x1F
#define CAP1298_CONFIG1 0x20
#define CAP1298_INPUT_ENABLE 0x21
#define CAP1298_INPUT_CONFIG 0x22
#define CAP1298_INPUT_CONFIG2 0x23
#define CAP1298_AVERAGING_AND_SAMPLING_CONFIG 0x24
#define CAP1298_CALIBRATION_ACTIVATE 0x26
#define CAP1298_INTERRUPT_ENABLE 0x27
#define CAP1298_REPEAT_RATE_ENABLE 0x28
#define CAP1298_SIGNAL_GUARD 0x29
#define CAP1298_MULTIPLE_TOUCH_CONFIG 0x2A
#define CAP1298_MULTIPLE_TOUCH_PATTERN_CONFIG 0x2B
#define CAP1298_MULTIPLE_TOUCH_PATTERN 0x2D
#define CAP1298_BASE_COUNT_OUT 0x2C
#define CAP1298_RECALIBRATION_CONFIG 0x2F
#define CAP1298_INT_MASK 0x01


#define CAP1298_PRODUCT_ID 0xFD
#define CAP1298_MANUFACTURER_ID 0xFE
#define CAP1298_REVISION 0xFF
#define CAP1298_MANUFACTURER_ID_VALUE 0x5D
#define CAP1298_PRODUCT_ID_VALUE 0x71
#define CAP1298_REVISION_VALUE 0x00

#define LONG_TOUCH_TIME 2000 // Time in ms

class CAP1298
{
private:
    I2CController busController;
    uint8_t m_touchData = 0;
    uint8_t m_newTouches = 0;
    uint8_t m_newReleases = 0;

    int16_t m_invalid_constructor = 0;
public:
    /**
     * @brief Construct a new CAP1298 object
     * 
     * @param sda the SDA pin
     * @param scl the SCL pin
     * @param freq the I2C frequency (default 100000)
     * @param address the I2C address (default 0x28)
     */
    CAP1298(gpio_num_t sda, gpio_num_t scl, uint32_t freq = 100000, uint8_t address = CAP1298_I2C_ADDRESS) : busController(address, sda, scl, freq) {
        if(sda == scl) {
            m_invalid_constructor = ESP_ERR_INVALID_ARG;
            return;
        }
    }
    ~CAP1298();

    /**
     * @brief Initialize the CAP1298
     * If the initialization fails, the flag will be set to a non-zero value
     * @return esp_err_t 
     */
    esp_err_t begin();

    /**
     * @brief Check if the touch status has changed
     * Read the CAP1298 interrupt status and reset it
     * @return true 
     * @return false 
     */
    bool touchStatusChanged();

    /**
     * @brief Update the touch status
     * Change the newTouches and newReleases variables
     */
    void updateTouchStatus();

    /**
     * @brief Get the New Touches object
     * Will return the new touches in uint8_t format corresponding to the bits of the touchData
     * @return uint8_t 
     */
    uint8_t getNewTouches() { return m_newTouches; }

    /**
     * @brief Get the New Releases object
     * Will return the new releases in uint8_t format corresponding to the bits of the touchData
     * @return uint8_t 
     */
    uint8_t getNewReleases() { return m_newReleases; }

    /**
     * @brief Get the Flag object
     * It is used to check if the constructor was successful
     * @return int16_t 
     */
    int16_t getFlag() { return m_invalid_constructor; }

    // TODO: Implement the following methods
    /**
     * @brief The method return true is the touch data are moving in the given direction
     * 
     * @param value_to_mapped The value to be mapped
     * @param callback_func The callback function
     */
    void isMoving(uint16_t value_to_mapped);

    // TODO: Implement the following methods
    /**
     * @brief The method check if the surface is touched multiple times.
     * 
     * @param time_touched The time to check if the surface is touched multiple times (max 3)
     * @param callback_func The fucntion to call when the multiple touch is achived.
     */
    void isRepeatedTouch(uint8_t time_touched, void* callbakc_func);

    // TODO: Implement the following methods
    /**
     * @brief The method check if the surface is touched for a long time.
     * If the surface is touched for a greater time than the given time, the method return true.
     * @return true 
     * @return false 
     */
    void isLongTouch(void* callback_func);

    void loop(uint16_t interval = 100);
};

#endif // CAP1298_H