#include "IODriver.hpp"

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <device.h>
#include <driver/gpio.h>
#include <esp_matter.h>
#include <esp_matter_console.h>
using namespace esp_matter;
using namespace chip::app::Clusters;
using namespace esp_matter::cluster;

SliderDriver slider;
PWMDriver pwm;

bool isPressed = false;

uint16_t old_level = 0;
bool old_state = false;

static void IRAM_ATTR buttonCb(void* arg) {
    isPressed = !isPressed;
}

void sliderTask(void *pvParameter)
{
    for(;;) {
        if(slider.newTouches()) {
            slider.updateTouchStatus();
            esp_matter_attr_val_t attr_val;
            attr_val.val.u8 = slider.getLevel(100);
            attr_val.type = (esp_matter_val_type_t)8;
            ESP_LOGI("IODriver", "Slider level: %d", attr_val.val.u8);
            esp_matter::attribute::update(1, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, &attr_val);
        }

        // Wait for the semaphore to be given
        if (isPressed) {
            // If the button is pressed, change the state of the light
            ESP_LOGI(TAG, "Button pressed: %d", isPressed);
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
            isPressed = false;
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
                ESP_LOGI(TAG, "OnOff: %d", val->val.b);
                // log the val type
                ESP_LOGI(TAG, "Val type: %d", (int)val->type);
                pwm.setState(val->val.b);
                old_state = val->val.b;
            }
        }
        else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {

                ESP_LOGI(TAG, "Current level: %d", val->val.u16);
                pwm.setIntensity((val->val.u8 * 100) / 255);

                if (val->val.u16 < 1) {
                    esp_matter_attr_val_t attr_val;
                    attr_val.type = (esp_matter_val_type_t)1;
                    attr_val.val.b = false;
                    esp_matter::attribute::update(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &attr_val);
                }
                else {
                    esp_matter_attr_val_t attr_val;
                    attr_val.val.b = true;
                    attr_val.type = (esp_matter_val_type_t)1;
                    esp_matter::attribute::update(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &attr_val);
                }

                // Save the level for the next iteration
                old_level = val->val.u16;
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
    if(is_configured) {
        xTaskCreate(sliderTask, "sliderTask", 4096, NULL, 5, NULL);
    }
    else {
        ESP_LOGE(TAG, "Slider not configured");
    }

    return ESP_OK;
}