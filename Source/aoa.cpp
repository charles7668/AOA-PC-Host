#include "aoa.h"
#include <iostream>
#include <string.h>
#include "defer.hpp"

using namespace std;

namespace AOA
{
    int AOAProtocolHandler::SwitchToAccessoryMode(libusb_device_handle *dev)
    {
        return libusb_control_transfer(dev,
                                       AOA_CTRL_OUT | LIBUSB_RECIPIENT_DEVICE,
                                       AOA_REQ_ACCESSORY,
                                       0,
                                       0,
                                       NULL,
                                       0,
                                       500);
    }
    int AOAProtocolHandler::SetAudioMode(libusb_device_handle *dev, bool mode)
    {
        int value = mode ? 1 : 0;
        return libusb_control_transfer(dev,
                                       AOA_CTRL_OUT | LIBUSB_RECIPIENT_DEVICE,
                                       AOA_REQ_AUDIO,
                                       value,
                                       0,
                                       NULL,
                                       0,
                                       500);
    }

    int AOAProtocolHandler::GetProtocol(libusb_device_handle *device_handle, uint16_t *protocol)
    {
        return libusb_control_transfer(device_handle,
                                       AOA_CTRL_IN | LIBUSB_RECIPIENT_DEVICE, // bmRequestType
                                       AOA_REQ_PROTOCOL,                      // bRequest
                                       0,                                     // value
                                       0,                                     // index
                                       (uint8_t *)protocol,                   // data buffer
                                       2,                                     // 2 byte
                                       500);                                  // timeout 500ms
    }

    int AOAProtocolHandler::SetProtocol(libusb_device_handle *dev, int idx, const char *buffer)
    {
        return libusb_control_transfer(dev,
                                       AOA_CTRL_OUT | LIBUSB_RECIPIENT_DEVICE,
                                       AOA_REQ_SETPROTO,
                                       0,
                                       idx,
                                       (unsigned char *)buffer,
                                       strlen(buffer) + 1,
                                       500); // timeout
    }

    AOAProtocolHandler::AOAProtocolHandler(libusb_device *device)
    {
        libusb_device_descriptor desc;
        // increase the reference count of the device
        device_ = device;
        libusb_ref_device(device_);
    }
    AOAProtocolHandler::~AOAProtocolHandler()
    {
        libusb_unref_device(device_);
        device_ = nullptr;
    }
    bool AOAProtocolHandler::EnableAccesoryMode()
    {
        int ret = 0;
        libusb_device_descriptor desc = {0};
        ret = libusb_get_device_descriptor(device_, &desc);
        if (ret != 0)
        {
            cerr << "Error getting device descriptor: " << libusb_error_name(ret) << "\n";
            return false;
        }

        libusb_device_handle *device_handle = NULL;

        ret = libusb_open(device_, &device_handle);
        if (ret < 0)
        {
            cerr << "Error opening device: " << libusb_error_name(ret) << "\n";
            return false;
        }
        Defer defer([&]() {
            libusb_close(device_handle);
        });

        uint16_t protocol;
        // get protocol
        ret = GetProtocol(device_handle, &protocol);
        if (ret < 0)
        {
            cerr << "Error getting protocol: " << libusb_error_name(ret) << "\n";
            return false;
        }

        bool setState = true;
        setState &= 0 == SetProtocol(device_handle, AOA_PROTO_MANUFACTURE_INDEX, ADK2012_MANUFACTURE_STRING);
        setState &= 0 == SetProtocol(device_handle, AOA_PROTO_MODEL_INDEX, ADK2012_MODEL_STRING);
        setState &= 0 == SetProtocol(device_handle, AOA_PROTO_DESCRIPTION_INDEX, ADK2012_DESCRIPTION_STRING);
        setState &= 0 == SetProtocol(device_handle, AOA_PROTO_VERSION_INDEX, ADK2012_VERSION_STRING);
        setState &= 0 == SetProtocol(device_handle, AOA_PROTO_URI_INDEX, ADK2012_URI_STRING);
        setState &= 0 == SetProtocol(device_handle, AOA_PROTO_SERIAL_INDEX, ADK2012_SERIAL_STRING);

        if (protocol == 2)
        {
            setState &= 0 == SetAudioMode(device_handle, true);
        }
        setState &= 0 == SwitchToAccessoryMode(device_handle);

        if (!setState)
        {
            cerr << "Error setting protocol: ";
            return false;
        }
        return true;
    }
}