#ifndef PWM_DRIVER_HPP
#define PWM_DRIVER_HPP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define TAG_PWM_DRIVER         "PWM_DRIVER"
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_MODE              LEDC_LOW_SPEED_MODE
#define TRIAC_PWM_CHANNEL      5
#define LEDC_DUTY_RESOLUTION   LEDC_TIMER_10_BIT
#define MAX_DUTY               (uint32_t)1024
#define MIN_DUTY               (uint32_t)0
#define LEDC_FREQUENCY         (uint32_t)720
#define LEDC_FADE_TIME         (ledc_fade_mode_t)500
// #define DEBUG_SENSOR

class PWMDriver
{
private: 
    bool m_fade;
    bool m_state;
    ledc_fade_mode_t m_fade_time;
    esp_err_t m_err;
    uint16_t m_duty;
    uint8_t m_intensity;
    SemaphoreHandle_t m_semaphore = NULL;

    static IRAM_ATTR bool cb_ledc_fade_end_event(const ledc_cb_param_t *param, void *user_arg)
    {
        BaseType_t taskAwoken = pdFALSE;

        if (param->event == LEDC_FADE_END_EVT)
        {
            SemaphoreHandle_t counting_sem = (SemaphoreHandle_t)user_arg;
            xSemaphoreGiveFromISR(counting_sem, &taskAwoken);
        }

        return (taskAwoken == pdTRUE);
    }

public:
    PWMDriver(uint8_t pwm_pin = TRIAC_PWM_CHANNEL, bool fade = true, ledc_fade_mode_t fade_time = LEDC_FADE_TIME);
    ~PWMDriver();
    esp_err_t setPWM(uint16_t duty);
    esp_err_t setIntensity(uint8_t intensity);
    esp_err_t setState(bool state);
    bool getState();
    uint8_t getIntensity();
    uint16_t getPWM();
};

#endif // PWM_DRIVER_HPP