#include "IODriver.hpp"

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <device.h>
#include <esp_matter.h>
#include <esp_matter_console.h>
#include "SliderDriver.hpp"

using namespace esp_matter;
using namespace chip::app::Clusters;
using namespace esp_matter::cluster;

static const char *TAG = "IODriver";
SliderDriver slider;

app_driver_handle_t app_driver_switch_init()
{
    HMI_driver_handle_t slider_handle = slider.slider_init();
    return (app_driver_handle_t)slider_handle;
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    // if the cluster is for on/off switch
    if (endpoint_id == 1) {
        // Get the node id of the device
        //uint64_t node_id = 0;

        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                ESP_LOGI(TAG, "OnOff: %d", val->val.b);
                // log the val type
                ESP_LOGI(TAG, "Val type: %d", (int)val->type);
            }
        }
        else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {

                ESP_LOGI(TAG, "Current level: %d", val->val.u16);


                if (val->val.u16 == 0) {
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
            }
        }
    }
    return ESP_OK;
}

esp_err_t app_driver_set_default(uint16_t endpoint_id)
{
    return ESP_OK;
}