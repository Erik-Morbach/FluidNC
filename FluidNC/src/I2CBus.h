#pragma once
#include <driver/i2c.h>

#include "Configuration/Configurable.h"
#include "Pin.h"
#include "Bus.h"

class I2CBus: public Bus{
private:
    void init_i2c_bus();
    void end_i2c_bus();

    i2c_cmd_handle_t begin_cmd(uint8_t device_address, uint8_t resource_address);
    esp_err_t        end_cmd(i2c_cmd_handle_t &cmd, uint32_t timeout, bool delete_cmd=true);
    i2c_port_t _port_num = I2C_NUM_0;
    uint32_t _freq_hz = 1000000;
    i2c_config_t _config;
public:

    Pin _sda;
    Pin _scl;
    I2CBus() = default;

    void validate() const override;
    void group(Configuration::HandlerBase& handler) override;
    void afterParse() override;

    void init();


    esp_err_t write(uint8_t device_address, uint8_t resource_address, const uint8_t *data, uint8_t data_size, uint32_t timeout=2) override;
    esp_err_t read(uint8_t device_address, uint8_t resource_address, uint8_t *data, uint8_t data_size, uint32_t timeout=2) override;

    ~I2CBus();


};