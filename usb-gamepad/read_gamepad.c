#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <libusb-1.0/libusb.h>
#include <errno.h>
#include <unistd.h> // For close
#include <string.h> // For memset
#include <time.h>     // For time()

#include "gamepad_decode.h" // Include our new header


#define VENDOR_ID 0x045e // ZhiXu Controller Vendor ID
#define PRODUCT_ID 0x028e // ZhiXu Controller Product ID

// Helper function to convert transfer type to string for better readability
const char* libusb_transfer_type_to_string(enum libusb_transfer_type type) {
    switch (type) {
        case LIBUSB_TRANSFER_TYPE_CONTROL: return "Control";
        case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS: return "Isochronous";
        case LIBUSB_TRANSFER_TYPE_BULK: return "Bulk";
        case LIBUSB_TRANSFER_TYPE_INTERRUPT: return "Interrupt";
        default: return "Unknown";
    }
}




// Function to interpret the 20-byte raw gamepad data
void interpret_gamepad_report(unsigned char *data, int actual_length) {
    // Clear screen and move cursor to home (0,0) position
    fprintf(stderr, "\033[2J\033[H");
    
    if (actual_length != 20) {
        fprintf(stderr, "Warning: Expected 20 bytes, but received %d bytes for interpretation.\n", actual_length);
        return;
    }

    // --- DPAD Parsing from data[2] (bitfield) ---
    uint8_t dpad_byte = data[2];
    int DPAD_UP_STATE = (dpad_byte & DPAD_UP) ? 1 : 0;
    int DPAD_DOWN_STATE = (dpad_byte & DPAD_DOWN) ? 1 : 0;
    int DPAD_LEFT_STATE = (dpad_byte & DPAD_LEFT) ? 1 : 0;
    int DPAD_RIGHT_STATE = (dpad_byte & DPAD_RIGHT) ? 1 : 0;

    // --- Buttons from data[3] (bitfield) ---
    uint8_t buttons_byte = data[3];
    int BUTTON_L1_STATE = (buttons_byte & BTN_L1) ? 1 : 0;
    int BUTTON_R1_STATE = (buttons_byte & BTN_R1) ? 1 : 0;
    int BUTTON_HOME_STATE = (buttons_byte & BTN_HOME) ? 1 : 0;
    int BUTTON_A_STATE = (buttons_byte & BTN_A) ? 1 : 0;
    int BUTTON_B_STATE = (buttons_byte & BTN_B) ? 1 : 0;
    int BUTTON_X_STATE = (buttons_byte & BTN_X) ? 1 : 0;
    int BUTTON_Y_STATE = (buttons_byte & BTN_Y) ? 1 : 0;

    // --- System / Stick Buttons from data[2] (HIGH nibble of dpad_byte) ---
    int BUTTON_L3_STATE = (dpad_byte & BTN_L3) ? 1 : 0;
    int BUTTON_R3_STATE = (dpad_byte & BTN_R3) ? 1 : 0;
    int BUTTON_BACK_STATE = (dpad_byte & BTN_BACK) ? 1 : 0;
    int BUTTON_START_STATE = (dpad_byte & BTN_START) ? 1 : 0;

    // --- Analog Triggers from data[4] and data[5] ---
    uint8_t LEFT_TRIGGER_ANALOG_VALUE = data[4];
    uint8_t RIGHT_TRIGGER_ANALOG_VALUE = data[5];

    // --- Analog Sticks (Little Endian signed 16-bit) ---
    int16_t LEFT_X = (int16_t)((data[7] << 8) | data[6]);   // LSB data[6], MSB data[7]
    int16_t LEFT_Y = (int16_t)((data[9] << 8) | data[8]);  // LSB data[8], MSB data[9]
    int16_t RIGHT_X = (int16_t)((data[11] << 8) | data[10]);// LSB data[10], MSB data[11]
    int16_t RIGHT_Y = (int16_t)((data[13] << 8) | data[12]);// LSB data[12], MSB data[13]


    // Print human-readable output in the specified format
    fprintf(stderr, "--- Gamepad State ---\n\n");

    fprintf(stderr, "Raw: ");
    for (int i = 0; i < actual_length; ++i) {
        fprintf(stderr, "%02x ", data[i]);
    }
    fprintf(stderr, "\n\n");

    fprintf(stderr, "DPAD:\n");
    fprintf(stderr, "Raw[2 low] = 0x%x\n\n", dpad_byte & 0x0F); // Low nibble for DPAD
    fprintf(stderr, "Up: %s\n", DPAD_UP_STATE ? "Pressed" : "Released");
    fprintf(stderr, "Down: %s\n", DPAD_DOWN_STATE ? "Pressed" : "Released");
    fprintf(stderr, "Left: %s\n", DPAD_LEFT_STATE ? "Pressed" : "Released");
    fprintf(stderr, "Right: %s\n\n", DPAD_RIGHT_STATE ? "Pressed" : "Released");

    fprintf(stderr, "Buttons:\n");
    fprintf(stderr, "Raw[2 high] = 0x%x\n\n", (dpad_byte >> 4) & 0x0F); // High nibble for System Buttons
    fprintf(stderr, "L3: %s\n", BUTTON_L3_STATE ? "Pressed" : "Released");
    fprintf(stderr, "R3: %s\n", BUTTON_R3_STATE ? "Pressed" : "Released");
    fprintf(stderr, "Start: %s\n", BUTTON_START_STATE ? "Pressed" : "Released");
    fprintf(stderr, "Back: %s\n\n", BUTTON_BACK_STATE ? "Pressed" : "Released");

    fprintf(stderr, "Raw[3 high] = 0x%x\n\n", (buttons_byte >> 4) & 0x0F); // High nibble for ABXY
    fprintf(stderr, "A: %s\n", BUTTON_A_STATE ? "Pressed" : "Released");
    fprintf(stderr, "B: %s\n", BUTTON_B_STATE ? "Pressed" : "Released");
    fprintf(stderr, "X: %s\n", BUTTON_X_STATE ? "Pressed" : "Released");
    fprintf(stderr, "Y: %s\n\n", BUTTON_Y_STATE ? "Pressed" : "Released");

    fprintf(stderr, "Raw[3 low] = 0x%x\n\n", buttons_byte & 0x0F); // Low nibble for L1/R1/Home
    fprintf(stderr, "L1: %s\n", BUTTON_L1_STATE ? "Pressed" : "Released");
    fprintf(stderr, "R1: %s\n", BUTTON_R1_STATE ? "Pressed" : "Released");
    fprintf(stderr, "Home: %s\n\n", BUTTON_HOME_STATE ? "Pressed" : "Released");

    // Analog Triggers
    fprintf(stderr, "Analog Triggers:\n");
    fprintf(stderr, "Raw[4] = 0x%02x\n", LEFT_TRIGGER_ANALOG_VALUE);
    fprintf(stderr, "Raw[5] = 0x%02x\n\n", RIGHT_TRIGGER_ANALOG_VALUE);

    // Helper function for drawing trigger bar
    char trigger_bar[21]; // 20 chars + null terminator
    int bar_length;

    // Left Trigger Bar
    memset(trigger_bar, '-', sizeof(trigger_bar) - 1);
    bar_length = (int)(LEFT_TRIGGER_ANALOG_VALUE / 255.0 * 20);
    for (int i = 0; i < bar_length; ++i) {
        trigger_bar[i] = '#';
    }
    trigger_bar[20] = '\0';
    fprintf(stderr, "Left Trigger:    [%s]\n", trigger_bar);

    // Right Trigger Bar
    memset(trigger_bar, '-', sizeof(trigger_bar) - 1);
    bar_length = (int)(RIGHT_TRIGGER_ANALOG_VALUE / 255.0 * 20);
    for (int i = 0; i < bar_length; ++i) {
        trigger_bar[i] = '#';
    }
    trigger_bar[20] = '\0';
    fprintf(stderr, "Right Trigger:   [%s]\n\n", trigger_bar);


    // Left Stick
    fprintf(stderr, "Left Stick:\n");
    fprintf(stderr, "Raw[6-7] = 0x%04x\n", (unsigned short)LEFT_X);
    fprintf(stderr, "Raw[8-9] = 0x%04x\n\n", (unsigned short)LEFT_Y);
    fprintf(stderr, "X: %d\n", LEFT_X);
    fprintf(stderr, "Y: %d\n\n", LEFT_Y);

    // Right Stick
    fprintf(stderr, "Right Stick:\n");
    fprintf(stderr, "Raw[10-11] = 0x%04x\n", (unsigned short)RIGHT_X);
    fprintf(stderr, "Raw[12-13] = 0x%04x\n\n", (unsigned short)RIGHT_Y); // Corrected byte range
    fprintf(stderr, "X: %d\n", RIGHT_X);
    fprintf(stderr, "Y: %d\n\n", RIGHT_Y);

    fprintf(stderr, "---------------------------------------------------------\n"); // Adjusted separator

}


