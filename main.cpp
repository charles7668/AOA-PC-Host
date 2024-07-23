#include <iostream>
#include <libusb-1.0/libusb.h>
#include <vector>
#include "aoa.h"
#include <unistd.h>
#include "defer.hpp"
#include <string.h>

using namespace std;

typedef struct USBIdentity
{
    int pid;
    int vid;
} USBIdentity;

libusb_device *find_target_device(libusb_device **dev_list, int dev_count, vector<USBIdentity> &supportDevice)
{
    libusb_device *target_device = nullptr;
    for (int i = 0; i < dev_count; i++)
    {
        libusb_device *device = dev_list[i];
        libusb_device_descriptor desc;
        int ret = libusb_get_device_descriptor(device, &desc);
        if (ret != 0)
        {
            cerr << "Error getting device descriptor: " << libusb_error_name(ret) << "\n";
            return nullptr;
        }

        for (auto &identity : supportDevice)
        {
            if (desc.idVendor == identity.vid && desc.idProduct == identity.pid)
            {
                target_device = device;
                break;
            }
        }
    }
    return target_device;
}

libusb_device *find_aoa_device(libusb_device **dev_list, int dev_count)
{
    libusb_device *target_device = nullptr;
    for (int i = 0; i < dev_count; i++)
    {
        libusb_device *device = dev_list[i];
        libusb_device_descriptor desc;
        int ret = libusb_get_device_descriptor(device, &desc);
        if (ret != 0)
        {
            cerr << "Error getting device descriptor: " << libusb_error_name(ret) << "\n";
            return nullptr;
        }

        if (((desc.idVendor == VID_GOOGLE) &&
             (desc.idProduct >= 0x2d00) &&
             (desc.idProduct <= 0x2d05)))
        {
            target_device = device;
            break;
        }
    }
    return target_device;
}

int get_endpoint(libusb_device *device, uint8_t &ep_in, uint8_t &ep_out)
{
    libusb_config_descriptor *desc;
    int ret = libusb_get_active_config_descriptor(device, &desc);
    if (ret != 0)
    {
        cerr << "Error getting device descriptor: " << libusb_error_name(ret) << "\n";
        return ret;
    }
    for (int j = 0; j < desc->bNumInterfaces; j++)
    {
        const libusb_interface *interface = &desc->interface[j];
        for (int k = 0; k < interface->num_altsetting; k++)
        {
            const libusb_interface_descriptor &altsetting = interface->altsetting[j];
            for (int k = 0; k < altsetting.bNumEndpoints; ++k)
            {
                const libusb_endpoint_descriptor &endpoint = altsetting.endpoint[k];
                if (endpoint.bmAttributes == LIBUSB_TRANSFER_TYPE_BULK)
                {
                    if (endpoint.bEndpointAddress & LIBUSB_ENDPOINT_IN)
                    {
                        cout << "Found IN endpoint: " << (int)endpoint.bEndpointAddress << "\n";
                        ep_in = endpoint.bEndpointAddress;
                    }
                    else
                    {
                        cout << "Found OUT endpoint: " << (int)endpoint.bEndpointAddress << "\n";
                        ep_out = endpoint.bEndpointAddress;
                    }
                }
            }
        }
    }
    return 0;
}

int main()
{
    vector<USBIdentity> support_devices(
        {USBIdentity{0xd002 , 0x18d1},
        USBIdentity{0x2d01 , 0x18d1}}
    );

    libusb_context *ctx = nullptr;
    int ret = libusb_init(&ctx);
    if (ret != 0)
    {
        cerr << "Error initializing libusb: " << libusb_error_name(ret) << "\n";
        return 1;
    }
    Defer defer([&]()
                { libusb_exit(ctx); });

    cout << "libusb initialized" << "\n";

    libusb_device **dev_lists;
    int count = libusb_get_device_list(ctx, &dev_lists);
    if (count <= 0)
    {
        cout << "No USB devices found" << "\n";
        return 1;
    }
    cout << "Found " << count << " USB devices" << "\n";
    libusb_device *target_device = find_target_device(dev_lists, count, support_devices);
    if (target_device == nullptr)
    {
        cout << "No target device found" << "\n";
        return 1;
    }
    {
        AOA::AOAProtocolHandler aoa_handler(target_device);
        bool enabled = aoa_handler.EnableAccesoryMode();
        if (!enabled)
        {
            cerr << "Error enabling accessory mode\n";
            return 1;
        }
    }
    libusb_free_device_list(dev_lists, 1);
    // wait few seconds to let the device switch to accessory mode
    sleep(5);
    count = libusb_get_device_list(ctx, &dev_lists);
    if (count <= 0)
    {
        cout << "No USB devices found" << "\n";
        return 1;
    }
    cout << "Found " << count << " USB devices" << "\n";
    target_device = find_aoa_device(dev_lists, count);
    if (target_device == nullptr)
    {
        cout << "No AOA device found" << "\n";
        return 1;
    }
    libusb_device_descriptor desc;
    ret = libusb_get_device_descriptor(target_device, &desc);
    if (ret != 0)
    {
        cerr << "Error getting device descriptor: " << libusb_error_name(ret) << "\n";
        return 1;
    }
    cout << "Found AOA device: VID: " << desc.idVendor << " PID: " << desc.idProduct << "\n";
    uint8_t ep_in = 0, ep_out = 0;
    ret = get_endpoint(target_device, ep_in, ep_out);
    if (ret != 0)
    {
        return 1;
    }
    unsigned char data[4096]{0};
    int actual_length = 0;
    libusb_device_handle *handle;
    ret = libusb_open(target_device, &handle);
    if (ret != 0)
    {
        cerr << "Error opening device: " << libusb_error_name(ret) << "\n";
        return 1;
    }
    cout << "device connected" << "\n";
    while (true)
    {
        int transfer_ret = libusb_bulk_transfer(handle, ep_in, data, sizeof(data), &actual_length, 1000);
        if (transfer_ret == LIBUSB_ERROR_NO_DEVICE)
        {
            cout << "Device disconnected\n";
            break;
        }
        if(transfer_ret == 0)
        {
            cout << "Transfer ret: " << transfer_ret << " actual length: " << actual_length << "\n";
            cout << (char*)data << endl;
            memset(data , 4096 , 0);
        }

    }
    libusb_close(handle);
    libusb_free_device_list(dev_lists, 1);
    return 0;
}