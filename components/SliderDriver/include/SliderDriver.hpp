#ifndef SLIDER_DRIVER_HPP
#define SLIDER_DRIVER_HPP

#include "CAP1298.hpp"
#include "IS31FL3235A.hpp"
#include "LightDriver.hpp"
#include "driver/gpio.h"
#include "device.h"
#include <esp_err.h>
#include <stdbool.h>
#include <stdint.h>
#include <rom/ets_sys.h>
#include "driver/gptimer.h"

#include "sdkconfig.h"


#define MAX_INTERVAL 8333

typedef void *HMI_driver_handle_t;

class SliderDriver : public LightDriver
{
private:
    bool m_isOn;
    bool start_fade = false;
    bool is_running = false;
    
    uint8_t m_level;
    uint8_t m_previous_level = 0;

    //CAP1298 capacitance_touch;
    //IS31FL3235A led_level_driver;
    gptimer_handle_t timer_handle;
    esp_err_t m_flag;
    gptimer_alarm_config_t alarm_config;

public:

    static bool IRAM_ATTR on_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
        SliderDriver *driver = static_cast<SliderDriver *>(user_ctx);
        driver->stop_timer();

        gpio_set_level((gpio_num_t)CONFIG_TRIAC_PWM, 0);
        gpio_set_level((gpio_num_t)CONFIG_TRIAC_PWM, 1);

        return true;
    }

    static void IRAM_ATTR gpio_isr_handler(void *arg) {
        gpio_set_level((gpio_num_t)CONFIG_TRIAC_PWM, 1);
        SliderDriver *driver = static_cast<SliderDriver *>(arg);
        driver->restart_timer();   
    }

    SliderDriver() /*: capacitance_touch(GPIO_NUM_10, GPIO_NUM_11), led_level_driver(GPIO_NUM_10, GPIO_NUM_11, IS31FL3235A_ADDR)*/ {
        // m_flag = capacitance_touch.begin();
        // m_flag = led_level_driver.begin();

        gptimer_config_t timer_config = {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT, // Source d'horloge par défaut
            .direction = GPTIMER_COUNT_UP,      // Comptage ascendant
            .resolution_hz = 1000000,           // Résolution de 1 MHz (1µs)
        };

        ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer_handle));

        // Configurer l'alarme
        alarm_config.alarm_count = MAX_INTERVAL / 2;
        alarm_config.reload_count = false;
        alarm_config.flags.auto_reload_on_alarm = true;

        gptimer_event_callbacks_t timer_callbacks = {
            .on_alarm = on_timer_alarm_cb,
        };

        ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_handle, &timer_callbacks, (void *)this));
        ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_handle, &alarm_config));
        ESP_ERROR_CHECK(gptimer_enable(timer_handle));


        // Init GPIO
        gpio_set_direction(GPIO_NUM_1, GPIO_MODE_OUTPUT);
        gpio_set_level((gpio_num_t)CONFIG_TRIAC_PWM, 1);

        gpio_set_direction((gpio_num_t)CONFIG_TRIAC_SYNC, GPIO_MODE_INPUT);
        gpio_set_intr_type((gpio_num_t)CONFIG_TRIAC_SYNC, GPIO_INTR_POSEDGE);
        
        gpio_install_isr_service(0);
        gpio_isr_handler_add((gpio_num_t)CONFIG_TRIAC_SYNC, gpio_isr_handler, (void *)this);

    }

    void restart_timer() {
        gptimer_start(timer_handle);
        this->is_running = true;
    }

    void stop_timer() {
        gptimer_stop(timer_handle);
        gptimer_set_raw_count(timer_handle, 0);
        this->is_running = false;
    }

    ~SliderDriver();
    bool start();
    HMI_driver_handle_t init();
    esp_err_t getFlag();
    uint8_t getLevel(uint8_t max_level);
    bool newTouches();
    uint8_t getNewTouches();
    void updateTouchStatus();

    // LightDriver Methods
    uint16_t get_temperature() { return 0; };
    uint8_t get_intensity() { return 0; };
    uint32_t get_duty(uint8_t channel) { return 0; };
    uint16_t get_hue() { return 0; };
    uint8_t get_saturation() { return 0; };
    uint16_t get_x() { return 0; };
    uint16_t get_y() { return 0; };

    esp_err_t set_power(bool power) override;
    esp_err_t set_brightness(uint8_t brightness) override;
    esp_err_t set_color(uint16_t x, uint16_t y) { return 0; };
    esp_err_t set_hue(uint16_t hue) { return 0; };
    esp_err_t set_saturation(uint8_t saturation) { return 0; };
    esp_err_t set_temperature(uint32_t temperature) { return 0; };

    void led_routine() {

    }
};

#endif // SLIDER_DRIVER_HPP