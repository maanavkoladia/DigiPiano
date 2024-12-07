#ifndef COMMUNICATION_H
#define COMMUNICATION_H
/**
 * SPI Communication Protocol for Note Presses
 * -------------------------------------------
 * This protocol defines an 8-bit message format for transmitting note press information
 * between two TM4C microcontrollers via SPI. The message includes details about the
 * note number, press/release status, and velocity of the key press.
 *
 * Bit structure (8 bits):
 * ----------------------------------
 * [7:6]  - Velocity (2 bits): Represents the velocity (speed) of the key press:
 *          00 - Low velocity
 *          01 - Medium velocity
 *          10 - High velocity
 *          11 - Maximum velocity
 * [5]    - Press/Release (1 bit): Indicates whether the message represents a press or release:
 *          1  - Key press
 *          0  - Key release
 * [4:0]  - Note (5 bits): Represents the note number, corresponding to the key that was pressed.
 *          Values range from 0 to 23, covering 24 keys (2 octaves).
 *
 * Summary:
 * --------
 * - The 8-bit message allows communication of the note pressed, the velocity of the press, 
 *   and whether it is a press or release.
 * - The message is structured to maximize efficiency, fitting all necessary data into a single byte.
 *
 * Example:
 * --------
 * A message of 0b10100101 (0xA5) represents:
 * - Velocity: 10 (High velocity)
 * - Press: 1 (Key press)
 * - Note: 00101 (Note 5)
 */
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "../inc/Notes.h"
#include "stdint.h"
#include "stdbool.h"
/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */


/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */
/**
 * @brief Pushes a key press event into the FIFO queue.
 * 
 * This function attempts to add a new key press event (represented as an 8-bit message) 
 * into the key press FIFO buffer. The message contains details about the note pressed, 
 * the velocity of the press, and whether it's a press or release event.
 * 
 * If the FIFO is full, the function turns on PE1 to signal the full state and returns false.
 * If successful, it stores the message in the buffer and returns true.
 * 
 * @param inMessage The 8-bit message encoding the note, velocity, and press/release status.
 * @return true if the message was successfully pushed into the FIFO.
 * @return false if the FIFO is full, and PE1 is turned on to indicate this condition.
 */
bool Push_keyPressFIFO(uint8_t inMessage);

/**
 * @brief Pops a key press event from the FIFO queue.
 * 
 * This function retrieves the oldest key press event (encoded as an 8-bit message) from 
 * the FIFO buffer. The message contains information about the note, press/release status, 
 * and velocity. 
 * 
 * If the FIFO is empty, it returns false. If successful, it stores the retrieved message 
 * in the provided pointer and returns true.
 * 
 * @param outMessage A pointer to an 8-bit variable where the retrieved message will be stored.
 *                   The message will contain the note, velocity, and press/release status.
 * @return true if the message was successfully popped from the FIFO.
 * @return false if the FIFO is empty and no data was retrieved.
 */
bool Pop_keyPressFIFO(uint8_t* outMessage);


/**
 * @brief inits spi on PortF for communcation with other tm4c123gh6pm chip
 */
void Chip_to_Chip_Init(void);

/**
 * @brief Constructs an 8-bit SPI message for note events.
 * 
 * This function constructs an 8-bit message containing the note information, press/release status,
 * and velocity of the key press, following the predefined protocol for communication between
 * two TM4C microcontrollers via SPI.
 * 
 * The message is structured as follows:
 * - Bits [15:6]: Velocity (10 bits) - Represents the speed of the key press:
 *   -0-300 ms
 * - Bit [5]: Press/Release (1 bit) - Indicates whether the note is a press or release:
 *   - 1 = Press
 *   - 0 = Release
 * - Bits [4:0]: Note (5 bits) - Represents the note number, ranging from 0 to 23 for the 24-key piano.
 * 
 * @param Note The note number (0-23) representing the key pressed or released.
 * @param isPress Boolean indicating if the event is a press (true) or release (false).
 * @param velocity The velocity of the key press (0-3) representing how fast the key is pressed.
 * @return uint8_t The 8-bit message encoding the note, press/release status, and velocity.
 * 
 * Example usage:
 * CreateMessage(5, true, 2) will return 0xA5 (binary 10100101), where:
 * - Velocity: 2 ms
 * - Press: 1 (Key press)
 * - Note: 5
 */

uint16_t CreateMessage(enum NotesIndexes note, bool isPress, uint32_t velocity);

void TestSendingMessages_Init(void);

void SendTestMessage(void);

void SendOutSPI(uint16_t messageOut);
#endif 

