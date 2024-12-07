/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "../inc/main.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../inc/SensorIn.h"
#include "../inc/Communication.h"
#include "../inc/Notes.h"
#include "../inc/Timer0A.h"
#include "../inc/Timer5A.h"
#include "../inc/LaunchPad.h"
#include "../inc/UART.h"

/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
bool samplesTaken = false;
/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */


void Clock_Delay1ms(uint32_t n);

void HeartBeat(void);
void PortE_Init(void);

void intToStr(int num, char str[12]);
char int_str_buf[12];

char keyNames[24][5] = {
  "G#3", "A3", "A#3", "B3", "C4", "C#4", "D4", "D#4", // SSI 0 | ADC3
  "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",  // SSI 2 | ADC1 
  "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3"   // SSI 3 | ADC2
};
/* ================================================== */
/*                    MAIN FUNCTION                   */
/* ================================================== */
int main(void){
	startupDelay();
	DisableInterrupts();
	PLL_Init(Bus80MHz);    // bus clock at 80 MHz
  //LaunchPad_Init();
	PortE_Init();
	SensorsInit();
	UART_Init();
	Chip_to_Chip_Init();
  //TestSendingMessages_Init();
	Notes_Init();
	//Timer5A_Init(HeartBeat, 80000000, 7);
	EnableInterrupts();
	//SendTestMessage();
	//GPIO_PORTE_DATA_R |= 0x02;
	while(1){
		// if(samplesTaken){
		// 	DisableInterrupts();
		// 	for(int i = 0; i < NUMBER_OF_SENSORS; i++){
    //     UART_OutString(keyNames[i]);
    //     UART_OutString(": ");
		// 		intToStr(ADC_Vals_Buf[i], int_str_buf);
		// 		UART_OutString(int_str_buf);
		// 	}
			//GPIO_PORTE_DATA_R |= 0x2;
			//while(1){}
		//}
		Notes_State_Update();
	}
}


/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */
void HeartBeat(void){
	GPIO_PORTE_DATA_R ^= 0x20;
 }
void startupDelay(void){
  for(uint16_t i = 0; i < UINT16_MAX; i++){}
}

void Clock_Delay(uint32_t ulCount){
  while(ulCount){
    ulCount--;
  }
}

void Clock_Delay1ms(uint32_t n){
  while(n){
    Clock_Delay(23746);  // 1 msec, tuned at 80 MHz, originally part of LCD module
    n--;
  }
}

void PortE_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;//turn on porte
	while((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R4) == 0){}

	//make pe0-4 gigital outs
	GPIO_PORTE_AMSEL_R &= ~0xFF;
	GPIO_PORTE_AFSEL_R &= ~0xFF;
	GPIO_PORTE_PCTL_R &= ~0xFFFFFFFF;
	GPIO_PORTE_DEN_R |= 0xFF;
	GPIO_PORTE_DIR_R |= 0xFF;

	GPIO_PORTE_DATA_R |= 0x04;//pe2 on
    GPIO_PORTE_DATA_R |= 0x08;//pe3 on
    GPIO_PORTE_DATA_R |= 0x10;//pe4 on
}

void intToStr(int num, char str[12]){
    int i = 0;
    int isNegative = 0;

    // Handle zero case
    if (num == 0) {
        if (i < 11) str[i++] = '0';
        if (i < 11) str[i++] = '\n'; // Add newline character
        str[i] = '\0'; // Null-terminate the string
        return;
    }

    // Handle negative numbers
    if (num < 0) {
        isNegative = 1;
        // Handle INT_MIN case (-2147483648)
        if (num == -2147483648) {
            num = 2147483647; // Will adjust after loop
        } else {
            num = -num;
        }
    }

    // Process individual digits
    while (num != 0 && i < 10) { // Leave space for '-', '\n', and '\0'
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    // Adjust for INT_MIN overflow
    if (isNegative && num == 2147483647) {
        if (i < 10) str[0] = '8'; // The last digit of INT_MIN
    }

    // Add minus sign if number is negative
    if (isNegative && i < 11)
        str[i++] = '-';

    // Add newline character if there's space
    if (i < 11)
        str[i++] = '\n';

    str[i] = '\0'; // Append string terminator

    // Reverse the string (excluding '\n' and '\0')
    int start = 0;
    int end = i - 2; // Exclude '\n' and '\0' from reversing
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}
