#include <SPI.h>
#include "DW1000Ranging.h"
#include <esp_now.h>
#include <WiFi.h>


#define ANCHOR_ADD "80:19:3B:S5:A2:8A:E2:1C"
#define TAG_ADD "85:01:5C:D7:A3:9D:F2:3K" // Tag address

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 5

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 5;   // spi select pin


// Interval between range requests in milliseconds

bool isAnchor = false;
int number = 0;
int number2 = 0;
int firstMessage = 10;

uint8_t broadcastAddress[] = {0xEC, 0x62, 0x60, 0xE4, 0x1D, 0xFC};


//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    char msg[50];
} struct_message;

// Create a struct_message called DHTReadings to hold sensor readings
struct_message outgoingReadings;

struct_message incomingReadings;

// Variable to store if sending data was successful
String success;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  }
  else {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Data: ");Serial.println(incomingReadings.msg);
}

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

    DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  for (int ii = 0; ii < 6; ++ii )
  {
    peerInfo.peer_addr[ii] = (uint8_t) broadcastAddress[ii];
  }
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  if(incomingReadings.msg == "2"){
      isAnchor = !isAnchor;
        if (isAnchor) {
            // Start as anchor
            DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        } else {
            // Start as tag
            DW1000Ranging.startAsTag(TAG_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        }
        // Toggle between anchor and tag mode   
          
          number = 0;
          number2 = 0;
          
    }else if(number == 2 && number2 == 3){
    isAnchor = !isAnchor;
    if (isAnchor) {
            // Start as anchor
            DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        } else {
            // Start as tag
            DW1000Ranging.startAsTag(TAG_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        }
        // Toggle between anchor and tag mode   
          strcpy(outgoingReadings.msg, "3");
          esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));
          
          number = 0;
          number2 = 0;
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
    if (!isAnchor) {
        // Eğer anchor modundaysak, anchor özel çıktıyı yazdır
        Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
        int address = DW1000Ranging.getDistantDevice()->getShortAddress();
        if(address == 4483){
          number = 2;
        }else if(address == 6528) {
          number2 = 3;
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