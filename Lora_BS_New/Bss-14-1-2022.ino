#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin
int node = 0;
bool recFlag = false;
String outgoing;              // outgoing message
String data;
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xBB;     // address of this device
byte destination = 0xFF;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2500;          // interval between sends
byte ackF = 0xF1;
byte NodeID[22] = {0x01, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7};
bool sent = false;
bool waiting = false;





void setup() {
  Serial.begin(9600);                   // initialize serial
  //while (!Serial);

  Serial.println("LoRa Duplex");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

    byte incomingLength = LoRa.read();    // incoming msg length

    String incoming = "";

    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }

    if (incomingLength != incoming.length()) {   // check length for error
      Serial.println("error: message length does not match length");
      sent = false;
      return;
      // skip rest of function
    }
    data +=incoming + " " + "Length " + String(incomingLength) + "RSSI " + String(LoRa.packetRssi());
    // if message is for this device, or broadcast, print details:
    //    Serial.println("Received from: 0x" + String(sender, HEX));
    //    Serial.println("Sent to: 0x" + String(recipient, HEX));
    //    Serial.println("Message length: " + String(incomingLength));
    //    Serial.println("Message: " + incoming);
    //    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    //    Serial.println("Snr: " + String(LoRa.packetSnr()));

    Serial.println(data);
    data="";
    
    return;
  //Serial.println("LoRa init succeeded.");
}





void loop() {
    onReceive(LoRa.parsePacket());
    
    
  }





void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  
  //LoRa.write(destination);              // add destination address
  LoRa.write(NodeID[1]);
  LoRa.write(localAddress);             // add sender address
  //LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  //msgCount++;                           // increment message ID
}


void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
  byte ack = LoRa.read();

  if (ack == ackF) {
    // read packet header bytes:
    // Serial.println("Correct Node");
    int recipient = LoRa.read();          // recipient address
    byte sender = LoRa.read();            // sender address
    // byte incomingMsgId = LoRa.read();     // incoming msg ID
    byte incomingLength = LoRa.read();    // incoming msg length

    String incoming = "";

    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }

    if (incomingLength != incoming.length()) {   // check length for error
      Serial.println("error: message length does not match length");
      sent = false;
      return;
      // skip rest of function
    }
    data +=String(sender,HEX)+" "+ incoming + " " + "Length " + String(incomingLength) + "RSSI " + String(LoRa.packetRssi());
    // if message is for this device, or broadcast, print details:
    //    Serial.println("Received from: 0x" + String(sender, HEX));
    //    Serial.println("Sent to: 0x" + String(recipient, HEX));
    //    Serial.println("Message length: " + String(incomingLength));
    //    Serial.println("Message: " + incoming);
    //    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    //    Serial.println("Snr: " + String(LoRa.packetSnr()));

    Serial.println(data);
    data="";
    delay(10);
    String Header=split_String(incoming,' ',0);
    Serial.println(Header);
    if (Header=="GPS"){
      sendMessage("I Recieved GPS Packet");
    }
    else if (Header=="RFID"){
      sendMessage("Mohit Sonesh");
    }
    
    return;
  }
  else return;

}
String split_String(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
