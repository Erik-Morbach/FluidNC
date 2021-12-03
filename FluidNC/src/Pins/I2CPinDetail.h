#pragma once

#include "PinDetail.h"
#include "../Devices/ExternalDevice.h"


namespace Pins {
    class I2CPinDetail : public PinDetail {
        PinCapabilities _capabilities;
        PinAttributes   _attributes;
        int             _readWriteMask;
        String          _deviceName;

        static const int         nI2CPins = 100;
        static std::vector<bool> _claimed;

    public:
        I2CPinDetail(pinnum_t index, const PinOptionsParser& options);

        PinCapabilities capabilities() const override;

        // I/O:
        void          write(int high) override;
        void          synchronousWrite(int high) override;
        int           read() override;
        void          setAttr(PinAttributes value) override;
        PinAttributes getAttr() const override;

        String toString() override;

        ~I2CPinDetail() override { _claimed[_index] = false; }
    };
}
