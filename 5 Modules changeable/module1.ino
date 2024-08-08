#include <SPI.h>
#include "DW1000Ranging.h"
#include <esp_now.h>
#include <WiFi.h>


#define ANCHOR_ADD "81:17:5V:D5:A9:5A:E2:4C" // Anchor address
#define TAG_ADD "83:13:2B:F5:G3:1A:L2:1C" // Tag address

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin


// Interval between range requests in milliseconds

bool isAnchor = true;
int firstTest3 = 0;
int firstTest4 = 0;
int firstTest5 = 0;
int secondTest2 = 0;


//Structure example to send data
//Must match the receiver structure

void setup()
{
    Serial.begin(115200);
    delay(1000);
    //init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    //SPI.setBitOrder(MSBFIRST);
    //SPI.setFrequency(20000000);
    //SPI.setDataMode(SPI_MODE0);
      
    //SPI.setClockDivider(SPI_CLOCK_DIV2);
  
    //SPI.setDataMode(SPI_MODE0);  // Set clock phase and polarity
    
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
    //define the sketch as anchor and tag
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachBlinkDevice(newBlink);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);

    DW1000Ranging.startAsTag(TAG_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
   
}

void loop()
{
  if(firstTest3 == 3 && firstTest4 == 4 && firstTest5 == 5){
    if (isAnchor) {
            // Start as anchor
            DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        } else {
            // Start as tag
            DW1000Ranging.startAsTag(TAG_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        }
        // Toggle between anchor and tag mode   
        
          firstTest3 = 0;
          firstTest4 = 0;
          firstTest5 = 0; 
    }else if(secondTest2 == 2){
      if (!isAnchor) {
            // Start as anchor
            DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        } else {
            // Start as tag
            DW1000Ranging.startAsTag(TAG_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        }
        secondTest2 = 0;
    }
    DW1000Ranging.loop(); 
}

void newRange()
{
    //Serial.print("from: ");
    //Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
    //byte* addressBytes = DW1000Ranging.getDistantDevice()->getByteAddress();
    //for(int i = 0; i < 2; i++) {
    //Serial.print(addressBytes[i], HEX);
  //}
    Serial.print("from: ");
    if (isAnchor) {
        // Eğer anchor modundaysak, anchor özel çıktıyı yazdır
        Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
        int address = DW1000Ranging.getDistantDevice()->getShortAddress();
        if(address == 6530){
          firstTest3 = 3;
        }else if(address == 5506) {
          firstTest4 = 4;
        }else if(address == 5249){
          firstTest5 = 5;
        }else if(address == 4483){
          secondTest2 = 2;
        }
      
    } else {
        // Eğer tag modundaysak, tag özel çıktıyı yazdır
        byte* addressBytes = DW1000Ranging.getDistantDevice()->getByteAddress();
        for(int i = 0; i < 2; i++) {
            Serial.print(addressBytes[i]);
        }
    }
    Serial.print("\t Range: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRange());
    Serial.print(" m");
    Serial.print("\t RX power: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
    Serial.println(" dBm");
}

void newBlink(DW1000Device *device)
{
    Serial.print("blink; 1 device added ! -> ");
    Serial.print(" short:");
    Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
    Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);
}