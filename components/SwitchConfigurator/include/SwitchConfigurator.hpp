#ifndef SWITCH_CONFIGURATOR_HPP
#define SWITCH_CONFIGURATOR_HPP

#include <stdio.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

#include "sdkconfig.h"

#define SWITCH_ON_OFF 0
#define SWITCH_DIMMABLE 1

#define SWITCH_TYPE CONFIG_DIMMER_TYPE

#define DEFAULT_INTENSITY (uint8_t)180
#define DEFAULT_STATE true

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace esp_matter::cluster;

esp_err_t configureSwitch(uint8_t flags, void *priv_data, esp_matter::node_t *node);

#endif // SWITCH_CONFIGURATOR_HPPdimmer_switch