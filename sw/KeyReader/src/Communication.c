/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "../inc/tm4c123gh6pm.h"
#include "../inc/Communication.h"
#include "stdint.h"
#include "stdbool.h"
#include "../inc/Notes.h"
#include "../inc/Timer1A.h"
#include "../inc/Timer3A.h"

/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
#define NumberOfParams 4
//makeing array of test msgs : note, press release(0:release, 1:press), velocity(0-300),
uint32_t testMessages[][NumberOfParams] = {
    {C4, true, 50, 80000000},
    {C4, false, 0, 80000000},
    {C4, true, 200, 80000000},
    {C4, false, 0, 80000000},
    {C4, true, 500, 80000000},
    {C4, false, 0, 80000000},
};
#define NumberOfMessages 6
uint16_t testMessages_idx = 0;

#define KEY_PRESS_FIFO_SIZE 24
uint8_t keyPressFIFO[KEY_PRESS_FIFO_SIZE];
uint8_t head_key_PressFIFO;
uint8_t tail_key_PressFIFO;

#define SENDING_THROUGH_SPI_PERIOD 80000

/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */
void PeriodicTask_SendThroughSPI(void);

/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */
void PortB_SPI_Master_Init(void){
    // 1. Enable clock for Port B and SSI2
    SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R2;  // Enable clock for SSI2
    while((SYSCTL_PRSSI_R & SYSCTL_RCGCSSI_R2) == 0) {}  // Wait until SSI2 is ready

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;  // Enable clock for Port B
    while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0) {};  // Wait for Port B to be ready

    // 2. Configure Port B pins for SSI2 (SPI) with only transmit functionality
    GPIO_PORTB_AFSEL_R |= 0xB0;  // Enable alternate functions on PB4, PB5, and PB7 (SSI2Clk, SSI2Fss, SSI2Tx)
    GPIO_PORTB_DEN_R |= 0xB0;    // Enable digital functionality on PB4, PB5, and PB7
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0x0F00FFFF) | 0x20220000;  // Configure PB4, PB5, PB7 as SSI2
    GPIO_PORTB_AMSEL_R &= ~0xB0;  // Disable analog functionality on PB4, PB5, PB7
    //GPIO_PORTB_DR4R_R |= 0x80;

    // 3. Configure SSI2 for SPI operation (Master Mode) with transmit only
    SSI2_CR1_R = 0x00000000;  // Disable SSI2 for configuration (Master Mode)
    SSI2_CC_R = 0x0;  // Select system clock (20 MHz) as the source for SSI2

    // 4. Configure clock prescale and SCR for 1 MHz SPI Clock
    SSI2_CPSR_R = 10;  // Set prescaler to 80
    SSI2_CR0_R = (0x0F << 0)  |  // DSS = 8-bit data (0x7 for 8 bits)
                 (0x00 << 4)  |  // FRF = 0, Freescale SPI Frame Format
                 (0x00 << 6)  |  // SPO = 0, clock is idle low
                 (0x00 << 7)  |  // SPH = 0, data is captured on the first clock edge
                 (7 << 8);       // SCR = 0 for a 1 MHz SPI clock

    // 5. Enable SSI2
    SSI2_CR1_R |= 0x00000002;  // Enable SSI2 (Master Mode)
}



void Chip_to_Chip_Init(void){
    head_key_PressFIFO = 0;
    tail_key_PressFIFO = 0;
    PortB_SPI_Master_Init();
    //Timer1A_Init(PeriodicTask_SendThroughSPI, SENDING_THROUGH_SPI_PERIOD, 0);
}

