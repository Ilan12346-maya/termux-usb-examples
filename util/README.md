# Utilities for USB Device Inspection

This directory contains C programs for listing and inspecting connected USB devices within the Termux environment.

## Files

### `usb_info.c`

This C program displays general information about a single USB device. It takes a file descriptor (provided by `termux-usb`) as an argument. It uses `libusb` functions to retrieve and print the device's Vendor ID, Product ID, Manufacturer, Product Name, and Serial Number.

### `get_device_descriptors.c`

This C program provides a more detailed inspection of a USB device. It also takes a file descriptor as an argument and uses `libusb` to:
- Retrieve and print the main device descriptor.
- Iterate through all configurations, printing their descriptors.
- For each configuration, iterate through all interfaces, printing their descriptors.
- For each interface, iterate through all endpoints, printing their descriptors.
- Attempt to read and display the HID Report Descriptor if the interface is identified as a Human Interface Device (HID).
This program is crucial for in-depth analysis of a device's capabilities and communication structure.

## How It Works (Common to C Programs)

Both `usb_info.c` and `get_device_descriptors.c` utilize the `libusb` library in a specific way to function within Termux:
1.  They disable standard `libusb` device discovery (`LIBUSB_OPTION_NO_DEVICE_DISCOVERY`).
2.  They use `libusb_wrap_sys_device` to "wrap" an existing system file descriptor (provided by `termux-usb -e`) into a `libusb_device_handle`. This allows `libusb` to interact with a specific USB device that Termux has already granted access to, bypassing the typical device enumeration limitations.

## Usage

You can use the `list_all_usb_info.sh` script to list general information (like that provided by `usb_info.c`) for all connected USB devices and find their device paths. Once you have a device path (e.g., `/dev/bus/usb/001/003`), you can use `termux-usb -e` to execute these programs:

*   **To get general USB information:**

    ```bash
    termux-usb -e ./usb_info /dev/bus/usb/001/003
    ```

*   **To get detailed device descriptors:**

    ```bash
    termux-usb -e ./get_device_descriptors /dev/bus/usb/001/003
    ```
    (Replace `/dev/bus/usb/001/003` with the actual device path.)
