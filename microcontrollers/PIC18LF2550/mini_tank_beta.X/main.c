/*
 * File:   main.c
 * Author: jonc
 *
 * Created on November 25, 2017
 */

#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = INTOSC_EC // Oscillator Selection bits (Internal oscillator, CLKO function on RA6, EC used by USB (INTCKO))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = OFF      // MCLR Pin Enable bit (RE3 input pin enabled; MCLR pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

#define _XTAL_FREQ 800000 // System clock frequency

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
//#define PWMA PORTCbits.RC1 
//#define PWMB PORTCbits.RC2

#define AIN2 PORTAbits.RA0
#define AIN1 PORTAbits.RA1 
#define STBY PORTAbits.RA2

#define BIN1 PORTAbits.RA3 
#define BIN2 PORTAbits.RA4

#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include "usart.h"

struct Tank {
    char messageType[32];
    char speed[32];
};

void setup_timer(unsigned period){
    /* Timer2 configuration */
    PR2 = period;           //  configure timer2 period
    T2CON = 0x02;           //  Set prescalar 16   
    TMR2ON = 1;             //  timer2 on
}

void PWMA_Init(){
    TRISCbits.RC1 = 0;         //  Set RC1 as output
    
    /* CCP PWM mode */
    CCP1CON &= 0xCF;        //  5,4 bits zeroed (DC1B1:DC1B0 = 00)
    CCP1CON |= 0x0C;        //  PWM mode ( CCP1M3:CCP1M0 = 1100)
}

void PWMB_Init(){
    TRISCbits.RC2 = 0;         //  Set RC2 as output
    
    /* CCP PWM mode */
    CCP2CON &= 0xCF;        //  5,4 bits zeroed (DC1B1:DC1B0 = 00)
    CCP2CON |= 0x0C;        //  PWM mode ( CCP1M3:CCP1M0 = 1100)
}

// Duty cycle range is between 0-1023, 1023 is fast and 0 is slow
void PWMA_setDC(unsigned int dutycycle){
    
    CCPR1L = dutycycle>>2;  //  PWM duty cycle - first 8-bits (MSb)
    CCP1CON &= 0xCF;        //  5,4 bits zeroed (DC1B1:DC1B0 = 00)
    CCP1CON |= ((dutycycle<<4)&0x30);  //  PWM duty cycle - last 2-bits (LSb) in CCP1CON 5,4 bits    
}

// Duty cycle range is between 0-1023, 1023 is fast and 0 is slow
void PWMB_setDC(unsigned int dutycycle){
    
    CCPR2L = dutycycle>>2;  //  PWM duty cycle - first 8-bits (MSb)
    CCP2CON &= 0xCF;        //  5,4 bits zeroed (DC1B1:DC1B0 = 00)
    CCP2CON |= ((dutycycle<<4)&0x30);  //  PWM duty cycle - last 2-bits (LSb) in CCP2CON 5,4 bits    
}

struct Tank parseMessage(char message[])
{
    struct Tank tank;
    strcpy(tank.messageType, strtok(message, "_"));
    
    if(strcmp(tank.messageType, NULL) != 0){
        strcpy(tank.speed, strtok(NULL, "_"));
    }
    else
    {
        strcpy(tank.speed, "1023");
    }

    return tank;
}

int main()
{
    OSCCON=0x72;       // use internal oscillator frequency  which is set to * MHz        
    ADCON1bits.PCFG = 15; //all digital

    TRISAbits.RA0 = 0;
    TRISAbits.RA1 = 0;
    
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    TRISAbits.RA4 = 0;
    
    // Set as input port for the PIR motion sensor
    TRISAbits.RA5 = 1;
    
    // Set LED as output port
    TRISCbits.RC0 = 0;
    // LED is turned off by default
    PORTCbits.RC0 = 0;
    
     /*open PWM at 2KHz*/ 
    PWMA_Init();
    PWMB_Init();
    setup_timer(624); //0x270
    
    USART_Init(9600);         /* initialize USART operation with 9600 baud rate */ 
    __delay_ms(50);

    struct Tank tank;
    
    
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
    STBY = 0;

    while(1)
    {
        char  message[32];
        USART_RecieveString(message);
        // USART_SendString(message);
        // First letter of message will 
        tank = parseMessage(message);
       
        
        unsigned speed  = 1023;
        
        char *ptr;
        
		// Before it was  if(tank.messageType != "S"){, possible bug fix, need to test
        if(strcmp(tank.messageType, "S") != 0){
            speed = strtoul(tank.speed, &ptr, 10);
        }

        if(strcmp(tank.messageType, "U") == 0)
        {   
            AIN1 = 0;
            AIN2 = 1;
            PWMA_setDC(speed);
            BIN1 = 1;
            BIN2 = 0;
            PWMB_setDC(speed);
            STBY = 1;

            //USART_SendString("LED_ON"); /* send LED ON status to terminal */
        }
        else if(strcmp(tank.messageType, "R") == 0){
            AIN1 = 1;
            AIN2 = 0;
            PWMA_setDC(speed);
            BIN1 = 1;
            BIN2 = 0;
            PWMB_setDC(speed);
            STBY = 1;
        }
        else if(strcmp(tank.messageType, "B") == 0){ // backwards
            AIN1 = 1;
            AIN2 = 0;
            PWMA_setDC(speed);
            BIN1 = 0;
            BIN2 = 1;
            PWMB_setDC(speed);
            STBY = 1;
        }
        else if(strcmp(tank.messageType, "L") == 0){ 
            AIN1 = 0;
            AIN2 = 1;
            PWMA_setDC(speed);
            BIN1 = 0;
            BIN2 = 1;
            PWMB_setDC(speed);
            STBY = 1;
        }
        else if(strcmp(tank.messageType, "S") == 0)          
        { 
            //PWMA_setDC(0);
            //PWMA = 0;
            //PWMB_setDC(0);
            AIN1 = 0;
            AIN2 = 0;
            BIN1 = 0;
            BIN2 = 0;
            STBY = 0;
            //USART_SendString("LED_OFF");/* send LED ON status to terminal */
        }
        else if(strcmp(tank.messageType, "1") == 0){ // TOP LEFT
            AIN1 = 0;
            AIN2 = 1;
            PWMA_setDC(speed);  
            STBY = 1;
        }
        else if(strcmp(tank.messageType, "2") == 0){ // TOP RIGHT
            BIN1 = 1;
            BIN2 = 0;
            PWMB_setDC(speed);
            STBY = 1;
        }
        else if(strcmp(tank.messageType, "3") == 0){ // BOTTOM LEFT
            AIN1 = 1;
            AIN2 = 0;
            PWMA_setDC(speed);
            STBY = 1;
        }
        else if(strcmp(tank.messageType, "4") == 0){ // BOTTOM RIGHT
            BIN1 = 0;
            BIN2 = 1;
            PWMB_setDC(speed);
            STBY = 1;
        }
        else if(strcmp(tank.messageType, "N") == 0) // Turn off LED and enable motion sensor detection
        {
            motion_disabled = 0;
            PORTCbits.RC0 = 0;
        }
        else if(strcmp(tank.messageType, "F") == 0) // Turn on LED and disable motion sensor detection
        {
            motion_disabled = 1;
            PORTCbits.RC0 = 1;
        }
    }
    
    return 0;
}