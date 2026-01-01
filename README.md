# Termux USB Hardware test

This repository contains a collection of shell scripts and C programs designed for interacting with USB devices within a Termux environment on Android. These scripts can serve as a foundation for exploring USB device information and raw input from gamepads or usb-Serial

## Contents

This repository is organized into the following directories:

*   **`usb-gamepad/`**: Contains C programs and shell scripts for interacting with USB gamepads.
    *   `gamepad_decode.h`: Header file for gamepad decoding.
    *   `read_gamepad.c`: C program to read gamepad input.
    *   `read_gamepad.sh`: Shell script wrapper for `read_gamepad`.
    *   `read_gamepad_raw.c`: C program to read raw gamepad input.
    *   `read_gamepad_raw.sh`: Shell script wrapper for `read_gamepad_raw`.
*   **`usb-serial/`**: Contains C programs and shell scripts for interacting with USB serial devices.
    *   `read_serial.c`: C program to read from a USB serial device.
    *   `read_serial.sh`: Shell script wrapper for `read_serial`.
*   **`util/`**: Contains various utility C programs and shell scripts.
    *   `get_device_descriptors.c`: C program to get detailed USB device descriptors.
    *   `get_device_descriptors.sh`: Shell script wrapper for `get_device_descriptors`.
    *   `list_all_usb_info.sh`: Shell script to list general information about all connected USB devices.
    *   `usb_info.c`: C program to display general USB information.
    *   `usb_info.sh`: Shell script wrapper for `usb_info`.

## Purpose

The primary goal of these scripts is to provide a starting point for developers and enthusiasts who want to:

-   Understand how USB devices are enumerated and described on an Android device via Termux.
-   Access raw input data from USB gamepads (e.g., Xbox gamepads) in a command-line environment.
-   Develop custom applications or drivers that interact with USB hardware in termux.

## Use in Termux

To use these scripts, you first need to identify the device path of your USB device. You can do this by running `list_all_usb_info.sh`:

```bash
./util/list_all_usb_info.sh

Listing information for all connected USB devices:
--------------------------------------------------

Device Path: /dev/bus/usb/001/003
--------------------
Vendor ID: 2341
Product ID: 8036
Manufacturer: Arduino LLC
Product: USB IO Board
Serial No:
--------------------------------------------------
Finished listing USB device information.
```

Once you have the device path (e.g., `/dev/bus/usb/001/003`), you can use the `termux-usb -e` option to execute the desired script with the correct file descriptor.

**Examples:**

*   **Get Device Descriptors:**
    ```bash
    termux-usb -e ./util/get_device_descriptors.sh /dev/bus/usb/001/003
    ```

*   **Read Raw Gamepad Input:**
    ```bash
    termux-usb -e ./usb-gamepad/read_gamepad_raw.sh /dev/bus/usb/001/003
    ```

*   **Read from USB Serial Device:**
    ```bash
    termux-usb -e ./usb-serial/read_serial.sh /dev/bus/usb/001/003
    ```

