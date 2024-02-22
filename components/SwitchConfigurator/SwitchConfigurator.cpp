#include "SwitchConfigurator.hpp"

endpoint_t *priv_endpoint;

endpoint_t *configureOnOffSwitch(on_off_switch::config_t &c_switch, uint8_t flags, void *priv_data, esp_matter::node_t *node)
{
    priv_endpoint = on_off_switch::create(node, &c_switch, flags, priv_data);
    
    return priv_endpoint;
}

endpoint_t *configureDimmableSwitch(dimmer_switch::config_t &c_switch, uint8_t flags, void *priv_data, esp_matter::node_t *node)
{
    priv_endpoint = dimmer_switch::create(node, &c_switch, flags, priv_data);
    
    return priv_endpoint;
}