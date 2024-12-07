#ifndef MAIN_H
#define MAIN_H

/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <stdio.h>
#include <stdlib.h>

/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */


/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */
void DisableInterrupts(void);           // Disable interrupts
void EnableInterrupts(void);            // Enable interrupts
void WaitForInterrupt(void);

#endif 

