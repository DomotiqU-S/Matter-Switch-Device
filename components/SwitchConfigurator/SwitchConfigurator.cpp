#include "SwitchConfigurator.hpp"

endpoint_t *priv_endpoint;

endpoint_t *configureOnOffSwitch(on_off_switch::config_t &c_switch, uint8_t flags, void *priv_data, esp_matter::node_t *node)
{
    priv_endpoint = on_off_switch::create(node, &c_switch, flags, priv_data);
    
    return priv_endpoint;
}

endpoint_t *configureDimmableSwitch(dimmable_plugin_unit::config_t &s_switch, dimmer_switch::config_t &c_switch, uint8_t flags, void *priv_data, esp_matter::node_t *node)
{
    //priv_endpoint = dimmer_switch::create(node, &c_switch, flags, priv_data);
    s_switch.on_off.on_off = false;
    s_switch.level_control.current_level = (uint8_t)254;
    priv_endpoint = dimmable_plugin_unit::create(node, &s_switch, flags, priv_data);

    // add switch client cluster
    priv_data = dimmer_switch::create(node, &c_switch, flags, priv_data);
    
    return priv_endpoint;
}