#include <SPI.h>              // include librariescfccccccc 
#include <LoRa.h>
#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define SS_PIN 3  //SS pin for Rfid (
#define RST_PIN 2 // Reset pin for rfid (Brown D2)
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
int code[] = {69,141,8,136}; //This is the stored UID
int codeRead = 0;
String uidString="";

SoftwareSerial mySerial(A0, A1); // RX, TX

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 1;         // change for your board; must be a hardware interrupt pin

int Gpsdata;             // for incoming serial data
unsigned int finish = 0; // indicate end of message
unsigned int pos_cnt = 0; // position counter
unsigned int lat_cnt = 0; // latitude data counter
unsigned int log_cnt = 0; // longitude data counter
unsigned int flg    = 0; // GPS flag
unsigned int com_cnt = 0; // comma counter
char lat[10];            // latitude array original=20
char lg[10];             // longitude array original=20

void Receive_GPS_Data();
float t ;
String outgoing;              // outgoing message
int recipient;
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xA2;     // address of this device
byte destination = 0xFF;      // destination to send to
byte ackF = 0xF1;
long lastSendTime = 0;        // last send time
long lastSendTime_gps=0;
int interval = 2000;          // interval between sends
int gps_interval=20000;
byte sender;
bool recFlag = false;
byte incomingLength ;
int recipientlast;
byte senderlast;


void setup() {
  Serial.begin(9600);                   // initialize serial
  mySerial.begin(9600);

  Serial.println("LoRa Duplex Node A");
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothin]g
  }
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);

  // Clear the buffer.
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE); // or BLACK);
  display.setTextSize(2);
  display.setCursor(10,0); 
  display.print("Scan Now");
  display.display();
  // Serial.println("LoRa init succeeded.");
}




void loop() {
  onReceive(LoRa.parsePacket());
  if(  rfid.PICC_IsNewCardPresent())
  {
      readRFID();
      if (uidString!="");
        send_RFID();
      
  }
  
  if (millis() - lastSendTime_gps > gps_interval){
    send_GPS_Data();
    lastSendTime_gps=millis();
    }

}

void  send_GPS_Data(){
  Receive_GPS_Data();
  finish = 0; pos_cnt = 0;
  outgoing ="GPS  Lat  "+ String(lat) + " " + "Lg:" + String(lg);
  incomingLength = outgoing.length();
  Serial.println(outgoing);
  LoRa.beginPacket();
    //int ack =12345678;
  LoRa.write(ackF);
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
    // LoRa.write(incomingMsgId);                 // add message ID
  LoRa.write(incomingLength);        // add payload length
  LoRa.print(outgoing);// add payload
  LoRa.endPacket();
}
void send_RFID() {
  outgoing="RFID"+uidString;
  incomingLength=outgoing.length();
  Serial.println(outgoing);//adf
  LoRa.beginPacket();
    //int ack =12345678;
  LoRa.write(ackF);
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
    // LoRa.write(incomingMsgId);                 // add message ID
  LoRa.write(incomingLength);        // add payload length
  LoRa.print(outgoing);
  LoRa.endPacket();
  uidString="";
}


