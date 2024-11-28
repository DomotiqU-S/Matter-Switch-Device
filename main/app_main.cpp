/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>
#include "IODriver.hpp"
#include "SwitchConfigurator.hpp"
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include <platform/ESP32/OpenthreadLauncher.h>
#endif

static const char *TAG = "app_main";
uint16_t sensor_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type)
    {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address Changed");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning failed, fail safe timer expired");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        // Start Sensor Task
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        break;

    default:
        break;
    }
}

// This callback is invoked when clients interact with the Identify Cluster.
// In the callback implementation, an endpoint can identify itself. (e.g., by flashing an LED or light).
static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %u, effect: %u, variant: %u", type, effect_id, effect_variant);
    return ESP_OK;
}

// This callback is called for every attribute update. The callback implementation shall
// handle the desired attributes and return an appropriate error code. If the attribute
// is not of your interest, please do not return an error code and strictly return ESP_OK.
static esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    if (type == PRE_UPDATE)
    {
        app_driver_attribute_update((app_driver_handle_t)priv_data, endpoint_id, cluster_id, attribute_id, val);
    }

    return ESP_OK;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize driver */
    app_driver_handle_t switch_handle = app_driver_switch_init();

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);

    esp_err_t ret = configureSwitch(ENDPOINT_FLAG_NONE, switch_handle, node);

    /* These node and endpoint handles can be used to create/add other endpoints and clusters. */
    if (!node || ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Matter node creation failed");
    }

    // switch_endpoint_id = endpoint::get_id(endpoint_switch);
    // ESP_LOGI(TAG, "Switch created with endpoint_id %d", switch_endpoint_id);

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    /* Set OpenThread platform config */
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };
    set_openthread_platform_config(&config);
#endif

    // /* Matter start */
    // err = esp_matter::start(app_event_cb);
    // if (err != ESP_OK) {
    //     ESP_LOGE(TAG, "Matter start failed: %d", err);
    // }

    // app_driver_start_sensor();

    app_driver_loop();
    // for (;;)
    // {
    //     // gpio_get_level((gpio_num_t)CONFIG_TRIAC_SYNC);
    //     // gpio_set_level((gpio_num_t)CONFIG_PIN_RELAY_ENABLE, true);
    //     // gpio_set_level((gpio_num_t)CONFIG_TRIAC_PWM, 1);
    //     ESP_LOGI(TAG, "PWM ON");
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    //     // gpio_set_level((gpio_num_t)CONFIG_PIN_RELAY_ENABLE, true);
    //     // gpio_set_level((gpio_num_t)CONFIG_TRIAC_PWM, 0);
    //     ESP_LOGI(TAG, "PWM OFF");
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }

    // app_driver_set_default(1);
}
