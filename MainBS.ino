#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin
const int led1 = 4;
const int led2 = 5;
const int led3 = 6;
const int led4 = 7;
const int led5 = 8;
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
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  //while (!Serial);

  Serial.println("LoRa Duplex");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  //Serial.println("LoRa init succeeded.");
}





void loop() {
  sendMessage(node);
  lastSendTime = millis();
  sent = true;
  while (sent) {
    onReceive(LoRa.parsePacket());

    if (millis() - lastSendTime > interval) {
      sent = false;

      Serial.println("wait for response over");
      break;

    }



  }
  node++;
  if (node > 18)node = 0;

}





void sendMessage(int swit) {
  LoRa.beginPacket();                   // start packet
  outgoing = "data";
  switch (swit) {
    case 0:    // your hand is on the sensor
      //Serial.println("Node A");
      data = "Node-A ";
      LoRa.write(NodeID[1]);
      break;
    case 1:    // your hand is close to the sensor
      // Serial.println("Node B");
      data = "Node-B ";
      LoRa.write(NodeID[2]);
      break;
    case 2:    // your hand is a few inches from the sensor
      // Serial.println("Node C");
      data = "Node-C ";
      LoRa.write(NodeID[3]);
      break;
    case 3:    // your hand is nowhere near the sensor
      //Serial.println("Node D");
      data = "Node-D ";
      LoRa.write(NodeID[4]);
      break;
    case 4:    // your hand is on the sensor
      //Serial.println("Node E");
      data = "Node-E ";
      LoRa.write(NodeID[5]);
      break;
    case 5:    // your hand is close to the sensor
      //Serial.println("Node F");
      data = "Node-F ";
      LoRa.write(NodeID[6]);
      break;
    case 6:    // your hand is a few inches from the sensor
      //Serial.println("Node G");
      data = "Node-G ";
      LoRa.write(NodeID[7]);
      break;
    case 7:    // your hand is nowhere near the sensor
      //Serial.println("Node H");
      data = "Node-H ";
      LoRa.write(NodeID[8]);
      break;
    case 8:    // your hand is on the sensor
      //Serial.println("Node I");
      data = "Node-I ";
      LoRa.write(NodeID[9]);
      break;
    case 9:    // your hand is close to the sensor
      //Serial.println("Node J");
      data = "Node-J ";
      LoRa.write(NodeID[10]);
      break;
    case 10:    // your hand is a few inches from the sensor
      //Serial.println("Node K");
      data = "Node-K ";
      LoRa.write(NodeID[11]);
      break;
    case 11:    // your hand is nowhere near the sensor
      //Serial.println("Node L");
      data = "Node-L ";
      LoRa.write(NodeID[12]);
      break;
    case 12:    // your hand is on the sensor
      //Serial.println("Node M");
      data = "Node-M ";
      LoRa.write(NodeID[13]);
      break;
    case 13:    // your hand is close to the sensor
      //Serial.println("Node N");
      data = "Node-N ";
      LoRa.write(NodeID[14]);
      break;
    case 14:    // your hand is a few inches from the sensor
      //Serial.println("Node O");
      data = "Node-O ";
      LoRa.write(NodeID[15]);
      break;
    case 15:    // your hand is nowhere near the sensor
      //Serial.println("Node P");
      data = "Node-P ";
      LoRa.write(NodeID[16]);
      break;
    case 16:    // your hand is on the sensor
      //Serial.println("Node Q");
      data = "Node-Q ";
      LoRa.write(NodeID[17]);
      break;
    case 17:    // your hand is close to the sensor
      //Serial.println("Node R");
      data = "Node-R ";
      LoRa.write(NodeID[18]);
      break;
    case 18:    // your hand is a few inches from the sensor
      //Serial.println("Node S");
      data = "Node-S ";
      LoRa.write(NodeID[19]);
      break;
    case 19:    // your hand is nowhere near the sensor
      //Serial.println("Node T");
      data = "Node-T ";
      LoRa.write(NodeID[20]);
      break;
    case 20:    // your hand is on the sensor
      //Serial.println("Node U");
      data = "Node-U ";
      LoRa.write(NodeID[21]);
      break;

  }
  //LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  //LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  //msgCount++;                           // increment message ID
}


void Rssi() {
  int x = map(LoRa.packetRssi(), -25, -130, 255, 0);

  if (x > 204) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);

  }
  else if ((x <= 204) && (x > 153)) {

    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);

  }
  else if ((x <= 153) && (x > 102)) {

    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);

  }
  else if ((x <= 102) && (x > 51)) {

    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);

  }
  else if (x <= 51) {

    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
    digitalWrite(led5, LOW);

  }

}


void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
  byte ack = LoRa.read();
  Rssi();
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
    data += incoming + " " + "Length " + String(incomingLength) + "RSSI " + String(LoRa.packetRssi());
    // if message is for this device, or broadcast, print details:
    //    Serial.println("Received from: 0x" + String(sender, HEX));
    //    Serial.println("Sent to: 0x" + String(recipient, HEX));
    //    Serial.println("Message length: " + String(incomingLength));
    //    Serial.println("Message: " + incoming);
    //    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    //    Serial.println("Snr: " + String(LoRa.packetSnr()));

    Serial.println(data);
    delay(2000);
    sent = false;
    return;
  }
  else return;

}
