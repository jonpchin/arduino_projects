#include "usart.h"
#include <pic18lf2550.h>

int motion_disabled = 0;

/*****************************USART Initialization*******************************/
void USART_Init(long baud_rate)
{
    TRISCbits.TRISC6=0;                       /*Make Tx pin as output*/
    TRISCbits.TRISC7=1;                       /*Make Rx pin as input*/
    float temp =  (((float)(F_CPU)/(float)baud_rate)-1);
    SPBRG=(int)temp;                /*baud rate=9600, SPBRG = (F_CPU /(64*9600))-1*/
    TXSTA=0x20;                     /*Transmit Enable(TX) enable*/ 
    RCSTA=0x90;                     /*Receive Enable(RX) enable and serial port enable */
}
/******************TRANSMIT FUNCTION*****************************************/ 
void USART_TransmitChar(char out)
{        
        while(TXIF==0);            /*wait for transmit interrupt flag*/
        TXREG=out;                 /*wait for transmit interrupt flag to set which indicates TXREG is ready
                                    for another transmission*/    
}
/*******************RECEIVE FUNCTION*****************************************/
char USART_ReceiveChar()
{
    if(OERR) // check for Error 
    {
        CREN = 0; //If error -> Reset 
        CREN = 1; //If error -> Reset 
    }
    
    while(RCIF==0)  /*wait for receive interrupt flag*/
    { 
        if(PORTAbits.RA5 && motion_disabled == 0)
        {
           // TODO: Figure out why first character in string is not sent to android remote
           USART_SendString(" motion");
           motion_disabled = 1;
           PORTCbits.RC0 = 1;
        }
    }                
    return(RCREG);                  /*receive data is stored in RCREG register and return to main program */
}

// Limit max string size to 32 char
void USART_RecieveString(char text[32])
{
    unsigned i=0;      /*array index variable */
    /*character is received until end of the string received */
    char letter = USART_ReceiveChar();   
    text[0] = letter;
    i++;
    
    while(letter != '\0' && i<30)
    {
        letter = USART_ReceiveChar();
        text[i]=letter;
        i++;

    }
    text[i] = '\0';
    //return text;   /* return the base address of received string */
}

// String limit for one message will be 31 characters
void USART_SendString(const char *out)
{
    unsigned i = 0;
    while(*out!='\0' && i<30)
   {            
        USART_TransmitChar(*out);
        out++;
        i++;
   }
}