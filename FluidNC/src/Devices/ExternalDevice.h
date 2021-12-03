#pragma once

#include <string>
#include <esp_task.h>

#include "../Configuration/GenericFactory.h"
#include "../Configuration/Configurable.h"
#include "../Pins/PinDetail.h"
#include "../Bus.h"

namespace Device{
    enum Type{ TYPE_ANALOG, TYPE_DIGITAL };
    enum Access{ ACCESS_READ, ACCESS_WRITE };

    class ExternalDevice : public Configuration::Configurable{
    private:
        uint8_t *_values;
        Bus *_bus;
        SemaphoreHandle_t _completed;
    public:
        ExternalDevice(Bus *bus=nullptr):_bus(bus) {}

        uint8_t _device_address;
        String _name;
        uint8_t _resource_address;
        uint32_t _quantity;
        Type _element_size;
        Access _flags;
        uint32_t _timeout;
        std::vector<Pins::PinDetail*> _connected_pins;
        

        void validate() const override;
        void group(Configuration::HandlerBase& handler) override;
        void afterParse() override;


        void init();


        void write(uint32_t position,uint8_t value);
        uint8_t read(uint32_t position);

        bool sync();
        void wait(uint32_t timeout=100);


        virtual const char* name() const { return "device"; }

        virtual void on_change(){};

        ~ExternalDevice() = default;
    };
    //using DeviceFactory = Configuration::GenericFactory<Device::ExternalDevice>;
}