#include "IODriver.hpp"

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <device.h>
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_matter.h>
#include <esp_matter_console.h>

#include "SliderDriver.hpp"

using namespace esp_matter;
using namespace chip::app::Clusters;
using namespace esp_matter::cluster;

SliderDriver slider;

bool is_pressed = false;
bool is_fading = false;
bool old_state = false;

uint8_t level = 0;
uint16_t old_level = 0;

static void IRAM_ATTR buttonCb(void* arg) {
    is_pressed = !is_pressed;
}

static void IRAM_ATTR touchPnl(void* arg) {
    // Get the touch status
    level = slider.getLevel(100);

    // Update the touch status
    slider.updateTouchStatus();
}

void sliderTask(void *pvParameter)
{
    for(;;) {
        if (is_pressed) {

            // If the button is pressed, change the state of the light
            ESP_LOGI(TAG, "Button pressed: %d", is_pressed);
            
            esp_matter_attr_val_t attr_val;
            if (!old_state) {
                attr_val.val.b = true;
            }
            else {
                attr_val.val.b = false;
            }

            if(attr_val.val.b) {
                esp_matter_attr_val_t attr_val;
                attr_val.val.u16 = old_level;
                attr_val.type = (esp_matter_val_type_t)8;
                esp_matter::attribute::update(1, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &attr_val);
            }

            else {
                attr_val.type = (esp_matter_val_type_t)1;
                esp_matter::attribute::update(1, OnOff::Id, OnOff::Attributes::OnOff::Id, &attr_val);
            }
            is_pressed = false;
        }

        if (is_fading) {
            // If the light is fading, update the level
            esp_matter_attr_val_t attr_val;

            attr_val.val.u16 = level;
            attr_val.type = (esp_matter_val_type_t)8;

            esp_matter::attribute::update(1, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &attr_val);
            is_fading = false;
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
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT);
    gpio_pulldown_en(GPIO_NUM_5);
    gpio_pullup_dis(GPIO_NUM_5);
    gpio_set_intr_type(GPIO_NUM_5, GPIO_INTR_NEGEDGE);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_5, buttonCb, (void*) GPIO_NUM_5);

    gpio_set_direction(GPIO_NUM_6, GPIO_MODE_INPUT);
    gpio_pulldown_en(GPIO_NUM_6);
    gpio_pullup_dis(GPIO_NUM_6);
    gpio_set_intr_type(GPIO_NUM_6, GPIO_INTR_POSEDGE);

    // gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_6, touchPnl, (void*) GPIO_NUM_6);

    return (app_driver_handle_t)slider_handle;
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    if (endpoint_id == 1) {
        // Get the node id of the device
        //uint64_t node_id = 0;

        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                // ESP_LOGI("IODriver", "OnOff: %d", val->val.b);
                slider.set_power(val->val.b);
            }
        }
        else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {

                // ESP_LOGI("IODriver", "Current level: %d", val->val.u8);

                // Fade the light with the light driver
                slider.set_brightness(val->val.u8);

                // Save the level for the next iteration
                old_level = val->val.u8;
            }
        }
    }
    return ESP_OK;
}

esp_err_t app_driver_set_default(uint16_t endpoint_id)
{
    return ESP_OK;
}

esp_err_t app_driver_start_sensor()
{
    bool is_configured = slider.start();
    // if(is_configured) {
    //     xTaskCreate(sliderTask, "sliderTask", 4096, NULL, 5, NULL);
    // }
    // else {
    //     ESP_LOGE(TAG, "Slider not configured");
    // }

    return ESP_OK;
}