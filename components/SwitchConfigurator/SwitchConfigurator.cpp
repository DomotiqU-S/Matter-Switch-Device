#include "SwitchConfigurator.hpp"

endpoint_t *priv_endpoint;

endpoint_t* configureSwitch(uint8_t flags, void *priv_data, esp_matter::node_t *node)
{
    free(priv_endpoint);

    priv_endpoint = endpoint::create(node, flags, priv_data);
    descriptor::config_t descriptor;
    descriptor::create(priv_endpoint, &descriptor, flags);

    #if SWITCH_TYPE == SWITCH_ON_OFF
        on_off_plugin_unit::config_t on_off_actuator;
        on_off_switch::config_t on_off_switch;

        on_off_actuator.on_off.on_off = true;

        endpoint::add_device_type(priv_endpoint, on_off_switch::get_device_type_id(), on_off_switch::get_device_type_version());

        on_off_switch::create(priv_endpoint, &on_off_switch, flags, priv_data);
        on_off_plugin_unit::create(priv_endpoint, &on_off_actuator, flags, priv_data);

    #elif SWITCH_TYPE == SWITCH_DIMMABLE
        dimmable_plugin_unit::config_t dimmer_actuator;
        dimmer_switch::config_t dimmer_switch;

        dimmer_actuator.on_off.on_off = DEFAULT_STATE;
        dimmer_actuator.level_control.current_level = DEFAULT_INTENSITY;
        dimmer_actuator.level_control.on_level = DEFAULT_INTENSITY;

        endpoint::add_device_type(priv_endpoint, dimmer_switch::get_device_type_id(), dimmer_switch::get_device_type_version());
        endpoint::add_device_type(priv_endpoint, dimmable_plugin_unit::get_device_type_id(), dimmable_plugin_unit::get_device_type_version());

        dimmer_switch::create(priv_endpoint, &dimmer_switch, flags, priv_data);
        dimmable_plugin_unit::create(priv_endpoint, &dimmer_actuator, flags, priv_data);

    #else
        #error "Invalid switch type"
    #endif

    return priv_endpoint;
}