#include "SwitchConfigurator.hpp"

endpoint_t *priv_endpoint;

esp_err_t configureOnOffSwitch(uint8_t flags, void *priv_data, esp_matter::node_t *node)
{
    free(priv_endpoint);

    on_off_plugin_unit::config_t on_off_switch;

    on_off_switch.on_off.on_off = true;

    priv_endpoint = endpoint::create(node, flags, priv_data);
    on_off_plugin_unit::create(priv_endpoint, &on_off_switch, flags, priv_data);

    return priv_endpoint != NULL ? ESP_OK : ESP_FAIL;
}

esp_err_t configureDimmableSwitch(uint8_t flags, void *priv_data, esp_matter::node_t *node)
{
    free(priv_endpoint);

    dimmable_plugin_unit::config_t dimmer_actuator;
    dimmer_switch::config_t dimmer_switch;

    dimmer_actuator.on_off.on_off = true;
    dimmer_actuator.level_control.current_level = (uint8_t)69;
    dimmer_actuator.level_control.on_level = (uint8_t)69;

    priv_endpoint = endpoint::create(node, flags, priv_data);

    dimmer_switch::create(priv_endpoint, &dimmer_switch, flags, priv_data);
    dimmable_plugin_unit::create(priv_endpoint, &dimmer_actuator, flags, priv_data);

    return priv_endpoint != NULL ? ESP_OK : ESP_FAIL;
}

endpoint_t *getEndpoint()
{
    return priv_endpoint;
}