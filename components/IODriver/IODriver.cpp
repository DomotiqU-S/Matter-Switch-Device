#include "IODriver.hpp"

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <device.h>
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_matter.h>
#include <esp_matter_console.h>

// include freertos task
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "SliderDriver.hpp"

#define DEBOUNCE_TIME 300
#define DEBOUNCE_TIME_TOUCH 20

using namespace esp_matter;
using namespace chip::app::Clusters;
using namespace esp_matter::cluster;

SliderDriver slider;

bool is_pressed = false;
bool is_fading = false;
bool old_state = false;

uint8_t level = 0;
uint16_t old_level = 0;
uint32_t old_tick_btn = 0;
uint32_t old_tick_touch = 0;

static void IRAM_ATTR buttonCb(void* arg) {
    uint32_t tick_now = xTaskGetTickCount();

    if(tick_now - old_tick_btn > DEBOUNCE_TIME) {
        is_pressed = true;
    }

    old_tick_btn = tick_now;
}

void sliderTask(void *pvParameter)
{
    for(;;) {
        if (is_pressed) {
            
            esp_matter_attr_val_t attr_val;
            if (!old_state) {
                attr_val.val.b = true;

                slider.set_level_led(old_level);
            }
            else {
                attr_val.val.b = false;
                slider.set_level_led(0);
            }

            attr_val.type = (esp_matter_val_type_t)1;
            esp_matter::attribute::update(1, OnOff::Id, OnOff::Attributes::OnOff::Id, &attr_val);
            
            old_state = !old_state;

            is_pressed = false;
        }

        if(slider.newTouches()) {
            uint32_t tick_now = xTaskGetTickCount();

            if(tick_now - old_tick_touch > DEBOUNCE_TIME_TOUCH) {
                    old_tick_touch = tick_now;

                slider.updateTouchStatus();
                level = slider.getLevel(254);

                if(level != old_level) {

                    esp_matter_attr_val_t attr_val;
                    attr_val.val.u8 = level;

                    attr_val.type = (esp_matter_val_type_t)2;
                    esp_matter::attribute::update(1, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &attr_val);

                    old_level = level;

                    // if actual state is off, change it to on
                    if(old_state) {
                        attr_val.val.b = true;
                        esp_matter::attribute::update(1, OnOff::Id, OnOff::Attributes::OnOff::Id, &attr_val);
                        old_state = !old_state;
                    }
                }
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

app_driver_handle_t app_driver_switch_init()
{
    HMI_driver_handle_t slider_handle = slider.init();

    //Create a task for the slider
    ESP_LOGI(TAG, "Slider initialization: %d", slider.getFlag());

    // Config the GPIO_PIN 0 as input and install the ISR service for falling edge
    gpio_set_direction((gpio_num_t)CONFIG_PIN_BTN, GPIO_MODE_INPUT);
    gpio_pulldown_en((gpio_num_t)CONFIG_PIN_BTN);
    gpio_pullup_dis((gpio_num_t)CONFIG_PIN_BTN);
    gpio_set_intr_type((gpio_num_t)CONFIG_PIN_BTN, GPIO_INTR_POSEDGE);

    gpio_isr_handler_add((gpio_num_t)CONFIG_PIN_BTN, buttonCb, (void*) CONFIG_PIN_BTN);

    return (app_driver_handle_t)slider_handle;
}

/* Do any conversions/remapping for the actual value here */
esp_err_t app_driver_light_set_power(esp_matter_attr_val_t *val)
{
    #ifdef DEBUG_DRIVER
        ESP_LOGI(TAG, "power: %d", val->val.b);
    #endif
    return slider.set_power(val->val.b);
}

esp_err_t app_driver_light_set_brightness(esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
    #ifdef DEBUG_DRIVER
        ESP_LOGI(TAG, "brightness: %d", value);
    #endif
    slider.set_level_led(value);
    return slider.set_brightness(value);
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    if (endpoint_id == 1) {

        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                slider.set_power(val->val.b);
            }
        }
        else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {

                ESP_LOGI(TAG, "Callback matter attribute bacon");
                // Fade the light with the light driver
                uint8_t level = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
                slider.set_level_led(level);
                slider.set_brightness(level);

                // Save the level for the next iteration
                old_level = val->val.u8;
            }
        }
    }
    return ESP_OK;
}

esp_err_t app_driver_set_default(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;

    /* Cluster brightness */
    esp_matter_attr_val_t val;
    val.val.u8 = DEFAULT_BRIGHTNESS;
    old_level = DEFAULT_BRIGHTNESS;
    err |= app_driver_light_set_brightness(&val);

    /* Cluster state */
    val.type = (esp_matter_val_type_t)1;
    val.val.b = DEFAULT_POWER;
    old_state = DEFAULT_POWER;
    err |= app_driver_light_set_power(&val);

    return err;
}

esp_err_t app_driver_start_sensor()
{
    bool is_configured = slider.start();
    if(is_configured) {
        xTaskCreate(sliderTask, "sliderTask", 4096, NULL, 4, NULL);
    }
    else {
        ESP_LOGE(TAG, "Slider not configured");
    }

    return ESP_OK;
}
