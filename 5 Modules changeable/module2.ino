#include <SPI.h>
#include "DW1000Ranging.h"
#include <esp_now.h>
#include <WiFi.h>


#define TAG_ADD "83:11:2B:F5:G3:1A:L2:1C" // Tag address

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