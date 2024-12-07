/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "../inc/tm4c123gh6pm.h"
#include "../inc/Notes.h"
#include "../inc/Timer5A.h"
#include "stdint.h"
#include "../inc/Communication.h"
#include "../inc/UART.h"

/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
#define TIME_ELAPSED_NOT_PRESS 512

Key KeyStates_list[NUMBER_OF_SENSORS];

uint16_t note_start_count[NUMBER_OF_SENSORS];
uint16_t note_release_count[NUMBER_OF_SENSORS];

typedef struct {
    uint8_t note_on;
    uint16_t key_position;
    uint8_t max_on;
    uint16_t pitch; //don't know if we use this might just use an index to tell
} KeyHammer;

KeyHammer Keys[NUMBER_OF_SENSORS];

#define INCREMENT_GLOBAL_COUNTER_PRIORITY 1
volatile uint32_t global_ms_count;

uint16_t idle_threshold[NUMBER_OF_SENSORS]={
	260,
	250,
	255,
	254,
	264,
	257,
	255,
	249,
	253,
	254,
	251,
	260,
	256,
	258,
	258,
	274,
	253,
	248,
	259,
	254,
	255,
	258,
	264,
	258,
};

uint16_t start_thresholds[NUMBER_OF_SENSORS] = {
	260 + 50,
	250 + 50,
	255 + 50,
	254 + 50,
	264 + 50,
	257 + 50,
	255 + 50,
	249 + 50,
	253 + 50,
	254 + 50,
	251 + 50,
	260 + 50,
	256 + 50,
	258 + 50,
	258 + 50,
	274 + 50,
	253 + 50,
	248 + 50,
	259 + 50,
	254 + 50,
	255 + 50,
	258 + 50,
	264 + 50,
	258 + 50,
};

uint16_t press_threshold[NUMBER_OF_SENSORS] = {
	587 - 25,
	620 - 25,
	631 - 25,
	556 - 25,
	640 - 25,
	635 - 25,
	595 - 25,
	609 - 25,
	613 - 25,
	571 - 25,
	606 - 25,
	631 - 25,
	662 - 25,
	625 - 25,
	616 - 25,
	694 - 25,
	639 - 25,
	654 - 25,
	684 - 25,
	636 - 25,
	647 - 25,
	624 - 25,
	627 - 25,
	622 - 25,
};

uint16_t release_threshold[NUMBER_OF_SENSORS] = {
    587 - 150,
	620 - 150,
	631 - 150,
	556 - 150,
	640 - 150,
	635 - 150,
	595 - 150,
	609 - 150,
	613 - 150,
	571 - 150,
	606 - 150,
	631 - 150,
	662 - 150,
	625 - 150,
	616 - 150,
	694 - 150,
	639 - 150,
	654 - 150,
	684 - 150,
	636 - 150,
	647 - 150,
	624 - 150,
	627 - 150,
	622 - 150,
};
/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */
void PeriodicTask_UpdateGlobalCounter(void);

/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */
void Notes_Init(void){
    Timer5A_Init(PeriodicTask_UpdateGlobalCounter, 80000, INCREMENT_GLOBAL_COUNTER_PRIORITY);
    global_ms_count = 0;
    for(int i = 0; i < NUMBER_OF_KEYS; i++){
        KeyStates_list[i].state = idle;
        KeyStates_list[i].currentTime_start = 0;
        KeyStates_list[i].elapsedTime = 0;
        KeyStates_list[i].pressMessageSent = true;
    }
}


void Notes_State_Update(void){
    for(int i = 0;i < NUMBER_OF_SENSORS;i++){
        uint16_t sample = ADC_Vals_Buf[i];
        enum KeyState next_state;
        switch (KeyStates_list[i].state)
        {

        case idle:
            if(sample < start_thresholds[i]) next_state = idle;
            else {
                KeyStates_list[i].currentTime_start = global_ms_count;
                next_state = start;
            }
            break;


        case start:
            if(sample < start_thresholds[i]) {
                KeyStates_list[i].currentTime_start = 0;
                next_state = idle;
            }
            else {
                if(sample > press_threshold[i]) {
                    //if diff is bigger than 300ms, dont send a press message
                    uint32_t global_time_record = global_ms_count;
                    if(global_time_record < KeyStates_list[i].currentTime_start){
                        KeyStates_list[i].elapsedTime = (UINT32_MAX - KeyStates_list[i].currentTime_start) + global_time_record;
                    }else{
                        KeyStates_list[i].elapsedTime = global_ms_count - KeyStates_list[i].currentTime_start; //gonna need to do abs value on this
                    }
                    if(KeyStates_list[i].elapsedTime < TIME_ELAPSED_NOT_PRESS){
                        SendOutSPI(CreateMessage(i, true, KeyStates_list[i].elapsedTime));
						//UART_OutUDec(KeyStates_list[i].elapsedTime);
						//UART_OutString("\n");
                    }
                    //SendOutSPI(CreateMessage(i, true, KeyStates_list[i].elapsedTime));
                    KeyStates_list[i].pressMessageSent = true;
                    next_state = press;
                }
                else next_state = start;
            }
            break;


        case press:
            if(sample > release_threshold[i]) next_state = press;
            else if(sample > start_thresholds[i]){
                next_state = start;
                KeyStates_list[i].currentTime_start = global_ms_count;
                KeyStates_list[i].elapsedTime = 0;
                KeyStates_list[i].pressMessageSent = false;
                //SEND RELEASE STATEMENT
                SendOutSPI(CreateMessage(i, false, 0));
            } else {
                next_state = idle;
                KeyStates_list[i].currentTime_start = 0;
                KeyStates_list[i].elapsedTime = 0;
                KeyStates_list[i].pressMessageSent = false;
                //SEND RELEASE STATEMENT
                SendOutSPI(CreateMessage(i, false, 0));
            }
            break;  
        default:
            next_state = idle;
            KeyStates_list[i].currentTime_start = 0;
            KeyStates_list[i].elapsedTime = 0;
            KeyStates_list[i].pressMessageSent = false;
            break;
        }

        KeyStates_list[i].state = next_state;
       
    }
}


void PeriodicTask_UpdateGlobalCounter(void){
    global_ms_count++;
}
