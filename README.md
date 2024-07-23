# AOA-PC-Host

- [AOA-PC-Host](#aoa-pc-host)
  - [Install Dependencies](#install-dependencies)
  - [Change sudo password](#change-sudo-password)
  - [Host Side Q\&A](#host-side-qa)
    - [Open USB Returns -3](#open-usb-returns--3)
    - [Checking Device PID and VID](#checking-device-pid-and-vid)
  - [Android Side Q\&A](#android-side-qa)
    - [Troubleshooting `getAccessoryList` Returning `null`](#troubleshooting-getaccessorylist-returning-null)
      - [Reference](#reference)

This repository is dedicated to the Android Open Accessory (AOA) PC Host example project.

For detailed information on what Android Open Accessory is, please [click here](https://source.android.com/docs/core/interaction/accessories/protocol).

## Install Dependencies

```bash
sudo apt update
sudo apt install alsa-base, alsa-utils, alsa-source,libasound2-dev
sudo apt install libusb-1.0-0-dev
sudo apt install build-essential
```

## Change sudo password

open `sudo-gdb` change password

```
password="your-pass-word"
```

## Host Side Q&A

### Open USB Returns -3

Run the program as root using sudo.

### Checking Device PID and VID

To view the list of connected devices along with their Product ID (PID) and Vendor ID (VID), use the `lsusb` command. The PID and VID are displayed in the format `{VID}:{PID}` within the list.

## Android Side Q&A

### Troubleshooting `getAccessoryList` Returning `null`

Sometimes, even when a USB host device is connected to an Android device, the `getAccessoryList` function might return `null`. This issue often arises if the required feature is not supported by the device.

To check if your device supports USB accessories, use the [`hasSystemFeature`](<https://developer.android.com/reference/android/content/pm/PackageManager#hasSystemFeature(java.lang.String)>) method with the `FEATURE_USB_ACCESSORY` parameter. You can find more information about `FEATURE_USB_ACCESSORY` [here](https://developer.android.com/reference/android/content/pm/PackageManager.html#FEATURE_USB_ACCESSORY).

If the method returns `false`, it indicates that the device might be missing some necessary feature files. To resolve this, consider adding `android.hardware.usb.host.xml` and `android.hardware.usb.accessory.xml` to the `/system/etc/permissions` directory.

These XML files can be located within the Android source code.

#### Reference

- [android - Why usbManager.getAcccessoryList(); always returns null? - Stack Overflow](https://stackoverflow.com/questions/8977885/why-usbmanager-getacccessorylist-always-returns-null)
