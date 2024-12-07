#ifndef MAIN_H
#define MAIN_H

/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <stdio.h>
#include "stdbool.h"
#include <stdlib.h>

/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */
extern bool samplesTaken;
/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */
void DisableInterrupts(void);           // Disable interrupts
void EnableInterrupts(void);            // Enable interrupts
void WaitForInterrupt(void);
void startupDelay(void);
#endif 

