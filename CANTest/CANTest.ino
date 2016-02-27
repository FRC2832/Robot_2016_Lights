#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel eyes = Adafruit_NeoPixel(7, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel sides = Adafruit_NeoPixel(49, 9, NEO_GRB + NEO_KHZ800);

int const EYEPIXELCOUNT = 7;
int const SIDEPIXELCOUNT = 49;
int const CHARGETIME = 100; //centiseconds
int const DISCHARGETIME = 20; //same
int const TWINKLEMAX = 100;
int const TWINKLEMIN = 60;
int const BINARYATTEMPT = 0;
bool const INVERTBINARY = false;
bool const BINARYOUT = false;
bool const BINARYOVERIDE = false;
bool const BINARYSET[8] = {1, 0, 1, 0, 0, 0, 0};
INT32U const OURCANID = 0x02021450;

int eyeLights[EYEPIXELCOUNT];
int eyeIncrement[EYEPIXELCOUNT];
int invertColor = 0;
int mesNum = 0;
int numCount = 0;
int eyePattern = 0;
int teamColour = 70; //green
int sidePattern = 2;
int sideStage = -1;
int sideIncrement = 0;
int sideInvert = 1;
float sensorValue;
int chargeLightsIncrement = 0;
int checkCount = 0;
long currentTime;
long previousTime;
char buffer[456];
bool enabled = true;
bool fire = false;
bool binary[8];
bool teleop = true;
INT32U canId = 0;
bool sideInit = false;
int sidePatternIncrement = 0;

MCP_CAN CAN(10);  //select pin 10

