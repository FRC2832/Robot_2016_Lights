#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel eyes = Adafruit_NeoPixel(7, 7, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel sides = Adafruit_NeoPixel(49, 6, NEO_GRB + NEO_KHZ800);

int const EYEPIXELCOUNT = 7;
int const SIDEPIXELCOUNT = 49;
int const CHARGETIME = 100; //centiseconds
int const DISCHARGETIME = 30; //same
int const TWINKLEMAX = 100;
int const TWINKLEMIN = 60;

int eyeLights[EYEPIXELCOUNT];
int eyeIncrement[EYEPIXELCOUNT];
int invertColor = 0;
int eyePattern = 0;
int sidePattern = 0;
int sideStage = 0;
int sideIncrement = 0;
int chargeLightsIncrement = 0;
long currentTime;
long previousTime;
char buffer[456];
bool fireTrue = false;

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

    switch (eyePattern) {
      case 0:
        eyePattern0();
        break;
    }
    switch (sidePattern) {
      case 0:
        sidePattern0();
        break;
      case 1:
        sidePattern1();
        break;
    }


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

void eyePattern0() {
  for (int i = 0; i < EYEPIXELCOUNT; i++) {
    if (eyeLights[i] > TWINKLEMAX || eyeLights[i] < TWINKLEMIN) {
      eyeIncrement[i] = eyeIncrement[i] * -1;
    }
    eyes.setPixelColor(i, Wheel(eyeLights[i] % 255));
    eyeLights[i] = eyeLights[i] + eyeIncrement[i];
  }
}

void sidePattern0() {
  if (sideIncrement >= 99) {
    sideIncrement = 0;
  }
  sideIncrement++;
}

void sidePattern1() {

  switch (sideStage) {
    case -1:
      sideIncrement = 0;
      chargeLightsIncrement = 0;
      sideStage++;
      break;
    case 0:
      if (chargeLightsIncrement >= SIDEPIXELCOUNT) {
        sideIncrement = 0;
        chargeLightsIncrement = 0;
        sideStage++;
      }
      else {
        if (sideIncrement > CHARGETIME / SIDEPIXELCOUNT) {
          sides.setPixelColor(chargeLightsIncrement, Wheel(0));
          chargeLightsIncrement++;
          sideIncrement = 0;
          invertColor = 0;
        }
        sideIncrement++;
      }
      break;
    case 1:
      if (sideIncrement >= 50) {
        if (fireTrue) {
          sideStage++;
        }
        sideIncrement = 0;
        invertColor = abs(invertColor - 255);
        for (int i = 0; i < SIDEPIXELCOUNT; i++) {
          sides.setPixelColor(i, invertColor, 0, 0);
        }
      }
      sideIncrement++;
      break;

    case 2:
      if ( !fireTrue) {
        sideIncrement++;
      }
      break;

    case 3:
      Serial.println(chargeLightsIncrement);
      if (chargeLightsIncrement >= SIDEPIXELCOUNT) {
        Serial.println("done");
        sideIncrement = 0;
        chargeLightsIncrement = 0;
        sideStage = 1;
      }
      else {
        if (sideIncrement > DISCHARGETIME / SIDEPIXELCOUNT) {
          sides.setPixelColor(SIDEPIXELCOUNT - chargeLightsIncrement, 0, 0, 0);
          chargeLightsIncrement++;
          sideIncrement = 0;
        }
        sideIncrement++;
      }
      break;
  }
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
