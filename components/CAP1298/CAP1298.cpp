#include "CAP1298.hpp"

CAP1298::CAP1298(gpio_num_t sda_pin, gpio_num_t scl_pin, uint8_t address)
{
    this->m_bus = I2CController::getInstance();

    this->m_bus->setSCLPin(scl_pin);
    this->m_bus->setSDAPin(sda_pin);
    this->m_address = address;
}

CAP1298::~CAP1298()
{
}

esp_err_t CAP1298::begin()
{
    esp_err_t ret;
    ret = this->m_bus->begin();
    return ret;
}

bool CAP1298::touchStatusChanged()
{
    uint8_t rx_buffer[1];
    uint8_t tx_buffer[1];
    this->m_bus->readByte(m_address, rx_buffer, CAP1298_MAIN_CONTROL);
    bool touchStatus = rx_buffer[0] & CAP1298_INT_MASK;
    
    // reset the interrupt
    tx_buffer[0] = 0x00;
    this->m_bus->writeByte(m_address, tx_buffer, CAP1298_MAIN_CONTROL);
    return touchStatus;
}

void CAP1298::updateTouchStatus()
{
    uint8_t rx_buffer[1];

    this->m_bus->readByte(m_address, rx_buffer, CAP1298_INPUT_STATUS);

    m_newTouches = (m_touchData ^ rx_buffer[0]) & rx_buffer[0];
	m_newReleases = (m_touchData ^ rx_buffer[0]) & m_touchData;
	m_touchData = rx_buffer[0];
}

esp_err_t CAP1298::init()
{
    uint8_t tx_buffer[1];
    esp_err_t ret = 0;

    tx_buffer[0] = 0x8C;
    ret |= this->m_bus->writeByte(m_address, tx_buffer, CAP1298_MULTIPLE_TOUCH_CONFIG);
    tx_buffer[0] = 0x5F;
    ret |= this->m_bus->writeByte(m_address, tx_buffer, CAP1298_SENSITIVITY_CONTROL);
    tx_buffer[0] = 0x00;
    ret |= this->m_bus->writeByte(m_address, tx_buffer, CAP1298_REPEAT_RATE_ENABLE);

    return ret;
}