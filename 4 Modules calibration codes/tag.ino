#include <SPI.h>
#include "DW1000Ranging.h"
#include <DW1000.h>
#include <esp_now.h>
#include <WiFi.h>

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4
#define TAG_ADD "83:13:2B:F5:G3:1A:L2:1C"

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

typedef struct struct_message {
    float x;
    float y;
} struct_message;
 
// Create a struct_message called myData
struct_message myData;

double array[3][3];

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    
    // Print the MAC address
  //Serial.print("Received data from: ");
  //Serial.println(macStr);
  //Serial.print("Received Data: ");
  //Serial.println(myData.b);
  
  if(strcmp(macStr, "EC:62:60:E4:13:48") == 0){
    array[0][1] = myData.x;
    array[0][2] = myData.y;
  } else if(strcmp(macStr, "C8:F0:9E:53:13:0C") == 0){
    array[2][1] = myData.x;
    array[2][2] = myData.y;
  }else if(strcmp(macStr, "C8:F0:9E:52:50:94") == 0){
    array[1][1] = myData.x;
    array[1][2] = myData.y;
  }else{
    array[0][1] = 0;
    array[1][1] = 0;
    array[2][1] = 0;
    array[0][2] = 0;
    array[1][2] = 0;
    array[2][2] = 0;
  }
  printf("%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f \n", array[0][0], array[0][1], array[0][2], array[1][0], array[1][1], array[1][2], array[2][0], array[2][1], array[2][2]);
  //int time = millis();
  //printf(" %d ",time);

}



void setup()
{
    Serial.begin(115200);
    //init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
    //define the sketch as anchor. It will be great to dynamically change the type of module
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);
    //Enable the filter to smooth the distance
    //DW1000Ranging.useRangeFilter(true);

    //we start the module as a tag
    //DW1000.setPulseFrequency(64);
    //DW1000.setPreambleLength(2048);
    //DW1000.setDataRate(110);
    //DW1000.setChannel(2);
    //DW1000.commitConfiguration();
    DW1000Ranging.startAsTag(TAG_ADD, DW1000.MODE_SHORTDATA_FAST_LOWPOWER, false);

    WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
}
  

    

void loop()
{
  DW1000Ranging.loop();
    
}
void newRange()
{
    //Serial.print("from: ");
    //Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
    int address = DW1000Ranging.getDistantDevice()->getShortAddress();

    //Serial.print("\t Range: ");
    //Serial.print(DW1000Ranging.getDistantDevice()->getRange());
    //Serial.print(" m");
    double range = DW1000Ranging.getDistantDevice()->getRange();

    if(address == 6021){
     array[0][0] = range;
    }else if(address == 4994){
      array[1][0] = range;
    }else if(address = 5249){
      array[2][0] = range;
    }else {
      array[0][0] = 0;
      array[1][0] = 0;
      array[2][0] = 0;
    }
    
    
    //Serial.print("\t RX power: ");
    //Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
    //Serial.println(" dBm");
}

void newDevice(DW1000Device *device)
{
    //Serial.print("ranging init; 1 device added ! -> ");
    //Serial.print(" short:");
    //Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
    //Serial.print("delete inactive device: ");
    //Serial.println(device->getShortAddress(), HEX);
}