void onReceive(int packetSize) {

  if (packetSize == 0) return;
  Serial.println("received");
  if (recFlag == true) {


    byte ack = LoRa.read();
    if (ack == ackF) {
      recipient = LoRa.read();          // recipient address
      sender = LoRa.read();            // sender address
      //byte incomingMsgId = LoRa.read();     // incoming msg ID
      byte incomingLength = LoRa.read();    // incoming msg length

      String incoming = "";

      while (LoRa.available()) {
        incoming += (char)LoRa.read();
      }

      if (incomingLength != incoming.length()) {   // check length for error
        Serial.println("error: message length does not match length");
        recFlag = false;
        return;
        // skip rest of function
      }

      delay(5);
      LoRa.beginPacket();                   // start packet
      LoRa.write(ackF);
      LoRa.write(recipient);              // add destination address
      LoRa.write(sender);             // add sender address
      //      LoRa.write(incomingMsgId);                 // add message ID
      LoRa.write(incomingLength);        // add payload length
      LoRa.print(incoming);                 // add payload
      LoRa.endPacket();                     // finish packet and send it
      recFlag = false;

    }
    else return;
  }

  recipient = LoRa.read();          // recipient address
  if (recipient == ackF) {
    delay(1000);
    return;
  }
  sender = LoRa.read();            // sender address

  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {
    Serial.println("error: message length does not match length");
    recFlag = false;
    return;
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    recFlag = true;
    lastSendTime = millis();
    delay(10);
    LoRa.beginPacket();
    LoRa.write(recipient);              // add destination address
    LoRa.write(sender);             // add sender address
    // LoRa.write(incomingMsgId);                 // add message ID
    LoRa.write(incomingLength);        // add payload length
    LoRa.print(incoming);                 // add payload
    LoRa.endPacket();
    Serial.println("forward");
    return;                             // skip rest of function
  }
  else if (recipient == localAddress) {
      clearUID("UserName",10,0);
      printUID(String(incoming));
      
      Serial.println("Reply from: 0x"+String(sender,HEX)+" " + String(incoming));

  }
}



/************************************************************************************
  Function    : Receive_GPS_Data()
  Description : finding Latitudse and longitude from GPRMC message
************************************************************************************/
void Receive_GPS_Data()
{
  while (finish == 0) {
    while (mySerial.available() > 0) {     // Check GPS data
      Gpsdata = mySerial.read();
      flg = 1;
      if ( Gpsdata == '$' && pos_cnt == 0) // finding GPRMC header
        pos_cnt = 1;
      if ( Gpsdata == 'G' && pos_cnt == 1)
        pos_cnt = 2;
      if ( Gpsdata == 'P' && pos_cnt == 2)
        pos_cnt = 3;
      if ( Gpsdata == 'R' && pos_cnt == 3)
        pos_cnt = 4;
      if ( Gpsdata == 'M' && pos_cnt == 4)
        pos_cnt = 5;
      if ( Gpsdata == 'C' && pos_cnt == 5 )
        pos_cnt = 6;
      if (pos_cnt == 6 &&  Gpsdata == ',') { // count commas in message
        com_cnt++;
        flg = 0;
      }

      if (com_cnt == 3 && flg == 1) {     //com_cnt==3
        lat[lat_cnt++] =  Gpsdata;         // latitude
        flg = 0;
      }

      if (com_cnt == 5 && flg == 1) {     //com_cnt==5
        lg[log_cnt++] =  Gpsdata;         // Longitude
        flg = 0;
      }

      if ( Gpsdata == '*' && com_cnt >= 5) {
        com_cnt = 0;                      // end of GPRMC message
        lat_cnt = 0;
        log_cnt = 0;
        flg     = 0;
        finish  = 1;

      }
    }
  }
}

void readRFID()
{
  
  rfid.PICC_ReadCardSerial();
  //Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  //Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

    clearUID("Scanning",10,0);
   
    //Serial.println("Scanned PICC's UID:");
    //printDec(rfid.uid.uidByte, rfid.uid.size);

    uidString = " "+String(rfid.uid.uidByte[2])+" "+String(rfid.uid.uidByte[3]);//+" "+String(rfid.uid.uidByte[2])+ " "+String(rfid.uid.uidByte[3]);
    //printUID(uidString);

    // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

//void printDec(byte *buffer, byte bufferSize) {
 // for (byte i = 0; i < bufferSize; i++) {
    //Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    //Serial.print(buffer[i], DEC);
 // }
//}

  void clearUID(String text,int x,int y)
  {
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(2);
    display.setCursor(x,y); 
    display.print(text);
    display.display();
  }

  void printUID(String text)
  {
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(1);
    display.setCursor(30,20); 
    display.print(text);
    display.display();
  }
