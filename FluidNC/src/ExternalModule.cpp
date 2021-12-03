#include "ExternalModule.h"
#include "I2CBus.h"
#include "Configuration/GenericFactory.h"
#include "Machine/MachineConfig.h"

void ExternalModule::validate() const{

}

void ExternalModule::group(Configuration::HandlerBase& handler) {
    static String busType;
    handler.item("type", busType);
    busType.toLowerCase();
    this->_bus = nullptr;
    if(busType.compareTo("i2c")==0){
        this->_bus = config->_i2c;
    }
    _devices.resize(1);
    _devices[0]= nullptr;
    handler.section("device",_devices[0],_bus);
    if(_devices[0]!=nullptr)
        _mapper[_devices[0]->_name] = _devices[0];
}

void ExternalModule::write(String deviceName, uint32_t position, uint8_t val){
    _mapper[deviceName]->write(position,val);
}
uint8_t ExternalModule::read(String deviceName, uint32_t position){
    return _mapper[deviceName]->read(position);
}

bool ExternalModule::sync(String deviceName){
    return _mapper[deviceName]->sync();
}
void ExternalModule::wait(String deviceName, uint32_t timeout){
    _mapper[deviceName]->wait(timeout);
}

void syncDevices(void *param){
    if(!param) return;
    ExternalModule *ex_module = (ExternalModule*)param;
    while(1){
        for(auto *device: ex_module->_devices){
            device->sync();
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void ExternalModule::afterParse(){
    for(auto *device: _devices){
        if(device) device->afterParse();
    }
    if(this->_handler!=nullptr) return;
    //xTaskCreate(syncDevices,"ExternalDevices",8192,this,1,&this->_handler);
}