// Jonathan Chin
// 9/9/2017
// Can be used to unlock security doors using RFID
// Tested with Asiawill RDM6300 125Khz EM4100 RFID Reader Module UART

// tags.h is .gitignore but it is placed in same directory as file in the sample format below:
// #ifndef SERIAL_TAGS
// #define SERIAL_TAGS
//  extern int tags[][14] = 
//  {
//    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
//    {14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
//    {1, 3, 3, 7, 1, 3, 3, 7, 1, 3, 3, 3, 67, 1}
//  };
//#endif

#include <SoftwareSerial.h>
#include "tags.h"
SoftwareSerial RFID(2, 3); // RX and TX

int yes = 13;
int no = 12;
 
void setup()
{
    RFID.begin(9600);    // start serial to RFID reader
    Serial.begin(9600);  // start serial to PC 
    pinMode(yes, OUTPUT); // for status LEDs
    pinMode(no, OUTPUT);
}
 
boolean compareTag(int a[14], int b[14])
{
    for (int i = 0 ; i < 14 ; ++i)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

// Returns true if scanned tag matches a known tag
boolean isTagMatch(int newtag[14])
{
    int totalRows = sizeof(tags)/sizeof(tags[0]);
    
    for(int i=0; i<totalRows; ++i)
    {
        if (compareTag(newtag, tags[i]))
        {
            return true;
        }
    }
    return false;
}
 
void readTags()
{ 
    if (RFID.available() > 0) 
    {
        // used for read comparisons
        int newtag[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
        // read tag numbers
        delay(100); // needed to allow time for the data to come in from the serial buffer.
 
        for (int i = 0 ; i < 14 ; ++i) // read the rest of the tag
        {
            int data  = RFID.read();
			newtag[i] = data;
        }
    
        RFID.flush(); // stops multiple reads
 
        // do the tags match up?
        if(isTagMatch(newtag))
        {
			Serial.println("Accepted");
			printTag(newtag);
			digitalWrite(yes, HIGH);
			delay(1000);
			digitalWrite(yes, LOW);
        }
        else
        {
			Serial.println("Rejected");
			printTag(newtag);
			digitalWrite(no, HIGH);
			delay(1000);
			digitalWrite(no, LOW);
        }
    }
}

void printTag(int tag[14])
{
    for(int i=0; i<14; ++i)
    {
        Serial.print(tag[i]);
        Serial.print(" ");
    }
    Serial.println("\n");
}
 
void loop()
{
    readTags();
}
