#include "SensorConfigurator.hpp"

endpoint_t *priv_endpoint;

void configureSensor(uint8_t flags, void *priv_data, esp_matter::node_t *node)
{
    free(priv_endpoint);

    temperature_sensor::config_t temp_config;
    temp_config.temperature_measurement.max_measured_value = 85 * 100;
    temp_config.temperature_measurement.min_measured_value = -40 * 100;
    temp_config.temperature_measurement.measured_value = 25 * 100;

    priv_endpoint = temperature_sensor::create(node, &temp_config, flags, priv_data);

    humidity_sensor::config_t humidity_config;
    humidity_config.relative_humidity_measurement.max_measured_value = 100 * 100;
    humidity_config.relative_humidity_measurement.min_measured_value = 0 * 100;
    humidity_config.relative_humidity_measurement.measured_value = 50 * 100;

    priv_endpoint = humidity_sensor::create(node, &humidity_config, flags, priv_data);

    light_sensor::config_t illuminance_config;
    illuminance_config.illuminance_measurement.illuminance_max_measured_value = 40000;
    illuminance_config.illuminance_measurement.illuminance_min_measured_value = 1;
    illuminance_config.illuminance_measurement.illuminance_measured_value = 1000;

    priv_endpoint = light_sensor::create(node, &illuminance_config, flags, priv_data);
}