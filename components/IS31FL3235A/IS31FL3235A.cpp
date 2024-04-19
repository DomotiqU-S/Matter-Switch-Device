#include "IS31FL3235A.hpp"

IS31FL3235A::IS31FL3235A(gpio_num_t sda_pin, gpio_num_t scl_pin, uint8_t address) {
    m_bus = I2CController::getInstance();
    m_bus->setSCLPin(scl_pin);
    m_bus->setSDAPin(sda_pin);
    m_address = address;
}

IS31FL3235A::~IS31FL3235A() {
}

esp_err_t IS31FL3235A::begin() {
    return m_bus->begin();
}

esp_err_t IS31FL3235A::reset() {
    uint8_t data = 0x00;
    return m_bus->write(m_address, &data, IS31FL3235A_REG_RESET, 1);
}

esp_err_t IS31FL3235A::sendConfig() {
    // Write 0x01 to register 0x00
    uint8_t data = 0x01;
    esp_err_t err = m_bus->write(m_address, &data, IS31FL3235A_REG_CONFIG, 1);

    // Write 0x01 to register 0x4A
    data = 0x00;
    err |= m_bus->write(m_address, &data, IS31FL3235A_REG_GLOBAL_CTRL, 1);

    data = 0x01;
    err |= m_bus->write(m_address, &data, IS31FL3235A_REG_FREQUENCY, 1);

    return err;
}

esp_err_t IS31FL3235A::setPWM(uint8_t channel, uint8_t pwm) {
    float x = (IMAX * ((float)pwm/100.0) * RESOLUTION) / IMAX;
    uint8_t data = (uint8_t)(x-1);
    esp_err_t err = m_bus->write(m_address, &data, IS31FL3235A_PWM_CHANNEL_0 + channel, 1);
    return err;
}

esp_err_t IS31FL3235A::setLEDControl(uint8_t channel, bool state) {
    uint8_t data = state;
    return m_bus->write(m_address, &data, IS31FL3235A_LED_CONTROL_CHANNEL_0 + channel, 1);
}

esp_err_t IS31FL3235A::updatePWM() {
    uint8_t data = 0;
    return m_bus->write(m_address, &data, IS31FL3235A_REG_PWM_UPDATE, 1);
}