#    include "../Machine/MachineConfig.h"
#    include "I2CPinDetail.h"

#    include "../Assert.h"

extern "C" void __digitalWrite(pinnum_t pin, uint8_t val);

namespace Pins {
    std::vector<bool> I2CPinDetail::_claimed(nI2CPins, false);

    I2CPinDetail::I2CPinDetail(pinnum_t index, const PinOptionsParser& options) :
        PinDetail(index), _capabilities(PinCapabilities::I2C | PinCapabilities::ADC | PinCapabilities::DAC | PinCapabilities::Input | PinCapabilities::Output), _attributes(Pins::PinAttributes::Undefined),
        _readWriteMask(0) {

        Assert(index < nI2CPins, "Pin number is greater than max %d", nI2CPins - 1);
        Assert(!_claimed[index], "Pin is already used.");
        // User defined pin capabilities
        for(auto opt: options){
            if(opt.is("low")){
                _attributes = _attributes | PinAttributes::ActiveLow;
            }
            else{
                _deviceName = opt();
            }
        }
        _claimed[index] = true;

        // readWriteMask is xor'ed with the value to invert it if active low
        _readWriteMask = _attributes.has(PinAttributes::ActiveLow);
        char s[50]; 
        sprintf(s,"I2C Pin %d in device %s",index,_deviceName.c_str());
        log_info(s);
    }

    PinCapabilities I2CPinDetail::capabilities() const { return  PinCapabilities::I2C | PinCapabilities::ADC | PinCapabilities::DAC | PinCapabilities::Input | PinCapabilities::Output; }

    // The write will not happen immediately; the data stays in 
    // device i2c until next sync()
    void IRAM_ATTR I2CPinDetail::write(int high) {
        int value = _readWriteMask ^ high;
        config->_external_module->write(_deviceName,_index, high);
        config->_external_module->sync(_deviceName);
    }

    // Write and wait for completion.  Not suitable for use from an ISR
    void I2CPinDetail::synchronousWrite(int high) {
        write(high);
        config->_external_module->wait(_deviceName);
    }

    // get data from the last sync()
    int IRAM_ATTR I2CPinDetail::read() {
        auto raw = config->_external_module->read(_deviceName,_index);
        return raw ^ _readWriteMask;
    }

    void I2CPinDetail::setAttr(PinAttributes value) {
        // Check the attributes first:
        Assert(value.validateWith(this->_capabilities), "Requested attributes do not match the I2C pin capabilities");
        Assert(!_attributes.conflictsWith(value), "Attributes on this pin have been set before, and there's a conflict.");

        _attributes = value;

        // I2S out pins cannot be configured, hence there
        // is nothing to do here for them. We basically
        // just check for conflicts above...
    }

    PinAttributes I2CPinDetail::getAttr() const { return _attributes; }

    String I2CPinDetail::toString() {
        auto s = String("I2C.") + int(_index);
        if (_attributes.has(PinAttributes::ActiveLow)) {
            s += ":low";
        }
        s += ":";
        s += this->_deviceName;
        return s;
    }
}
