// Copyright (c) 2021 - Erik Morbach
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.
#pragma once

#include <vector>

#include "Configuration/Configurable.h"
#include "Devices/ExternalDevice.h"
#include "Bus.h"

class ExternalModule : public Configuration::Configurable {
private:
    TaskHandle_t _handler;
    Bus *_bus;
    std::map<String, Device::ExternalDevice*> _mapper;
public:
    ExternalModule() = default;

    std::vector<Device::ExternalDevice*> _devices;

    void validate() const override;
    void group(Configuration::HandlerBase& handler) override;
    void afterParse() override;

    void init();

    void write(String deviceName, uint32_t position, uint8_t val);
    uint8_t read(String deviceName, uint32_t position);

    bool sync(String deviceName="all");
    void wait(String deviceName="all", uint32_t timeout=100);

    ~ExternalModule() override {
        if(this->_handler!=nullptr) 
            vTaskDelete(this->_handler);
    };
};