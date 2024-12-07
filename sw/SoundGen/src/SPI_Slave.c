#include "../inc/SPI_Slave.h"
#include "../inc/PlayNotes.h"
#include "../inc/UART.h"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

KeyStatus statuses[MAX_NOTES] = {0};
KeyStatus prevStatuses[MAX_NOTES] = {0};
uint16_t flight_times[MAX_NOTES] = {0};
// A Queue of currently on notes:
struct Queue notesQueue;

// Function to initialize the queue
void initQueue(struct Queue *q) {
	q->front = 0;
	q->rear = -1;
	q->size = 0;
}

// Function to check if the queue is full
int isFull(struct Queue *q) { return q->size == MAX_QUEUE_SIZE; }

// Function to check if the queue is empty
int isEmpty(struct Queue *q) { return q->size == 0; }

// Function to push a note into the queue
int push(struct Queue *q, struct note note) {
	if (isFull(q)) {
		return 0;
	}
	// Move rear pointer to the next position and reset if it exceeds the array
	// bounds
	q->rear++;
	if (q->rear >= MAX_QUEUE_SIZE) {
		q->rear = 0;
	}
	q->data[q->rear] = note;
	q->size++;
	return 1;
}

struct note *get(struct Queue *q, int index) {
	if (index > q->size) {
		return 0;
	}
	int pos = q->front + index;
	if (pos > MAX_QUEUE_SIZE) {
		pos -= MAX_QUEUE_SIZE;
	}
	return &q->data[pos];
}

// Function to pop a note from the queue
struct note pop(struct Queue *q) {
	struct note emptyNote = {0}; // Placeholder for an empty note
	if (isEmpty(q)) {
		return emptyNote;
	}
	struct note note = q->data[q->front];
	// Increment front and reset to 0 if it reaches MAX_QUEUE_SIZE
	q->front++;
	if (q->front == MAX_QUEUE_SIZE) {
		q->front = 0;
	}
	q->size--;
	return note;
}

// Function to get the current size of the queue
int getCurrentSize(struct Queue *q) { return q->size; }

// 		PD0 = SSI3Clk (Clock)
// PD1 = SSI3Fss (Chip Select/Frame Select)
// PD2 = SSI3Tx  (MOSI, Master Out Slave In)
// PD3 = SSI3Rx  (MISO, Master In Slave Out)
void SPI_SlaveInit() { // uses SSI3 (port D to receive chip to chip!!!)

	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R3; // Enable SSI3
	SYSCTL_RCGCGPIO_R |= 0x08;             // activate port D
	while ((SYSCTL_PRGPIO_R & 0x08) == 0) {
	}; // wait for port D to be ready

	GPIO_PORTD_AFSEL_R |= 0x0F; // enable alt function on PD0,1,2,3
	GPIO_PORTD_DEN_R |= 0x0F;   // enable digital on PD0,1,2,3
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFFFF0000) +
	                    0x00001111; // configure PD0,1,2,3 for SSI
	GPIO_PORTD_AMSEL_R &= ~0x0F;    // disable analog on pins

	SSI3_CR1_R = 0;
	SSI3_CR1_R |=
	    0x00000004; // enable SSI in slave mode (bit 2 set to 1 for slave mode)

	SSI3_CR0_R &= ~(0x0000FFF0); // clear settings for SCR, SPH, SPO
	SSI3_CR0_R |= 0x0F; // DSS = 16-bit data, SPH = 0, SPO = 0 (Freescale format)

	SSI3_IM_R |= 0x04;        // enable receive FIFO interrupt
	SSI3_CR1_R |= 0x00000002; // enable SSI

	while (SSI3_SR_R & SSI_SR_RNE) {
		int nothing = SSI3_DR_R;
	}
	NVIC_PRI14_R =
	    (NVIC_PRI14_R & 0xFF00FFFF) | (1 << 21); // Set priority of SSI0 interrupt
	NVIC_EN1_R |= (1 << 26); // Enable SSI0 interrupt in NVIC 58-32
}

extern int RePress[];

int putInQueue(int nr, int vel) {
	// traverse queue to search for existing notes
	int index = notesQueue.front;
	int count = 0;
	while (count < notesQueue.size) { // traverse Queue
		index++;
		if (index == MAX_QUEUE_SIZE) {
			index = 0; // Wrap around to the beginning if necessary
		}
		if (nr == notesQueue.data[index].number) {
			notesQueue.data[index].press = 1;
			RePress[nr] = 1;
			return 1; // was in queue already so just change the press field from here
		}
		count++;
	}

	struct note newNote;
	RePress[nr] = 0;
	newNote.number = nr;
	newNote.velocity = vel;
	newNote.press = 1; // typical
	push(&notesQueue, newNote);
	return 0; // not found in queue so should add it in as new
}

uint16_t receivedData;
void SSI3_Handler(void) { // triggered whenver receive fifo is not empty
	/*
	 * - Bits [7:6]: Velocity (2 bits) - Represents the speed of the key press:
	 *   - 00 = Low velocity
	 *   - 01 = Medium velocity
	 *   - 10 = High velocity
	 *   - 11 = Maximum velocity
	 * - Bit [5]: Press/Release (1 bit) - Indicates whether the note is a press or
	 * release:
	 *   - 1 = Press
	 *   - 0 = Release
	 * - Bits [4:0]: Note (5 bits) - Represents the note number, ranging from 0 to
	 * 23 for the 24-key piano.
	 */

	while (SSI3_SR_R & SSI_SR_BSY) {
	}
	receivedData = SSI3_DR_R; // read data from RX FIFO
	while (SSI3_SR_R & SSI_SR_BSY) {
	}
	receivedData = SSI3_DR_R; // read data from RX FIFO
	while (SSI3_SR_R & SSI_SR_BSY) {
	}
	receivedData = SSI3_DR_R; // read data from RX FIFO
	while (SSI3_SR_R & SSI_SR_BSY) {
	}
	receivedData = SSI3_DR_R; // read data from RX FIFO
	SSI3_ICR_R = 0x01; // clear the receive FIFO time-out interrupt (RTIC bit)
	uint32_t id = receivedData & 0x1F;
	if (id >= 24)
		return;
	id = id >= 16 ? id - 16 : id + 8; // Fix out of order IDs
	uint32_t status = (receivedData >> 5) & 0x01;
	uint32_t note_time = (receivedData >> 6) & 0x03FF; // 9-bits of velocity

	if (status == 1) { // Press
  	// UART_OutString("Released: ");
    // GPIO_PORTF_DATA_R ^= 0x02;
		statuses[id] = KEY_PRESSED;
		// if (note_time > 512) note_time = 512;
		flight_times[id] = note_time;
	} else { // Release
		if (statuses[id] != KEY_IDLE && statuses[id] != KEY_RELEASING) {
  		statuses[id] = KEY_RELEASED;
		}
	 	// UART_OutString("Released: ");
	 	// GPIO_PORTF_DATA_R ^= 0x04;;
	}
}
// UART_OutChar('\n');
// note only gets sent over on a transition from unpressed-> pressed , or
// pressed->release phase,  or release phase-> re-pressed on unpressed->
// pressed we add the note to the playing queue with press field = 1 . If
// press is 1 that means play from start. so after the first sample gets sent
// out turn the press field to 0. on pressed -> release phase we dont have to
// do anything since thats the default subsequent behavior , so the note will
// play out on release phase -> re-pressed we have to modify the
// already-present corresponding note in the queue to have a press=1 which
// requires a queue traversal
