#include "CAP1298.hpp"

CAP1298::~CAP1298()
{
}

esp_err_t CAP1298::begin()
{
    esp_err_t ret;
    ret = this->busController.begin();
    return ret;
}

bool CAP1298::touchStatusChanged()
{
    uint8_t rx_buffer[1];
    uint8_t tx_buffer[1];
    this->busController.readByte(rx_buffer, CAP1298_MAIN_CONTROL);
    bool touchStatus = rx_buffer[0] & CAP1298_INT_MASK;
    
    // reset the interrupt
    tx_buffer[0] = 0x00;
    this->busController.writeByte(tx_buffer, CAP1298_MAIN_CONTROL);
    return touchStatus;
}

void CAP1298::updateTouchStatus()
{
    uint8_t rx_buffer[1];

    this->busController.readByte(rx_buffer, CAP1298_INPUT_STATUS);

    m_newTouches = (m_touchData ^ rx_buffer[0]) & rx_buffer[0];
	m_newReleases = (m_touchData ^ rx_buffer[0]) & m_touchData;
	m_touchData = rx_buffer[0];
}

void CAP1298::isMoving(uint16_t value_to_mapped)
{

}

esp_err_t CAP1298::init()
{
    uint8_t tx_buffer[1];
    esp_err_t ret = 0;

    tx_buffer[0] = 0x8C;
    ret |= this->busController.writeByte(tx_buffer, CAP1298_MULTIPLE_TOUCH_CONFIG);
    tx_buffer[0] = 0x5F;
    ret |= this->busController.writeByte(tx_buffer, CAP1298_SENSITIVITY_CONTROL);
    tx_buffer[0] = 0x00;
    ret |= this->busController.writeByte(tx_buffer, CAP1298_REPEAT_RATE_ENABLE);

    return ret;
}