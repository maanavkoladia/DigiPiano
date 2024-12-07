/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */

#include "../inc/PlayNotes.h"
#include "../inc/SPI_Slave.h"
#include "../inc/Timer1A.h"
#include "../inc/tm4c123gh6pm.h"
#include "stdint.h"
#include <stdbool.h>
int SamplingPeriod = 7256; // reload value for timer when using 11.025k -> 7256
// int end_ringing_target= (80000000*4)/7256;// how mant timer ticks to ring for
// (would probably be the same for all notes)

// These are all 12 bit values
// Table for 261.63 frequency note:

bool DAC_ON = 1;
bool BT_ON = 0;
int table_sizes[MAX_NOTES] = {0}; // size of waveform on SD card. read in init
int offsets[MAX_NOTES] = {0};     // offset of waveform on SD Card. read in init
int cursors[MAX_NOTES] = {0};     // progression of playing samples
int RePress[24] = {0};            // Which notes are repressed
unsigned releaseTimes[MAX_NOTES] = {0};
FillPosition bufferlocation[MAX_NOTES] = {0}; // 0=fill at bottom, 1= top
uint16_t indexers[MAX_NOTES] = {0};
uint16_t TablesArray[MAX_NOTES][512] = {0};
/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
// #define SINEWAVE_TABLE_SIZE 64
// uint8_t sinewave_idx = 0;
// const uint16_t SineWave[64] = {
//     1024, 1122, 1219, 1314, 1407, 1495, 1580, 1658, 1731, 1797, 1855,
//     1906, 1948, 1981, 2005, 2019, 2024, 2019, 2005, 1981, 1948, 1906,
//     1855, 1797, 1731, 1658, 1580, 1495, 1407, 1314, 1219, 1122, 1024,
//     926,  829,  734,  641,  553,  468,  390,  317,  251,  193,  142,
//     100,  67,   43,   29,   24,   29,   43,   67,   100,  142,  193,
//     251,  317,  390,  468,  553,  641,  734,  829,  926};
//
uint16_t velocities[512] = {
    512, 487, 463, 440, 419, 398, 379, 360, 343, 326, 310, 295, 280, 267, 254,
    241, 230, 218, 208, 198, 188, 179, 170, 162, 154, 146, 139, 132, 126, 120,
    114, 108, 103, 98,  93,  88,  84,  80,  76,  72,  69,  65,  62,  59,  56,
    53,  51,  48,  46,  44,  42,  39,  38,  36,  34,  32,  31,  29,  28,  26,
    25,  24,  23,  21,  20,  19,  18,  17,  17,  16,  15,  14,  13,  13,  12,
    12,  11,  10,  10,  9,   9,   8,   8,   8,   7,   7,   6,   6,   6,   5,
    5,   5,   5,   4,   4,   4,   4,   4,   3,   3,   3,   3,   3,   2,   2,
    2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,
    1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,
};

/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */

/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */

void MyUART_OutChar(char data) {

	while ((UART1_FR_R & 0x0020) != 0)
		; // wait until TXFF is 0

	UART1_DR_R = data;
}

int putagain1 = 1;
int putagain2 = 1;
int putagain3 = 1;

uint32_t amp = 0;
int iterations = 0;
// int number_of_on_notes = 0;

int8_t shift[]={0, 3 , 3, 3, 3, 3, 3,3, 3 };

void PlayNotes() {
	if (!Samples_Preloaded)
		return;
	if (BT_ON) {
		int32_t signed_amp = amp - 32768;
		signed_amp >>= 3;
		MyUART_OutChar((char)(signed_amp & 0x00FF));        // send lower byte first
		MyUART_OutChar((char)((signed_amp & 0xFF00) >> 8)); // //send upper byte
	} else if (DAC_ON) {
		amp = (amp >> 4) + 32768; // convert to unsigned
		DAC_Out(amp);
	}
	amp = 32768; // middle = 0x7fffffff

	static const int attackTime = 32;  // 2^5
	static const int releaseTime = 4096; // 2^9
	int id;
	char counter=0;
	// number_of_on_notes=0;
	for ( id = 0; (id < MAX_NOTES); id++) {
		if (statuses[id] == KEY_IDLE) counter++;
	}
	if(counter==24)return;

	for ( id = 0; (id < MAX_NOTES); id++) {
		KeyStatus status = statuses[id];
		switch (status) {
		case KEY_IDLE:
			continue;
		case KEY_PLAYING: case KEY_RELEASING:
			break;
		case KEY_PRESSED:
			releaseTimes[id] = table_sizes[id] - releaseTime;
			cursors[id] = 0;
			indexers[id] = 0;
			statuses[id] = KEY_PLAYING;
			break;
		case KEY_RELEASED:
			releaseTimes[id] = cursors[id];
			if (releaseTimes[id] + releaseTime > table_sizes[id]) {
				releaseTimes[id] = table_sizes[id] - releaseTime;
			}
			statuses[id] = KEY_RELEASING;
			break;
		default:
			break; // Unreachable
		}

		uint16_t *samples = TablesArray[id];
		int32_t note_amp = samples[indexers[id]] - 32768; // convert to signed

		// Edit for velocity
		note_amp = (note_amp * velocities[flight_times[id]]) >> FLIGHT_TIME_BITS;

		// Release logic
		if (cursors[id] > releaseTimes[id]) {
			if (cursors[id] > releaseTimes[id] + releaseTime) { // End the note
				statuses[id] = KEY_IDLE;
				cursors[id] = -1; // is this some fuckery black magic??
				bufferlocation[id] = FILL_BOTTOM;
				continue;
			} else { // Ramp down for release
				unsigned timeRemaining = releaseTime - (cursors[id] - releaseTimes[id]);
				note_amp = (note_amp * timeRemaining) >> 12; // releaseTime = 2^12
			}
		}

		// Ramp up for attack
		if (cursors[id] < attackTime) {
			// number_of_on_notes++;
			note_amp = (note_amp * cursors[id]) >> 5;
		}
		// number_of_on_notes++;

		// Calc velocity sacling based on ms
		amp += note_amp;
		// amp += note_amp;

		// Decide whether loads should occur
		static const int topTrigger = SAMPLE_BUFFER_START;
		static const int botTrigger = (SAMPLE_BUFFER_START + SAMPLE_BUFFER_LEN) / 2;
		if (indexers[id] == topTrigger) {
			bufferlocation[id] = FILL_TOP;
		} else if (indexers[id] == botTrigger) {
			bufferlocation[id] = FILL_BOTTOM;
		}

		cursors[id]++;
		indexers[id]++;
		if (indexers[id] >= SAMPLE_BUFFER_END) {
  		 indexers[id] = 0;
		}
	}
}

