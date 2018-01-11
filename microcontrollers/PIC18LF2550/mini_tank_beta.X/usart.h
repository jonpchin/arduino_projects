#ifndef USART_HEADER_FILE_H
#define	USART_HEADER_FILE_H

void USART_Init(long);
void USART_TransmitChar(char);
void USART_SendString(const char *);
char USART_ReceiveChar();
void USART_RecieveString(char text[10]);

extern int motion_disabled;

#define F_CPU 8000000/64

#endif

