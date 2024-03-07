#include "SensorConfigurator.hpp"

endpoint_t *priv_endpoint;

void configureSensor(uint8_t flags, void *priv_data, esp_matter::node_t *node)
{
    priv_endpoint = endpoint::create(node, flags, priv_data);
    
    descriptor::config_t desc_config;
    descriptor::create(priv_endpoint, &desc_config, CLUSTER_FLAG_SERVER);

    endpoint::add_device_type(priv_endpoint, temperature_sensor::get_device_type_id(), temperature_sensor::get_device_type_version());
    endpoint::add_device_type(priv_endpoint, humidity_sensor::get_device_type_id(), humidity_sensor::get_device_type_version());
    endpoint::add_device_type(priv_endpoint, light_sensor::get_device_type_id(), light_sensor::get_device_type_version());
}