#ifndef IODRIVER_HPP
#define IODRIVER_HPP

#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>

#define MATTER_BRIGHTNESS 254
#define STANDARD_BRIGHTNESS 100

#define DEFAULT_POWER false
#define DEFAULT_BRIGHTNESS 60

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include "esp_openthread_types.h"
#endif

typedef void* app_driver_handle_t;

app_driver_handle_t app_driver_switch_init();

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val);

esp_err_t app_driver_set_default(uint16_t endpoint_id);

esp_err_t app_driver_start_sensor();

esp_err_t app_driver_light_set_power(esp_matter_attr_val_t *val);

esp_err_t app_driver_light_set_brightness(esp_matter_attr_val_t *val);

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                                           \
    {                                                                                   \
        .radio_mode = RADIO_MODE_NATIVE,                                                \
    }

#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                                            \
    {                                                                                   \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,                              \
    }

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()                                            \
    {                                                                                   \
        .storage_partition_name = "nvs", .netif_queue_size = 10, .task_queue_size = 10, \
    }
#endif

#endif // IODRIVER_HPP