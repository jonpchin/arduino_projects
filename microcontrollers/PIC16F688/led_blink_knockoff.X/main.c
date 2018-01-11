// Jonathan Chin
// 9/26/2017
// Binking LED with knockoff circuit debugger

// CONFIG
#pragma config FOSC = EXTRCCLK  // Oscillator Selection bits (EXTRC oscillator: External RC on RA5/OSC1/CLKIN, CLKOUT function on RA4/OSC2/CLKOUT pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

#include <xc.h>
#define _XTAL_FREQ 8000000      // System clock frequency

void main(void) {
    OSCCONbits.SCS  = 0b10;         // Internal RC is used for system clock
    ANSEL = 0x00;                   // Set all I/O to digital I/O
    
    TRISCbits.TRISC0    = 0;        // Set RC0 as output

    while(1)
    {
        PORTCbits.RC0 = 1; // Toggle LED pin
        __delay_ms(1000);
        PORTCbits.RC0 = 0;
        __delay_ms(1000);
    }
    
    return;
}