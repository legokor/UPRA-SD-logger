#include <SPI.h>
#include <SdFat.h>

#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

//SD CARD - SPI
#define MOSI    11        //MOSI
#define MISO    12        //MISO
#define CLK     13        //CLK        
#define CS      10        //CHIP SELECT

#define DEFAULT_BAUD  9600

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

/*
 * SD Card variables
 */

bool card_present=false;

SdFat sd;
SdFile dataFile;
uint32_t conf_baud;
char reader;

uint32_t config_get_baud(void);

void setup() {
  // put your setup code here, to run once:
    //SD CARD init
  if (!sd.begin(CS, SPI_HALF_SPEED)) 
  {
    card_present=false;
    Serial.begin(DEFAULT_BAUD);
    Serial.println("default, nocard");
  }
  else
  {
    card_present=true;
    if (dataFile.open("data.txt", O_RDWR | O_CREAT | O_AT_END))
    {
      dataFile.println(F("BACKUP GPS DATALOG"));
      dataFile.close();
      Serial.println("datafile created");
    }   
    else 
    {
      card_present=false;
      dataFile.close();
    }
    conf_baud = config_get_baud();
    Serial.begin(conf_baud);
    Serial.println("config");
  }
  dataFile.close();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((stringComplete) && (card_present))
  {
    if (dataFile.open("data.txt", O_RDWR | O_CREAT | O_AT_END))
    {
      dataFile.print(inputString);
    }   
    dataFile.close();
    Serial.println("ok");
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() 
{
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') 
    {
      stringComplete = true;
    }
  }
}

uint32_t config_get_baud(void)
{
    uint32_t baud;
    if( dataFile.open("config.cfg", O_READ))
    {
      reader = 0;          
      baud = 0;
      while(1)
      {
        reader = dataFile.read();
        delay(20);
        if((reader == '\n') || (reader == '\r'))
        {
          if(baud != 0) return baud;
          return DEFAULT_BAUD;
        }
        baud *= 10;
        delay(20);
        baud += reader - '0';
        delay(20);
      }
    }
    dataFile.close();
    return DEFAULT_BAUD;
}


