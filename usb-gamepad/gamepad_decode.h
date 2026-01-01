#ifndef GAMEPAD_DECODE_H
#define GAMEPAD_DECODE_H

/*
 * Gamepad State Packet Specification
 *
 * Packet size: 20 bytes
 * Byte order: LITTLE ENDIAN
 *
 * Raw example:
 * 00 14 00 00 00 00 59 00 A3 01 00 00 00 00 00 00 00 00 00 00
 */

#include <stdint.h>

/* =========================================================
 * Byte 2 – D-Pad + System Buttons
 * =========================================================
 *
 * LOW nibble (bits 0–3): D-Pad
 * Bit 0 (0x01): Up
 * Bit 1 (0x02): Down
 * Bit 2 (0x04): Left
 * Bit 3 (0x08): Right
 *
 * HIGH nibble (bits 4–7): System / Stick Buttons
 * Bit 4 (0x10): L3
 * Bit 5 (0x20): R3
 * Bit 6 (0x40): Back
 * Bit 7 (0x80): Start
 */

#define DPAD_UP     (1 << 0)
#define DPAD_DOWN   (1 << 1)
#define DPAD_RIGHT  (1 << 2) 
#define DPAD_LEFT   (1 << 3) 

#define BTN_START   (1 << 4)
#define BTN_BACK    (1 << 5) 
#define BTN_L3      (1 << 6) 
#define BTN_R3      (1 << 7)

/* =========================================================
 * Byte 3 – Action / Shoulder Buttons
 * =========================================================
 *
 * LOW nibble (bits 0–3):
 * Bit 0: L1
 * Bit 1: R1
 * Bit 2: Home (Xbox / Home button)
 * Bit 3: (Unused?)
 *
 * HIGH nibble (bits 4–7):
 * Bit 4: A
 * Bit 5: B
 * Bit 6: X
 * Bit 7: Y
 */

#define BTN_L1      (1 << 0)
#define BTN_R1      (1 << 1)
#define BTN_HOME    (1 << 2) // Xbox / Home button

#define BTN_A       (1 << 4)
#define BTN_B       (1 << 5)
#define BTN_X       (1 << 6)
#define BTN_Y       (1 << 7)

/* =========================================================
 * Controller Report Structure
 * =========================================================
 * Total size: exactly 20 bytes
 */

#pragma pack(push, 1)
typedef struct
{
    uint8_t  report_id;      /* Byte 0: always 0x00 */
    uint8_t  length;         /* Byte 1: always 0x14 (20 bytes) */

    uint8_t  dpad_system;    /* Byte 2: DPAD (low nibble, with Left/Right swapped) + System (high nibble, with Start/Back/L3/R3 reordered) */
    uint8_t  buttons;        /* Byte 3: L1/R1/Home (low nibble) + ABXY (high nibble) */

    uint8_t  trigger_left;   /* Byte 4: analog 0–255 */
    uint8_t  trigger_right;  /* Byte 5: analog 0–255 */

    int16_t  left_x;         /* Byte 6–7 */
    int16_t  left_y;         /* Byte 8–9 */

    int16_t  right_x;        /* Byte 10–11 */
    int16_t  right_y;        /* Byte 12–13 */

    uint8_t  reserved[6];    /* Byte 14–19: unused */
} GamepadReport;
#pragma pack(pop)

/* =========================================================
 * Notes
 * =========================================================
 *
 * - Raw[2] LOW  = D-Pad bits (1,2,4,8)
 * - Raw[2] HIGH = L3, R3, Back, Start
 * - Raw[3] LOW  = L1, R1, L2, R2 (digital)
 * - Raw[3] HIGH = A, B, X, Y
 * - Triggers are analog (bytes 4 & 5)
 * - Stick range: -32768 .. +32767
 * - Structure size MUST be exactly 20 bytes
 */
#endif // GAMEPAD_DECODE_H
