/*
SD Card Reader testing

 The circuit:
  * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module.
 		Pin 4 used here for consistency with other Arduino examples
 		
jonc
12/24/2017

 */

#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
const int chipSelect = 4;

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    
    Serial.print("\nInitializing SD card...");
    
    // we'll use the initialization code from the utility libraries
    // since we're just testing if the card is working!
    if (!card.init(SPI_HALF_SPEED, chipSelect)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card inserted?");
        Serial.println("* is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
        return;
    } else {
        Serial.println("Wiring is correct and a card is present.");
    }

    
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    bool result = SD.mkdir("/haha");
    if(result){
        Serial.println("Folder created");
    }else{
        Serial.println("Could not create folder");
    }
    writeFile("/haha/wow.txt", "hi foo");
    //writeFile("test123.txt", "yay test123");
    //readFile("test.txt");
    //listSDContents();
    printSDCardInfo();
    //deleteFile("test.txt");
}

void printSDCardInfo(){
	// print the type of card
	Serial.print("\nCard type: ");
	switch (card.type()) {
    case SD_CARD_TYPE_SD1:
        Serial.println("SD1");
        break;
	case SD_CARD_TYPE_SD2:
	    Serial.println("SD2");
	    break;
	case SD_CARD_TYPE_SDHC:
	    Serial.println("SDHC");
	    break;
	default:
	    Serial.println("Unknown");
	}
	
	// Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
	if (!volume.init(card)) {
    	Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    	return;
	}
	
	// print the type and size of the first FAT-type volume
	uint32_t volumesize;
	Serial.print("\nVolume type is FAT");
	Serial.println(volume.fatType(), DEC);
	Serial.println();
	
	volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
	volumesize *= volume.clusterCount();       // we'll have a lot of clusters
	volumesize *= 512;                            // SD card blocks are always 512 bytes
	Serial.print("Volume size (bytes): ");
	Serial.println(volumesize);
	Serial.print("Volume size (Kbytes): ");
	volumesize /= 1024;
	Serial.println(volumesize);
	Serial.print("Volume size (Mbytes): ");
	volumesize /= 1024;
	Serial.println(volumesize);

	Serial.println("\nFiles found on the card (name, date and size in bytes): ");
	root.openRoot(volume);
	
	// list all files in the card with date and size
	root.ls(LS_R | LS_DATE | LS_SIZE);
}

bool doesFileExist(String filePath){
	if (SD.exists(filePath)) {
    	Serial.println(filePath + " exists.");
    	return true;
	} else {
    	Serial.println(filePath + " doesn't exist.");
    	return false;
 	}
}

void createFile(String filePath){
	// open a new file and immediately close it:
	Serial.println("Creating file: " + filePath);
	File file = SD.open(filePath, FILE_WRITE);
	file.close();
}

void deleteFile(String filePath){
	if(doesFileExist(filePath)){
		Serial.println("Deleted file: " + filePath);
  		SD.remove(filePath);
	}else{
		Serial.println(filePath + "does not exist no need to delete");
	}
}

void readFile(String filePath){

    Serial.println("Reading file contents: " + filePath);
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open(filePath);
    
    // if the file is available, write to it:
    if (dataFile) {
        while (dataFile.available()) {
            Serial.write(dataFile.read());
        }
        dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
        Serial.println("error opening " +  filePath +", check if file exists.");
    }
}

void writeFile(String filePath, String content){
	// open the file. Only one file can be open at a time,
    // so close this one before opening another.
    File file = SD.open(filePath, FILE_WRITE);
    
    // if the file opened, write to it:
    if (file) {
        Serial.print("Writing to: " + filePath);
        file.println(content);
        // close the file:
        file.close();
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening: " + filePath + " for writeFile");
    }
}

void listSDContents(){
	File root = SD.open("/");
  	printDirectoryContents(root, 0);
}

void printDirectoryContents(File dir, int numTabs) {
	while (true) {

	    File entry =  dir.openNextFile();
	    
	    if (! entry) {
	    	// no more files
	    	break;
	    }
	    for (uint8_t i = 0; i < numTabs; i++) {
	    	Serial.print('\t');
	    }
	   	Serial.print(entry.name());
	    if (entry.isDirectory()) {
	    	Serial.println("/");
	    	printDirectoryContents(entry, numTabs + 1);
	    } else {
	     	// files have sizes, directories do not
	    	Serial.print("\t\t");
	    	Serial.println(entry.size(), DEC);
		}
			entry.close();
	}
}


void loop(void) {

}
