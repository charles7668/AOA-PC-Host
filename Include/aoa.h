#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <libusb-1.0/libusb.h>

/*
 * AOA 2.0 protocol
 * ref. https://source.android.com/accessories/aoa2.html
 */
// LIBUSB_ENDPOINT_IN == DEVICE_TO_HOST
// LIBUSB_ENDPOINT_OUT == HOST_TO_DEVICE
#define AOA_CTRL_OUT (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT)
#define AOA_CTRL_IN (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN)

#define AOA_REQ_PROTOCOL (51)
#define AOA_REQ_SETPROTO (52)
#define AOA_PROTO_MANUFACTURE_INDEX (0)
#define AOA_PROTO_MODEL_INDEX (1)
#define AOA_PROTO_DESCRIPTION_INDEX (2)
#define AOA_PROTO_VERSION_INDEX (3)
#define AOA_PROTO_URI_INDEX (4)
#define AOA_PROTO_SERIAL_INDEX (5)
#define AOA_REQ_ACCESSORY (53)
#define AOA_REQ_REGISTER_HID (54)
#define AOA_REQ_UNREGISTER_HID (55)
#define AOA_REQ_SET_HID_REPORT (56)
#define AOA_SEND_HID_EVENT (57)
#define AOA_REQ_AUDIO (58)
#define VID_GOOGLE (0x18d1)
#define AOA_PID_BASE (0x2d00)           /* accessory */
#define AOA_PID_WITH_ADB (0x2d01)       /* accessory + adb */
#define AOA_PID_AUDIO_ONLY (0x2d02)     /* audio */
#define AOA_PID_AUDIO_WITH_ADB (0x2d03) /* audio + adb */
#define AOA_PID_WITH_AUDIO (0x2d04)     /* accessory + audio */
#define AOA_PIO_WITH_AUDIO_ADB (0x2d05) /* accessory + audio + adb */

/*
 * from adk2012/board/library/usbh.h
 * usb accssory filter in ADK2012.apk
 * see source ./res/xml/usb_accessory_filter.xml
 *   Manufacture "Google, Inc."
 *   Model       "DemoKit"
 *   Version     "2.0"
 */
#define ADK2012_MANUFACTURE_STRING ("Google, Inc.")
#define ADK2012_MODEL_STRING ("DemoKit")
#define ADK2012_DESCRIPTION_STRING ("DemoKit Arduino Board")
#define ADK2012_VERSION_STRING ("2.0")
#define ADK2012_URI_STRING ("http://www.android.com")
#define ADK2012_SERIAL_STRING ("0000000012345678")

// android app needs to match this
#define BT_ADK_UUID 0x1d, 0xd3, 0x50, 0x50, 0xa4, 0x37, 0x11, 0xe1, 0xb3, 0xdd, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66

namespace AOA
{
    class AOAProtocalHandler
    {
    private:
        libusb_device *device_;
        int GetProtocol(libusb_device_handle *device_handle, uint16_t *protocol);
        int SetProtocol(libusb_device_handle *device_handle, int idx, const char *buffer);
        int SetAudioMode(libusb_device_handle *device_handle, bool mode);
        int SwitchToAccessoryMode(libusb_device_handle *device_handle);

    public:
        AOAProtocalHandler(libusb_device *device);
        ~AOAProtocalHandler();

        bool EnableAccesoryMode();
    };
}