//returns true if successful and false and profile pe1 if full
bool Push_keyPressFIFO(uint8_t inMessage){
    uint8_t next_key_PressFIFO = head_key_PressFIFO + 1;
    next_key_PressFIFO = next_key_PressFIFO >= KEY_PRESS_FIFO_SIZE? 0 : next_key_PressFIFO;

    if(next_key_PressFIFO == tail_key_PressFIFO){
        GPIO_PORTE_DATA_R |= 0x02;//turn on pe1 if fifo is full
        return false;
    }

    keyPressFIFO[head_key_PressFIFO] = inMessage;
    head_key_PressFIFO = next_key_PressFIFO;
    return true;
}

//returns false if empty, true if successful
bool Pop_keyPressFIFO(uint8_t* outMessage){
    if(head_key_PressFIFO == tail_key_PressFIFO){
        return false;
    }
    *outMessage = keyPressFIFO[tail_key_PressFIFO];
    tail_key_PressFIFO++;
    tail_key_PressFIFO = tail_key_PressFIFO >= KEY_PRESS_FIFO_SIZE? 0 : tail_key_PressFIFO;
    return true;
}

uint16_t CreateMessage(enum NotesIndexes note, bool isPress, uint32_t velocity) {
    uint16_t outMessage = (velocity << 6) | (isPress << 5) | (note & 0x1F);
    return outMessage;
}


void PeriodicTask_SendThroughSPI(void){
    while (SSI2_SR_R & SSI_SR_BSY){};
    SSI2_DR_R = 0xAAF0;
    // uint8_t message;
    // if(Pop_keyPressFIFO(&message)){
    //     while (SSI1_SR_R & SSI_SR_BSY){};  // Wait for SSI1 transfer to complete
    //     //SSI1_DR_R = message;
    //     GPIO_PORTE_DATA_R |= 0x2;
    // }
}

void SendTestMessage(void){
    Push_keyPressFIFO(CreateMessage(25, 1, 8));
}

void PeriodicTask_SendTestMessage(void){
    uint16_t  message = CreateMessage(testMessages[testMessages_idx][0], testMessages[testMessages_idx][1], testMessages[testMessages_idx][2]);
    while (SSI2_SR_R & SSI_SR_BSY){};
    SSI2_DR_R =  message;
    while (SSI2_SR_R & SSI_SR_BSY){};
    SSI2_DR_R =  message;
    while (SSI2_SR_R & SSI_SR_BSY){};
    SSI2_DR_R =  message;
    while (SSI2_SR_R & SSI_SR_BSY){};
    SSI2_DR_R =  message;
    // while (SSI2_SR_R & SSI_SR_BSY){};
    // SSI2_DR_R =  message;
    uint16_t press = testMessages[testMessages_idx][1];
    if(press == 1){
        GPIO_PORTF_DATA_R ^= 0x02;
    }else if (press == 0){
        //GPIO_PORTF_DATA_R ^= 0x04;
    }
    
    // Timer3A_Stop();
    // TIMER1_TAILR_R = testMessages[testMessages_idx][3];
    
    testMessages_idx++;
    testMessages_idx%=NumberOfMessages;
    
    

    // //NVIC_DIS1_R = 0<<(35-32);        // 9) enable interrupt 35in NVIC
    // TIMER1_CTL_R = ~TIMER_CTL_TAEN;       // 10) enable timer4A
    // TIMER1_TAILR_R = (uint32_t) testMessages[testMessages_idx];
    // TIMER1_CTL_R = ~TIMER_CTL_TAEN;

}

void SendOutSPI(uint16_t messageOut){
    while (SSI2_SR_R & SSI_SR_BSY){};
    SSI2_DR_R = messageOut;
    while (SSI2_SR_R & SSI_SR_BSY){};
    SSI2_DR_R = messageOut;
    while (SSI2_SR_R & SSI_SR_BSY){};
    SSI2_DR_R = messageOut;
    while (SSI2_SR_R & SSI_SR_BSY){};
    SSI2_DR_R = messageOut;
}

void TestSendingMessages_Init(void){
    PortB_SPI_Master_Init();
    Timer1A_Init(PeriodicTask_SendTestMessage, 80000000, 1);
}