int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    libusb_context *context = NULL;
    libusb_device_handle *handle = NULL;
    int fd = -1;
    int r = 0;
    unsigned char data[32]; // Max packet size is 32 bytes from descriptor dump
    int actual_length;
    int interface_number = 0; // Interface 0 based on descriptor dump
    int endpoint_address = 0x81; // Interrupt IN endpoint 0x81 based on descriptor dump
    int max_packet_size = 32;

    if (argc < 2 || sscanf(argv[1], "%d", &fd) != 1) {
        fprintf(stderr, "Usage: %s <file_descriptor>\n", argv[0]);
        return 1;
    }

    libusb_set_option(NULL, LIBUSB_OPTION_NO_DEVICE_DISCOVERY);
    r = libusb_init(&context);
    if (r < 0) {
        fprintf(stderr, "libusb_init failed: %s\n", libusb_error_name(r));
        return 1;
    }
    fprintf(stderr, "DEBUG: libusb_init successful.\n");

    r = libusb_wrap_sys_device(context, (intptr_t)fd, &handle);
    if (r < 0) {
        fprintf(stderr, "libusb_wrap_sys_device failed: %s\n", libusb_error_name(r));
        libusb_exit(context);
        return 1;
    }
    fprintf(stderr, "DEBUG: libusb_wrap_sys_device successful. Handle obtained.\n");

    // Try to detach kernel driver if one is active for Interface 0
    int kernel_driver_active = 0; 
    fprintf(stderr, "DEBUG: Checking for active kernel driver on interface %d.\n", interface_number);
    if (libusb_kernel_driver_active(handle, interface_number) == 1) {
        fprintf(stderr, "DEBUG: Kernel driver active. Attempting to detach.\n");
        r = libusb_detach_kernel_driver(handle, interface_number);
        if (r < 0) {
            fprintf(stderr, "libusb_detach_kernel_driver failed (interface %d): %s\n", interface_number, libusb_error_name(r));
            goto error_exit_with_handle;
        }
        kernel_driver_active = 1;
        fprintf(stderr, "DEBUG: Kernel driver detached.\n");
    } else {
        fprintf(stderr, "DEBUG: No active kernel driver found or already detached.\n");
    }

    fprintf(stderr, "DEBUG: Attempting to claim interface %d.\n", interface_number);
    r = libusb_claim_interface(handle, interface_number);
    if (r < 0) {
        fprintf(stderr, "libusb_claim_interface failed (interface %d): %s\n", interface_number, libusb_error_name(r));
        if (kernel_driver_active) {
            fprintf(stderr, "DEBUG: Reattaching kernel driver.\n");
            libusb_attach_kernel_driver(handle, interface_number); // Reattach if we detached it
        }
        goto error_exit_with_handle;
    }
    fprintf(stderr, "DEBUG: Interface %d claimed successfully.\n", interface_number);

    fprintf(stderr, "Reading raw HID input from device (Press Ctrl+C to stop):\n");
    fprintf(stderr, "Endpoint Address: 0x%02x, Interface: %d, Max Packet Size: %d\n", endpoint_address, interface_number, max_packet_size);
    // Removed 2-second time limit for continuous polling
    fprintf(stderr, "DEBUG: Entering continuous polling loop.\n");

    while (1) { // Continuous polling
        r = libusb_interrupt_transfer(handle, endpoint_address, data, max_packet_size, &actual_length, 100); // Shorter timeout for 10Hz
        if (r == LIBUSB_ERROR_TIMEOUT) {
            // No need to print dots, just continue polling without new output if no data
            continue; 
        } else if (r < 0) {
            if (r == LIBUSB_ERROR_NO_DEVICE) {
                fprintf(stderr, "\nERROR: Device disconnected. Exiting.\n");
                break; // Exit the loop
            } else if (r == LIBUSB_ERROR_PIPE) {
                 fprintf(stderr, "libusb_interrupt_transfer error: LIBUSB_ERROR_PIPE (endpoint halted). Retrying...\n");
                 libusb_clear_halt(handle, endpoint_address);
                 usleep(100000); // Wait 100ms before retrying
                 continue;
            }
            fprintf(stderr, "libusb_interrupt_transfer failed: %s\n", libusb_error_name(r));
            break;
        }

        if (actual_length > 0) {
            // Removed raw byte printing here to rely solely on interpret_gamepad_report
            interpret_gamepad_report(data, actual_length); // Call the interpretation function
        }
    }

    // Cleanup upon successful exit or break from loop
    libusb_release_interface(handle, interface_number);
    if (kernel_driver_active) {
        libusb_attach_kernel_driver(handle, interface_number); // Reattach if we detached it
    }
    
    libusb_close(handle);
    libusb_exit(context);
    return 0;

error_exit_with_handle:
    if (handle) { // Only close handle if it was successfully opened
        libusb_close(handle);
    }
    // Fall through to common context exit
// error_exit_no_config: // Label removed as it's not needed for this simplified code
    if (context) { // Only exit context if it was successfully initialized
        libusb_exit(context);
    }
    return 1;
}