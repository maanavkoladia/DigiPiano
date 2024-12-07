/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "../inc/main.h"
#include "../inc/LaunchPad.h"
#include "../inc/PLL.h"
#include "../inc/PlayNotes.h"
#include "../inc/SPI_Slave.h"
#include "../inc/Timer0A.h"
#include "../inc/UART.h"
#include "../inc/eDisk.h"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
int Samples_Preloaded = 0;

/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */
void Clock_Delay1ms(uint32_t n);
void startupDelay(void);
void HeartBeat(void);
void startupDelay(void);
void PortE_Init(void);
/* ================================================== */
/*                    MAIN FUNCTION                   */
/* ================================================== */

void diskError(char *errtype, unsigned long n) {
	PF2 = 0x04; // turn LED off to indicate error
	while (1) {
	};
}

void initSDCard() {
	BYTE buffer[512] = {0};

	// Read in metadata about waveforms
	eDisk_ReadBlock(buffer, 0);
	unsigned num_waveforms = *(unsigned *)buffer;
	int k;
	for (k = 0; k < num_waveforms; k++) {
		unsigned offset = 4 + k * 8;
		offsets[k] = *(unsigned *)(buffer + offset);
		table_sizes[k] = *(unsigned *)(buffer + offset + 4);
	}

	// Preload samples
	int i;
	for (i = 0; i < num_waveforms; i++) {
		eDisk_ReadBlock(buffer, offsets[i] / 512);
		int j;
		for (j = 0; j < 256; j++) {
			TablesArray[i][j] = *((unsigned short *)buffer + j);
		}
	}
	Samples_Preloaded = 1;
}

void PortF(void) {
	unsigned long volatile delay;
	SYSCTL_RCGCGPIO_R |= 0x20; // activate port F
	delay = SYSCTL_RCGCGPIO_R;
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTF_LOCK_R = 0x4C4F434B; // 2) unlock PortF PF0
	GPIO_PORTF_CR_R |= 0x1F;        // allow changes to PF4-0
	GPIO_PORTF_DIR_R =
	    0x0E; // make PF3-1 output (PF3-1 built-in LEDs),PF4,0 input
	GPIO_PORTF_PUR_R = 0x11;   // PF4,0 have pullup
	GPIO_PORTF_AFSEL_R = 0x00; // disable alt funct on PF4-0
	GPIO_PORTF_DEN_R = 0x1F;   // enable digital I/O on PF4-0
	GPIO_PORTF_PCTL_R = 0x00000000;
	GPIO_PORTF_AMSEL_R = 0; // disable analog functionality on PF
}

extern int RePress[];

// SSI0 FOR SD CARD, SSI2 FOR DAC, CHIP TO CHIP NOW USES SSI3  (port D)
int main(void) {
	startupDelay();
	DisableInterrupts();
	PLL_Init(Bus80MHz); // bus clock at 80 MHz
	// LaunchPad_Init();
	// UART_Init();

	PortF(); // LaunchPad switches and LED
	PortE_Init();

	initQueue(&notesQueue); // initialize the queue of on notes

	PlayNotes_Init();

	Timer0A_Init(HeartBeat, 80000000, 7);
	SPI_SlaveInit(); // initialize SSI0 for receive fifo not empty interrupt,
	                 // freescale mode 00 with 8 bit data
	EnableInterrupts();

	{ // close result scope
		int result = eDisk_Init(0);
		if (result)
			diskError("eDisk_Init", result);
	}
	initSDCard();

	// int test = 0;
	// putInQueue(0,1);

	while (1) {
		// WaitForInterrupt();
		// if (isEmpty(&notesQueue)) {
		// 	continue; // nothing to play so no buffers to update
		// }

		// Loop to fill remaining samples
		// int qIndex = 0, count = 0;
		int n;
		for (n = 0; n < MAX_NOTES; n++) {
			// qIndex = qIndex == MAX_QUEUE_SIZE ? 0 : qIndex;
			// int noteIndex = notesQueue.data[qIndex].number;
			int noteIndex = n;
			FillPosition *fillPos = &bufferlocation[noteIndex];
			if (*fillPos == FILL_NONE) {
				continue;
			}

			// Keep in mind each SD card read takes 1ms
			BYTE *fillBuffer =
			    (BYTE *)(TablesArray[noteIndex]) + (*fillPos == FILL_TOP ? 512 : 0);
			*fillPos = FILL_NONE;
			unsigned curSector = (offsets[noteIndex] + 2 * cursors[noteIndex]) / 512;
			eDisk_ReadBlock(fillBuffer, curSector + 1);
			// if (RePress[noteIndex]) {
			// 	RePress[noteIndex] = 0;
			// }
		}
	}
}

/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */
void HeartBeat(void) { GPIO_PORTF_DATA_R ^= 0x02; }

void Clock_Delay(uint32_t ulCount) {
	while (ulCount) {
		ulCount--;
	}
}

void Clock_Delay1ms(uint32_t n) {
	while (n) {
		Clock_Delay(
		    23746); // 1 msec, tuned at 80 MHz, originally part of LCD module
		n--;
	}
}

void startupDelay(void){
	uint16_t i ;
  for(i= 0; i < UINT16_MAX; i++){}
}

void PortE_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;//turn on porte
	while((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R4) == 0){}

	//make pe0-3 gigital outs
	// GPIO_PORTE_AMSEL_R &= ~0x0F;
	// GPIO_PORTE_AFSEL_R &= ~0x0F;
	// GPIO_PORTE_PCTL_R &= ~0x0000FFFF;
	GPIO_PORTE_DEN_R |= 0x01;
	GPIO_PORTE_DIR_R &= ~0x01;//make them ins
}
