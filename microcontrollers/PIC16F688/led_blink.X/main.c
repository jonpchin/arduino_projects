// Jonathan Chin
// 9/26/2017
// Binking LED

// CONFIG
#include <xc.h>

#include "thirdparty/lcd_hd44780_pic16.h"

void main (void)
{
  //Initialize the LCD Module
  LCDInit(LS_NONE);

  //Clear the display
  LCDClear();

  //Write a string
  LCDWriteString("Hello World !");

  while(1)
  {
     //Do nothing, just loop indefinitely
  }

}