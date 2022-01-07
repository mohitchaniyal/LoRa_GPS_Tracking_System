#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define SS_PIN 3
#define RST_PIN 2

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin
//const int led1 = 4;
//const int led2 = 5;
//const int led3 = 6;
//const int led4 = 7;
//const int led5 = 8;
float t ;
String outgoing;              // outgoing message
int recipient;
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xB1;     // address of this device
byte destination = 0xFF;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends
byte sender;
bool recFlag = false;
int recipientlast;
byte senderlast;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
int code[] = {69,141,8,136}; //This is the stored UID
int codeRead = 0;
String uidString;

void setup() {
  Serial.begin(9600);
  Serial.println("LoRa Duplex Node A");
  
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothin]g
  }
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  // Clear the buffer.
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE); // or BLACK);
  display.setTextSize(2);
  display.setCursor(10,0); 
  display.print("RFID Lock");
  display.display();
  
}




void loop() {
  /* if (millis() - lastSendTime > interval) {
     String message = "HeLoRa World!";   // send a message
     sendMessage(message, node);
     node++;
     if (node > 21)node = 0;
     Serial.println("Sending " + message);
     lastSendTime = millis();            // timestamp the message
     interval = random(2000) + 1000;    // 2-3 seconds
    }*/

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  if(  rfid.PICC_IsNewCardPresent())
  {
      readRFID();
  }
  delay(100);



}

void sendMessage(String outgoing ) {
  LoRa.beginPacket();                   // start packet


  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;
  if (recFlag == true) {
    int ack = LoRa.read();
    if (ack == 12345678) {
      recipient = LoRa.read();          // recipient address
      sender = LoRa.read();            // sender address
      byte incomingMsgId = LoRa.read();     // incoming msg ID
      byte incomingLength = LoRa.read();    // incoming msg length

      String incoming = "";

      while (LoRa.available()) {
        incoming += (char)LoRa.read();
      }

      delay(5);
      LoRa.beginPacket();                   // start packet


      LoRa.write(destination);              // add destination address
      LoRa.write(localAddress);             // add sender address
      LoRa.write(msgCount);                 // add message ID
      LoRa.write(outgoing.length());        // add payload length
      LoRa.print(outgoing);                 // add payload
      LoRa.endPacket();                     // finish packet and send it
      msgCount++;

    }
    else return;


  }
  recipient = LoRa.read();          // recipient address
  sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    recFlag = true;
    delay(5);
    LoRa.beginPacket();
    LoRa.write(recipient);              // add destination address
    LoRa.write(sender);             // add sender address
    //LoRa.write(msgCount);                 // add message ID
    LoRa.write(outgoing.length());        // add payload length
    LoRa.endPacket();
    LoRa.print(outgoing);                 // add payload

    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));

  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();



}
void readRFID()
{
  
  rfid.PICC_ReadCardSerial();
  Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

    clearUID();
   
    Serial.println("Scanned PICC's UID:");
    printDec(rfid.uid.uidByte, rfid.uid.size);

    uidString = String(rfid.uid.uidByte[0])+" "+String(rfid.uid.uidByte[1])+" "+String(rfid.uid.uidByte[2])+ " "+String(rfid.uid.uidByte[3]);
    
    printUID();

    int i = 0;
    boolean match = true;
    while(i<rfid.uid.size)
    {
      if(!(rfid.uid.uidByte[i] == code[i]))
      {
           match = false;
      }
      i++;
    }

    if(match)
    {
      Serial.println("\nI know this card!");
      printUnlockMessage();
    }else
    {
      Serial.println("\nUnknown Card");
    }


    // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

  void clearUID()
  {
    display.setTextColor(BLACK); // or BLACK);
    display.setTextSize(1);
    display.setCursor(30,20); 
    display.print(uidString);
    display.display();
  }

  void printUID()
  {
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(1);
    display.setCursor(0,20); 
    display.print("UID: ");
    display.setCursor(30,20); 
    display.print(uidString);
    display.display();
  }

  void printUnlockMessage()
  {
    display.display();
    display.setTextColor(BLACK); // or BLACK);
    display.setTextSize(2);
    display.setCursor(10,0); 
    display.print("RFID Lock");
    display.display();
    
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(2);
    display.setCursor(10,0); 
    display.print("Unlocked");
    display.display();
    
    delay(2000);
    
    display.setTextColor(BLACK); // or BLACK);
    display.setTextSize(2);
    display.setCursor(10,0); 
    display.print("Unlocked");

    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(2);
    display.setCursor(10,0); 
    display.print("RFID Lock");
    display.display();
  }
