#include <iostream>
#include <libusb-1.0/libusb.h>
#include <vector>
#include "aoa.h"
#include <unistd.h>
#include "defer.hpp"

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

int main()
{
    vector<USBIdentity> support_devices{
        USBIdentity{0x18D1, 0xD002}};

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
        AOA::AOAProtocalHandler aoa_handler(target_device);
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
    libusb_free_device_list(dev_lists, 1);
    return 0;
}