/* DAC pins:
SSI2CLK	PB4
SSI2Fss	PB5
L_DAC	PB6
SSI2Tx	PB7

..............................................................................
*/
void DAC_Init(void) {                    // using SSI2
	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R2; // Enable SSI2
	while ((SYSCTL_PRSSI_R & SYSCTL_RCGCSSI_R2) == 0) {
	} // Wait until SSI2 ready

	// Enable clocks for Port B
	SYSCTL_RCGCGPIO_R |= 0x02; // Enable clocks for Port B
	while ((SYSCTL_PRGPIO_R & 0x02) == 0) {
	} // Wait until Port B ready

	// ---- Configure Port B for SSI2 (PB4-PB7) ----
	GPIO_PORTB_AFSEL_R |= 0xB0;  // Enable alt funct on PB4-PB7
	GPIO_PORTB_AFSEL_R &= ~0x40; // ldac
	GPIO_PORTB_DEN_R |= 0xF0;    // Enable digital I/O on PB4-PB7
	GPIO_PORTB_PCTL_R =
	    (GPIO_PORTB_PCTL_R & 0x0000FFFF) + 0x20220000; // Configure PB4-PB7 as SSI
	GPIO_PORTB_AMSEL_R &= ~0xF0; // Disable analog functionality on PB4-PB7

	// ------------------- Configure SSI2 ---------------------
	SSI2_CR1_R = 0x00000000; // Disable SSI to configure
	SSI2_CPSR_R = 0x04;      // Set CPSR = 4 (prescale divisor)
	SSI2_CR0_R = 0;          // Clear the CR0 register
	SSI2_CR0_R |= (12 << 8); // Set SCR = 0 (serial clock rate)
	SSI2_CR0_R |= (1 << 7);  // Set SPH = 0 (capture data on the first clock edge)
	SSI2_CR0_R |= (1 << 6);  // Set SPO = 0 (clock is idle low)
	SSI2_CR0_R |= (0 << 4);  // Set FRF = 00 (Freescale SPI format)
	SSI2_CR0_R |= 0x0F;      // Set DSS = 16 bits (0x07 = 7 for 8-bit data)
	SSI2_CR1_R |= 0x00000002; // Enable SSI

	GPIO_PORTB_DATA_R |= 0x40;
}

void DAC_Out(uint16_t data) {
	volatile uint8_t i;
	while ((SSI2_SR_R & 0x00000002) == 0); // SSI Transmit FIFO Not Full
	SSI2_DR_R = data;
	while (SSI2_SR_R & SSI_SR_BSY);
	GPIO_PORTB_DATA_R &= ~0x40;
	i++;
	i++;
	i++;
	GPIO_PORTB_DATA_R |= 0x40;
}

void MyUART_Init(void) { // should be called only once

	SYSCTL_RCGCUART_R |= 0x00000002; // activate UART1

	SYSCTL_RCGCGPIO_R |= 0x00000004; // activate port C

	UART1_CTL_R &= ~0x00000001; // disable UART

	UART1_IBRD_R = 1; // IBRD = int(80,000,000/(16*115,200)) = int(43.40278)

	UART1_FBRD_R = 0; // FBRD = round(0.40278 * 64) = 26

	UART1_LCRH_R = 0x00000070; // 8 bit, no parity bits, one stop, FIFOs

	UART1_CTL_R |= 0x00000001; // enable UART

	GPIO_PORTC_AFSEL_R |= 0x30; // enable alt funct on PC5-4

	GPIO_PORTC_DEN_R |= 0x30; // configure PC5-4 as UART1

	GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & 0xFF00FFFF) + 0x00220000;

	GPIO_PORTC_AMSEL_R &= ~0x30; // disable analog on PC5-4
}

// void PeriodicTask_SineWave(void) {
// 	DAC_Out(SineWave[sinewave_idx] << 4);
// 	sinewave_idx++;
// 	sinewave_idx %= SINEWAVE_TABLE_SIZE;
// }

void PlayNotes_Init(void) {
  if((GPIO_PORTE_DATA_R & 0x01)){//if low, aux, else bt
    DAC_ON = 0;
	 	BT_ON = 1;
	}else{
  	DAC_ON = 1;
  	BT_ON = 0;
	}
	DAC_Init();
	MyUART_Init();
	Timer1A_Init(PlayNotes, SamplingPeriod, 3); // outputs to DAC or ESP
}
