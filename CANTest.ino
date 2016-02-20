#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

Adafruit_NeoPixel eyes = Adafruit_NeoPixel(7, 7, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel sides = Adafruit_NeoPixel(49, 6, NEO_GRB + NEO_KHZ800);


int const EYEPIXELCOUNT = 7;
int const SIDEPIXELCOUNT = 49;
int const CHARGETIME = 100; //centiseconds
int const DISCHARGETIME = 30; //same
int const TWINKLEMAX = 100;
int const TWINKLEMIN = 60;

long currentTime;
long previousTime;
char buffer[456];
MCP_CAN CAN(10);  //select pin 10

void setup() {
  Serial.begin(9600);
START_INIT:

  if (CAN_OK == CAN.begin(CAN_1000KBPS))                  // init can bus : baudrate = 1000k
  {
      //CAN.init_Filt(0, 0, 0x12c);
    Serial.println("CAN BUS Shield init ok!");
  }
  else
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
    goto START_INIT;
  }

}

void loop() {
  Serial.println(currentTime);
  currentTime = millis();
  if (currentTime > previousTime + 10) {
    previousTime = currentTime;

    ctrl();

    sides.show();
    eyes.show();
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return eyes.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;

    return eyes.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return eyes.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void ctrl() {

  unsigned char len = 0;
  unsigned char buf[8];

  if (CAN_MSGAVAIL == CAN.checkReceive())           // check if data coming
  {
    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf


    INT32U canId = CAN.getCanId();


    Serial.println("-----------------------------");
    Serial.println("get data from ID: ");
    Serial.println(canId);
    Serial.println("len = ");
    Serial.println(len);


    for (int i = 0; i < len; i++) // print the data
    {
      Serial.print(buf[i]);
      Serial.print("\t");

    }
    Serial.println();
  }

}
