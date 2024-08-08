#include <SPI.h>
#include "DW1000Ranging.h"
#include <esp_now.h>
#include <WiFi.h>

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

int address;
int number, number2, number3, receivedValue;
double rangeA1, rangeA2, rangeA3;

uint8_t broadcastAddress[] = {0xEC, 0x62, 0x60, 0xE4, 0x13, 0x48};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int x;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  receivedValue = myData.x;
}


void setup()
{
    Serial.begin(115200);
    delay(1000);
    //init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
    //define the sketch as anchor. It will be great to dynamically change the type of module
    DW1000Ranging.attachNewRange(newRange);
    //Enable the filter to smooth the distance
    //DW1000Ranging.useRangeFilter(true);

    //we start the module as a tag
    DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_SHORTDATA_FAST_LOWPOWER, false);

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
    }

  
  // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
  
  // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
    
    if(number == 1 && number2 == 2 && number3 == 3){
      
      myData.x = 1;
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

      if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
      else {
      Serial.println("Error sending the data");
    }
    
    DW1000Ranging.startAsAnchor("85:11:5B:D5:A9:9A:E2:9C", DW1000.MODE_SHORTDATA_FAST_LOWPOWER, false);
    number = 0;
    number2 = 0;
    number3 = 0;
  }else if(receivedValue == 2){
    DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_SHORTDATA_FAST_LOWPOWER, false);
    receivedValue = 0;
  }
    //printf("1,%.3f,%.3f,%.3f \n", rangeA1, rangeA2, rangeA3);
    DW1000Ranging.loop();
}

void newRange()
{
    Serial.print("from: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
    Serial.print("\t Range: ");
    address = DW1000Ranging.getDistantDevice()->getShortAddress();
    if(address == 6022 || address == 4995){
      rangeA1 = DW1000Ranging.getDistantDevice()->getRange();
      number = 1;
    }else if(address == 4736){
      rangeA2 = DW1000Ranging.getDistantDevice()->getRange();
      number2 = 2;
    }else if(address == 5249){
      rangeA3 = DW1000Ranging.getDistantDevice()->getRange();
      number3 = 3;
    }
    Serial.print(DW1000Ranging.getDistantDevice()->getRange());
    Serial.print(" m");
    Serial.print("\t RX power: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
    Serial.println(" dBm");
}
