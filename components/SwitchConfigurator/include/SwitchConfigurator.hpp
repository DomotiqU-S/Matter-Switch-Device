#ifndef SWITCH_CONFIGURATOR_HPP
#define SWITCH_CONFIGURATOR_HPP

#include <stdio.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

endpoint_t* configureOnOffSwitch(on_off_switch::config_t &c_switch, uint8_t flags, void *priv_data, esp_matter::node_t *node);
endpoint_t* configureDimmableSwitch(dimmable_plugin_unit::config_t &s_switch, dimmer_switch::config_t &c_switch, uint8_t flags, void *priv_data, esp_matter::node_t *node);

#endif // SWITCH_CONFIGURATOR_HPPdimmer_switch