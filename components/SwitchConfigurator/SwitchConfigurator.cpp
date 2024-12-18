#include "SwitchConfigurator.hpp"

endpoint_t *priv_endpoint;

/**
 * @brief The function configure a switch with the given flags, private data and node.
 * 
 * @param flags flags to configure the switch
 * @param priv_data the callback data
 * @param node the node to install the switch
 * @return esp_err_t 
 */
esp_err_t configureSwitch(uint8_t flags, void *priv_data, esp_matter::node_t *node)
{
    free(priv_endpoint);

    priv_endpoint = endpoint::create(node, flags, priv_data);
    descriptor::config_t descriptor;
    descriptor::create(priv_endpoint, &descriptor, CLUSTER_FLAG_SERVER);

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

        //dimmer_switch::create(priv_endpoint, &dimmer_switch, CLUSTER_FLAG_CLIENT | CLUSTER_FLAG_SERVER, priv_data);
        on_off::create(priv_endpoint, &dimmer_actuator.on_off, CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
        level_control::create(priv_endpoint, &dimmer_actuator.level_control, CLUSTER_FLAG_SERVER, level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());

    #else
        #error "Invalid switch type"
    #endif

    return priv_endpoint != nullptr ? ESP_OK : ESP_FAIL;
}