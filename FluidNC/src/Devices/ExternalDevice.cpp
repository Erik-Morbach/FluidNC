#include "ExternalDevice.h"
#include <string>
#include <esp_task.h>

#include "../Configuration/GenericFactory.h"
#include "../Configuration/Configurable.h"

namespace Device{
    //namespace {
    //    DeviceFactory::InstanceBuilder<ExternalDevice> registration("device");
    //}
    void ExternalDevice::validate() const{
    }
    void ExternalDevice::group(Configuration::HandlerBase& handler) {
        static String _element_type;
        static String _rw_flag;
        handler.item("id",_device_address);
        handler.item("name",_name);
        handler.item("address",_resource_address);
        handler.item("quantity",_quantity);
        handler.item("element_type",_element_type);
        if(_element_type.compareTo("analog")==0) _element_size=TYPE_ANALOG;
        else _element_size = TYPE_DIGITAL;
        handler.item("rw",_rw_flag);
        if(_rw_flag.compareTo("read")==0) _flags = ACCESS_READ;
        else _flags = ACCESS_WRITE;
        handler.item("timeout",_timeout);
    }
    void ExternalDevice::afterParse(){
        this->_values = new uint8_t[this->_quantity];
        this->_completed = xSemaphoreCreateMutex();
        char aux[100];
        sprintf(aux, "Device %s : size=%d type=%s",_name.c_str(),_quantity,_element_size==TYPE_DIGITAL?"Digital":"Analog");
        log_info(aux);

    }

    void ExternalDevice::init(){

    }


    void ExternalDevice::write(uint32_t position,uint8_t value){
        if(this->_element_size==TYPE_DIGITAL){
            uint8_t _mask = (1<<(position%8));
            this->_values[position/8] &= ~(value<<_mask);
            this->_values[position/8] |= (value<<_mask);
        }
        else this->_values[position] = value;
    }
    uint8_t ExternalDevice::read(uint32_t position){
        if(this->_element_size==TYPE_DIGITAL){
            uint8_t _mask = (1<<(position%8));
            return this->_values[position/8]&_mask;
        }
        else return this->_values[position];
    }

    bool ExternalDevice::sync(){
        if(!this->_bus) return true;
        uint8_t *_new_values = new uint8_t[this->_quantity];
        bool sucess = true;
        if(this->_flags==ACCESS_READ){
            bool diff = false;
            if(this->_bus->read(this->_device_address,this->_resource_address,_new_values,this->_quantity,this->_timeout)!=ESP_OK)
                sucess = false;
            for(int i=0;i<this->_quantity;++i){
                diff |= (_new_values[i]!=this->_values[i]);
                swap(_new_values[i],_values[i]);
            }
            if(diff) {
                this->on_change();
            }
        } else{
            for(int i=0;i<this->_quantity;++i) _new_values[i] = this->_values[i];
            if(this->_bus->write(this->_device_address,this->_resource_address,_new_values,this->_quantity, this->_timeout)!=ESP_OK)
                sucess = false;
        }
        if(sucess){
            if(xSemaphoreTake(this->_completed,0)!=pdTRUE){
                xSemaphoreGive(this->_completed);
            }
        } 
        return sucess;
    }

    void ExternalDevice::wait(uint32_t timeout) {
        if(!this->_bus) return;
        xSemaphoreTake(this->_completed,pdMS_TO_TICKS(10));
        xSemaphoreTake(this->_completed,pdMS_TO_TICKS(timeout));
        xSemaphoreGive(this->_completed);
    }
}