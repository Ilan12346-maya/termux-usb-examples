# USB Gamepad Communication

This directory contains C programs and shell scripts for reading data from a USB gamepad in Termux.

## Files

-   **`read_gamepad_raw.c`**: A C program that reads raw data from a USB gamepad using `libusb`. This is useful for understanding the raw data format of a gamepad.
-   **`read_gamepad_raw.sh`**: A wrapper script that executes the `read_gamepad_raw` program with the file descriptor provided by `termux-usb`.
-   **`read_gamepad.c`**: A C program that reads and decodes gamepad data.
-   **`read_gamepad.sh`**: A wrapper script that executes the `read_gamepad` program.
-   **`gamepad_decode.h`**: A header file containing decoding logic for gamepads.

## How It Works

These scripts rely on `libusb` and the `termux-usb` utility to access the USB device. The process involves the following steps:

1.  **Device Access**: The `termux-usb` command provides a file descriptor for the specified USB device to the shell script.

2.  **Device Wrapping**: The C program uses `libusb_wrap_sys_device` to wrap the system device associated with the file descriptor. This allows `libusb` to work with the device without needing to perform device discovery.

3.  **Kernel Driver**: The program checks if a kernel driver is already attached to the device. If so, it detaches the kernel driver to allow `libusb` to claim the interface.

4.  **Claiming Interface**: The program claims the USB interface to begin communication.

5.  **Interrupt Transfer**: The program uses `libusb_interrupt_transfer` to read data from the gamepad's interrupt endpoint.

6.  **Driver Re-attachment**: After reading the data, the program re-attaches the kernel driver so that the operating system can resume normal operation with the device.

## Usage

1.  Identify the device path of your USB gamepad using `../../util/list_all_usb_info.sh`.

2.  Use `termux-usb` to execute the desired script with the device path:

    *   **To read raw gamepad data:**

        ```bash
        termux-usb -e ./read_gamepad_raw /dev/bus/usb/001/005
        ```

    *   **To read decoded gamepad data:**
        ```bash
        termux-usb -e ./read_gamepad /dev/bus/usb/001/005
        ```

    (Replace `/dev/bus/usb/001/005` with the actual device path of your USB gamepad.)

The program will then read and print the data from the gamepad.