void setup() {
  sensorValue = analogRead(A0);
  Serial.begin(9600);
  for (int i = 0; i < EYEPIXELCOUNT; i++) {
    eyeIncrement[i] = 1;
    eyeLights[i] = TWINKLEMIN + (rand() % (TWINKLEMAX - TWINKLEMIN));
  }
  eyes.begin();
  sides.begin();
START_INIT:

  if (CAN_OK == CAN.begin(CAN_1000KBPS))                  // init can bus : baudrate = 1000k
  {
    CAN.init_Filt(0, 1, OURCANID);
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
  ctrl();
  currentTime = millis();
  if (currentTime > previousTime + 10) {
    previousTime = currentTime;
    sensorValue = analogRead(A0) * 0.0049 ;
    //Serial.println(sensorValue);
    //Serial.println(sidePattern);
    //ctrl();
    numCount++;
    if (numCount >= 50) {
       Serial.println(sidePattern);
      numCount = 0;
    }
    switch (eyePattern) {
      case 0:
        eyeTwinkle();
        break;
    }
    switch (sidePattern) {
      case 0:
        sideSpeed();
        break;

      case 1:
        sideShoot();
        break;

      case 2:
        sideColor();
        break;

      case 3:
        sideExpel();
        break;

      case 4:
        sideIngest();
        break;

      case 5:
        sideDisabled();
        break;

      case 6:
        sideBinary();
        break;
    }

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

void eyeTwinkle() {
  for (int i = 0; i < EYEPIXELCOUNT; i++) {
    if (eyeLights[i] > TWINKLEMAX || eyeLights[i] < TWINKLEMIN) {
      //Serial.println(eyeLights[i]);
      eyeIncrement[i] = eyeIncrement[i] * -1;
      //Serial.println(eyeIncrement[0]);
    }
    eyes.setPixelColor(i, Wheel(eyeLights[i] % 255));
    eyeLights[i] = eyeLights[i] + eyeIncrement[i];
  }
}

void sideDisabled() {
  /*
  if (sideIncrement >= 1) {
    sidePatternIncrement = sidePatternIncrement + sidePattern;
    if (sidePatternIncrement >= EYEPIXELCOUNT || sidePatternIncrement < 0) {
      sidePattern = sidePattern * -1;
    }
    for (int i = 0; i < EYEPIXELCOUNT / 2; i++) {
      if (i == sidePatternIncrement) {
        sides.setPixelColor(i, Wheel(teamColour));
      }
      else {
        sides.setPixelColor(i, 0, 0, 0);
      }
      if (i == EYEPIXELCOUNT - sidePatternIncrement) {
        sides.setPixelColor(i, Wheel(teamColour));
      }
      else {
        sides.setPixelColor(i, 0, 0, 0);
      }
    }
    sideIncrement = 0;
  }
  sideIncrement++;
  */
  for (int i = 0; i < EYEPIXELCOUNT; i++) {
    sides.setPixelColor(i, Wheel(70));    
  }
}

void sideSpeed() {
  if (sideIncrement >= 99) {
    sideIncrement = 0;
  }
  sideIncrement++;
}

void sideBinary() {
  for (int i = 0; i < 8; i++) {
    if (binary[i]) {
      sides.setPixelColor(i, Wheel(170));
    }
    else {
      sides.setPixelColor(i, 0, 0, 0);
    }
  }
  for (int i = 8; i < 49; i++) {
    sides.setPixelColor(i, 0, 0, 0);
  }
}

void sideShoot() {

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
          sides.setPixelColor(chargeLightsIncrement, Wheel(teamColour));
          chargeLightsIncrement++;
          sideIncrement = 0;
          invertColor = 0;
        }
        sideIncrement++;
      }
      break;

    case 1:
      if (fire) {
        sideStage++;
      }
      break;

    case 2:
      //Serial.println(chargeLightsIncrement);
      if (chargeLightsIncrement >= SIDEPIXELCOUNT) {
        //Serial.println("done");
        sideIncrement = 0;
        chargeLightsIncrement = 0;
      }
      else {
        Serial.println("a");
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

void sideColor() {
  for (int i = 0; i < SIDEPIXELCOUNT; i++) {
    sides.setPixelColor(i, Wheel(teamColour));
  }
}

void sideExpel() {
  if (sideIncrement >= 1) {
    for (int i = 0; i < SIDEPIXELCOUNT; i++) {
      if (i == SIDEPIXELCOUNT - chargeLightsIncrement) {
        sides.setPixelColor(i, Wheel(teamColour));
      }
      else {
        sides.setPixelColor(i, 0, 0, 0);
      }
    }
    if (chargeLightsIncrement >= SIDEPIXELCOUNT) {
      chargeLightsIncrement = 0;
    }
    chargeLightsIncrement++;
    sideIncrement = 0;
  }
  sideIncrement++;
}

void sideIngest() {
  if (sideIncrement >= 1)
  {
    for (int i = 0; i < SIDEPIXELCOUNT; i++)
    {
      if (i == chargeLightsIncrement)
      {
        sides.setPixelColor(i, Wheel(teamColour));
      }
      else
      {
        sides.setPixelColor(i, 0, 0, 0);
      }
    }

    if (chargeLightsIncrement >= SIDEPIXELCOUNT) {
      chargeLightsIncrement = 0;
    }
    chargeLightsIncrement++;
    sideIncrement = 0;
  }
  sideIncrement++;
}

void ctrl() {

  unsigned char len = 0;
  unsigned char buf[8];

  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    // check if data coming
    if (!BINARYOVERIDE) {
      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
      canId = CAN.getCanId();
      //Serial.println(canId);
    }
    //teleop/auton,red/blue,enable/disable,spin,shoot,expel,ingest,null

    if (canId == OURCANID || BINARYOVERIDE) {
      //if( (canId & 0x003F) == 0x0010 ) {
      mesNum++;
      //Serial.println("message");
      for (int i = 7; i >= 0; i--)
      {
        binary[i] = (buf[i] != 0);
      }

      if (BINARYOVERIDE) {
        memcpy(binary, BINARYSET, 8);
      }

      if (INVERTBINARY) {
        for (int i = 0; i <= 3; i++)
        {
          binary[i] = binary[7 - i];
        }
      }
      if (!BINARYOUT) {
        teleop = binary[0]; //teleop/auton

        if (binary[1]) { //red/blue
          teamColour = 0; //red
        }
        else {
          teamColour = 170; //blue
        }

        if (binary[2]) { //enable/disabled
          enabled = true;
        }
        else {
          sidePattern = 5;
          enabled = false;
          return;
        }

        if (binary[3]) { //spin
          Serial.println("binary3");
          sidePattern = 1;
          if (!sideInit) {
            sideStage = -1;
            sideInit = true;
          }
          //return;
        }
        else {
          sideInit = false;
        }

        fire = binary[4];  //shoot

        if (binary[5]) { //expell
          sidePattern = 3;
          return;
        }

        if (binary[6]) { //ingest
          sidePattern = 4;
          return;
        }

        checkCount = 0;
        for (int i = 3; i < 7; i++) {
          if (binary[i]) {
            checkCount++;
          }
        }

        if (checkCount == 0 && enabled) {
          if (teleop) {
            sidePattern = 2;
          }
          else {
            sidePattern = 2;
          }
        }
      }
      else {
        sidePattern = 6;
      }
    }
  }
}

