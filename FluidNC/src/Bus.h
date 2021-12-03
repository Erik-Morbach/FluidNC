#pragma once

#include "Configuration/Configurable.h"

class Bus: public Configuration::Configurable{
public:
    virtual esp_err_t write(uint8_t device_address, uint8_t resource_address, const uint8_t *data, uint8_t data_size, uint32_t timeout=2){ return ESP_OK; }
    virtual esp_err_t read(uint8_t device_address, uint8_t resource_address, uint8_t *data, uint8_t data_size, uint32_t timeout=2){ return ESP_OK; }

    virtual void validate() const {};
    virtual void group(Configuration::HandlerBase& handler) {};
    virtual void afterParse() {};

    virtual ~Bus() {};
};
