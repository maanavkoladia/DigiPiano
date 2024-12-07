#ifndef PLAYNOTES_H
#define PLAYNOTES_H

/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "stdint.h"
#include "SPI_Slave.h"
/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */
typedef enum FillPosition {
	FILL_NONE = 0,
	FILL_BOTTOM,
	FILL_TOP
} FillPosition;

#define MAX_NOTES 24
#define MAX_FLIGHT_TIME 512
#define FLIGHT_TIME_BITS 9
#define SAMPLE_BUFFER_START 0
#define SAMPLE_BUFFER_LEN 512
#define SAMPLE_BUFFER_END SAMPLE_BUFFER_START + SAMPLE_BUFFER_LEN

/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */
void DAC_Out(uint16_t data);
void DAC_Init();
void BT_Init();
void BT_Out(uint8_t data);
void PlayNotes_Init(void);
void PlayNotes();

extern int Samples_Preloaded;
extern int offsets[MAX_NOTES];// do in init
extern int cursors[MAX_NOTES];// keep track of number of samples that have been played
extern FillPosition bufferlocation[MAX_NOTES];//0=fill at bottom, 1= top
extern uint16_t TablesArray[MAX_NOTES][SAMPLE_BUFFER_LEN];
extern int table_sizes[MAX_NOTES]; // length of each waveform
extern KeyStatus statuses[MAX_NOTES];
extern uint16_t flight_times[MAX_NOTES]; // how long a note was in flight for
extern KeyStatus prevStatuses[MAX_NOTES];
extern unsigned releaseTimes[MAX_NOTES];


#endif
