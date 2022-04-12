#include <SPI.h>              // include librariescfccccccc 
#include <LoRa.h>
#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
#define SS_PIN 3  //SS pin for Rfid (
#define RST_PIN 2 // Reset pin for rfid (Brown D2)
Adafruit_SSD1306 display(OLED_RESET);
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
SoftwareSerial mySerial(A0, A1); // RX, TX
String uidString="";
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
String outgoing;              // outgoing message
int recipient;
String localAddress = "A2";     // A1,A2,A3...An
String destination = "FF";      // destination to send to
byte ackF = 0xF1;
long lastSendTime = 0;        // last send time
long lastSendTime_gps=0;
int interval = 2000;          // interval between sends
int gps_interval=20000;
byte sender;
byte incomingLength ;

void setup() {
  Serial.begin(9600);                   // initialize serial
  mySerial.begin(9600);

  Serial.println("LoRa Duplex Node A");
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(434E6)) {             // initialize ratio at 433 MHz
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
  display.setTextSize(1);
  display.setCursor(10,0); 
  display.print("Scan Now");
  display.display();
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
  outgoing =destination+" " +localAddress+" " +"GPS "+ String(lat) + " " + String(lg);
  incomingLength = outgoing.length();
  Serial.println(outgoing);
  LoRa.beginPacket();
  //LoRa.write(ackF);
  //LoRa.write(destination);              // add destination address
  //LoRa.write(localAddress);             // add sender address
  //LoRa.write(incomingLength);        // add payload length
  LoRa.print(outgoing);// add payload
  LoRa.endPacket();
}
void send_RFID() {
  Receive_GPS_Data();
  finish = 0; pos_cnt = 0;
  outgoing=destination+" "+localAddress+" "+"RFID"+uidString+" "+String(lat) + " " +String(lg); //String(lat) + " " +String(lg)
  incomingLength=outgoing.length();
  Serial.println(outgoing);//adf
  LoRa.beginPacket();
  //LoRa.write(ackF);
  //LoRa.write(destination);              // add destination address
  //LoRa.write(localAddress);             // add sender address             // add message ID
  //LoRa.write(incomingLength);        // add payload length
  LoRa.print(outgoing);
  LoRa.endPacket();
  uidString="";
}

void onReceive(int packetSize) {
  String incoming = "";
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  //Serial.println(incoming);
  String src=getValue(incoming,' ',0);
  String dest=getValue(incoming,' ',1);
  String type=getValue(incoming,' ',2);
  if( src==localAddress){
    Serial.println(incoming);
    if (type=="RFID"){
      String uname=getValue(incoming,' ',3)+' '+ getValue(incoming,' ',4);
      String Bal="Balance : "+String(getValue(incoming,' ',5));
      Serial.println(Bal);
      clearUID(uname,5,2);
      printUID(Bal);}
    }
 // if (type=="User"){
   // clearUID("UserName",10,0);
  //  printUID(String(data));}
  
}

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
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

    clearUID("Scanning",10,0); 
    uidString = " "+String(rfid.uid.uidByte[0])+" "+String(rfid.uid.uidByte[1])+" "+String(rfid.uid.uidByte[2])+ " "+String(rfid.uid.uidByte[3]);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
  void clearUID(String text,int x,int y)
  {
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(1);
    display.setCursor(x,y); 
    display.print(text);
    display.display();
  }
  void printUID(String text)
  {
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(1);
    display.setCursor(5,20); 
    display.print(text);
    display.display();
  }

String getValue(String data, char separator, int index)
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
