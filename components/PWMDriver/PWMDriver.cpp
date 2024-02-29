#include "PWMDriver.hpp"

PWMDriver::PWMDriver(uint8_t pwm_pin, bool fade, ledc_fade_mode_t fade_time)
{
    m_fade = fade;
    m_fade_time = fade_time;
    m_duty = 0;
    m_intensity = 0;
    m_state = false;
    m_semaphore = xSemaphoreCreateBinary();
    m_err = ESP_OK;
    if (m_semaphore == NULL) {
        ESP_LOGE(TAG_PWM_DRIVER, "Semaphore creation failed");
    }
    ledc_timer_config_t ledc_timer;
    ledc_timer.speed_mode = LEDC_MODE;
    ledc_timer.duty_resolution = LEDC_DUTY_RESOLUTION;
    ledc_timer.timer_num = LEDC_HS_TIMER;
    ledc_timer.freq_hz = LEDC_FREQUENCY;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ledc_channel_config_t pwm_channel;
    pwm_channel.speed_mode = LEDC_MODE;
    pwm_channel.channel = LEDC_CHANNEL_0;
    pwm_channel.timer_sel = LEDC_HS_TIMER;
    pwm_channel.duty = 0;
    pwm_channel.hpoint = 0;
    pwm_channel.gpio_num = pwm_pin;
    pwm_channel.intr_type = m_fade ? LEDC_INTR_FADE_END : LEDC_INTR_DISABLE;
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel));

    if (m_fade) {
        ledc_fade_func_install(0);
        ledc_cbs_t callbacks = { .fade_cb = this->cb_ledc_fade_end_event};
        m_semaphore = xSemaphoreCreateCounting(2, 0);
        ledc_cb_register(LEDC_MODE, LEDC_CHANNEL_0, &callbacks, (void *)m_semaphore);
    }
}

PWMDriver::~PWMDriver()
{
    ledc_stop(LEDC_MODE, LEDC_CHANNEL_0, 0);
}

esp_err_t PWMDriver::setPWM(uint16_t duty)
{
    m_duty = duty;
    if (m_fade) {
        m_err = ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL_0, m_duty, m_fade_time);
        m_err |= ledc_fade_start(LEDC_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
        if (m_err != ESP_OK) {
            ESP_LOGE(TAG_PWM_DRIVER, "ledc_set_fade_with_time failed with error: %d", m_err);
        }
        xSemaphoreTake(m_semaphore, portMAX_DELAY);
    } else {
        m_err = ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, m_duty);
        m_err = ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
        if (m_err != ESP_OK) {
            ESP_LOGE(TAG_PWM_DRIVER, "ledc_set_duty failed with error: %d", m_err);
        }
    }
    return m_err;
}

esp_err_t PWMDriver::setIntensity(uint8_t intensity)
{
    printf("Intensity: %d\n", intensity);
    m_intensity = intensity;
    m_duty = (uint16_t)((MAX_DUTY - MIN_DUTY) * (m_intensity / 100.0));
    return ESP_OK;
}

esp_err_t PWMDriver::setState(bool state)
{
    m_state = state;
    return setPWM(m_state ? m_duty : MIN_DUTY);
}

bool PWMDriver::getState()
{
    return m_state;
}

uint8_t PWMDriver::getIntensity()
{
    return m_intensity;
}

uint16_t PWMDriver::getPWM()
{
    return m_duty;
}