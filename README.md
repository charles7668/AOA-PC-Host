# AOA-PC-Host

This repository is dedicated to the Android Open Accessory (AOA) PC Host example project.

For detailed information on what Android Open Accessory is, please [click here](https://source.android.com/docs/core/interaction/accessories/protocol).

### Install Dependencies

```bash
sudo apt update
sudo apt install alsa-base, alsa-utils, alsa-source,libasound2-dev
sudo apt install libusb-1.0-0-dev
sudo apt install build-essential
```

## Q&A

### Open USB Returns -3

Run the program as root using sudo.

### Checking Device PID and VID

To view the list of connected devices along with their Product ID (PID) and Vendor ID (VID), use the `lsusb` command. The PID and VID are displayed in the format `{VID}:{PID}` within the list.
