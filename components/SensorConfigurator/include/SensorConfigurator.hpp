#ifndef SENSOR_CONFIGURATOR_HPP
#define SENSOR_CONFIGURATOR_HPP

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>
#include <stdio.h>

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace esp_matter::cluster;

/**
 * @brief Create a new endpoint for a sensor
 * Add the temperature, humidity and light sensor device types to the endpoint
 * 
 * @param flags 
 * @param priv_data 
 * @param node 
 */
void configureSensor(uint8_t flags, void *priv_data, esp_matter::node_t *node);

#endif // SENSOR_CONFIGURATOR_HPP