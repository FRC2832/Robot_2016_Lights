#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel eyes = Adafruit_NeoPixel(7, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel sides = Adafruit_NeoPixel(49, 9, NEO_GRB + NEO_KHZ800);

int const COLOR_GREEN = 70;
int const COLOR_BLUE = 170;
int const COLOR_RED = 0;
int const EYEPIXELCOUNT = 7;
int const SIDEPIXELCOUNT = 39;
int const CHARGETIME = 100; //centiseconds
int const DISCHARGETIME = 20; //same
int const BINARYATTEMPT = 0;
bool const INVERTBINARY = false;
bool const BINARYOUT = false;
bool const BINARYOVERIDE = false;
bool const BINARYSET[8] = {0, 1, 1, 0, 0, 0, 0};
INT32U const OURCANID = 0x02021451;
bool const ENABLECAN = true;
int const SPARKLERANGE[2] = {30, 40}; //SPARKLERANGE[Eye, Side]

int eyeLights[EYEPIXELCOUNT];
int eyeIncrement[EYEPIXELCOUNT];
int messageIncrement = 0;
int invertColor = 0;
int mesNum = 0;
int numCount = 0;
int eyePattern = 0;
int teamColour = COLOR_GREEN;
int sidePattern = 2;
int sideStage = -1;
int sideIncrement = 0;
float sensorValue;
int chargeLightsIncrement = 0;
int checkCount = 0;
int sinIncrement = 0;
int sinIncrementor = 1;
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
int sidePatternIncrementor = 1;
int sparkleIncrement[50][2] = {1}; //0 is eye, 1 is side
int sparkleValue[50][2] = {0};

MCP_CAN CAN(10);  //select pin 10

void setup() {
  sensorValue = analogRead(A0);
  Serial.begin(9600);
  srand((int)(analogRead(A0) * 1000));
  //Serial.println(rand() % 100);
  for (int i = 0; i < EYEPIXELCOUNT; i++) {
    sparkleValue[i][0] = (rand() % (SPARKLERANGE[0] - 1) + 1);
    sparkleIncrement[i][0] = 1;
    //Serial.println(sparkleValue[i][0]);
  }
  for (int i = 0; i < SIDEPIXELCOUNT; i++) {
    sparkleValue[i][1] = (rand() % (SPARKLERANGE[1] - 1) + 1);
    sparkleIncrement[i][1] = 1;
  }
  eyes.begin();
  sides.begin();
  if (ENABLECAN) {
START_INIT:

    if (CAN_OK == CAN.begin(CAN_1000KBPS))                  // init can bus : baudrate = 1000KB/s
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
}

void loop() {
  ctrl();
  currentTime = millis();
  if (currentTime > previousTime + 10) {
    previousTime = currentTime;
    if (messageIncrement >= 10) {
      messageIncrement = 0;
      //Serial.println(mesNum);
      //mesNum = 0;
      Serial.println();
      for (int i = 0; i < 8; i++) {
        Serial.print(binary[i]);
      }
    }

    messageIncrement++;
    sensorValue = analogRead(A0) * 0.0049 ;
    //Serial.println(sensorValue);
    //Serial.println(sidePattern);
    //ctrl();
    numCount++;
    if (numCount >= 50) {
      numCount = 0;
    }
    //Serial.println(sidePattern);
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
      case 7:
        sideAutonDefault();
        break;
      case 8:
        sideTeleopDefault();
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
    eyes.setPixelColor(i, Wheel(80 + Sparkle(i, 0)));
  }
}

void sideDisabled() {

}

int Sparkle(int i, int strip) {
  sparkleValue[i][strip] += sparkleIncrement[i][strip];
  if (sparkleValue[i][strip] >= SPARKLERANGE[strip] || sparkleValue[i][strip] <= 0) {
    sparkleIncrement[i][strip] *= -1;
  }
  //Serial.println(sparkleValue[1][strip]);
  //Serial.println(sparkleIncrement[1][strip]);
  return sparkleValue[i][strip] - SPARKLERANGE[strip] / 2;
}

void sideAutonDefault() {
  if (sideIncrement >= 2) {
    sidePatternIncrement += sidePatternIncrementor;
    if (sidePatternIncrement >= 25 || sidePatternIncrement <= 0) {
      sidePatternIncrementor *= -1;
    }
    for (int i = 0; i <= SIDEPIXELCOUNT / 2; i++) {
      if (i < sidePatternIncrement) {
        if (teamColour == COLOR_RED) {
          sides.setPixelColor(i, 0, 255 - 255 / 26 * i, 255 / 26 * i);
          sides.setPixelColor(SIDEPIXELCOUNT - i, 0, 255 - 255 / 26 * i, 255 / 26 * i);
        }
        else {
          sides.setPixelColor(i, 255 - 255 / 26 * i, 255 / 26 * i, 0);
          sides.setPixelColor(SIDEPIXELCOUNT - i, 255 - 255 / 26 * i, 255 / 26 * i, 0);
        }
      }
      else {
        sides.setPixelColor(i, 0, 0, 0);
        sides.setPixelColor(SIDEPIXELCOUNT - i, 0, 0, 0);
      }
    }
    sideIncrement = 0;
  }
  sideIncrement++;
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
  for (int i = 8; i < SIDEPIXELCOUNT; i++) {
    sides.setPixelColor(i, 0, 0, 0);
  }
}

void sideTeleopDefault() {
  if (sideIncrement >= 2) {
    sideIncrement = 0;
    sinIncrement += sinIncrementor;
    if (sinIncrement >= 19 || sinIncrement <= 0) {
      sinIncrementor *= -1;
    }
    for (int i = 0; i < SIDEPIXELCOUNT; i++) {
      sides.setPixelColor(i, Wheel(teamColour + (sinIncrement + (i % 10) - 30)));
      //Serial.println(sinIncrement);
    }
  }
  sideIncrement++;
}

void sideShoot() {

  switch (sideStage) {
    case -1:
      sideIncrement = 0;
      chargeLightsIncrement = 0;
      for (int i = 0; i < SIDEPIXELCOUNT; i++){
        sides.setPixelColor(i, 0, 0, 0) ;
      }
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
        //Serial.println("a");
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
  if (sideIncrement >= 3) {
    sideIncrement = 0;
    for (int i = 0; i < SIDEPIXELCOUNT; i++) {
      sides.setPixelColor(i, Wheel(teamColour + Sparkle(i, 1)));
    }
  }
  sideIncrement++;
}

void sideExpel() {
  if (sideIncrement >= 1) {
    for (int i = 0; i < SIDEPIXELCOUNT; i++) {
      if (i == SIDEPIXELCOUNT - chargeLightsIncrement) {
        sides.setPixelColor(i, Wheel(teamColour + Sparkle(i, 1)));
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
        sides.setPixelColor(i, Wheel(teamColour + Sparkle(i, 1)));
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

  if (ENABLECAN) {
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
      // check if data coming
      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
      canId = CAN.getCanId();
      //Serial.println(canId);

      //teleop/auton,red/blue,enable/disable,spin,shoot,expel,ingest,null
      if (canId == OURCANID) {
        //if( (canId & 0x003F) == 0x0010 ) {
        mesNum++;
        //Serial.println("message");
        for (int i = 7; i >= 0; i--)
        {
          binary[i] = (buf[i] != 0);
        }
        if (INVERTBINARY) {
          for (int i = 0; i <= 3; i++)
          {
            binary[i] = binary[7 - i];
          }
        }
      }
    }
  }
  if (BINARYOVERIDE) {
    memcpy(binary, BINARYSET, 8);
  }
  if (!BINARYOUT) {
    teleop = binary[0]; //teleop/auton

    if (binary[1]) { //red/blue
      teamColour = COLOR_RED; //red
    }
    else {
      teamColour = COLOR_BLUE; //blue
    }

    if (binary[2]) { //enable/disabled
      enabled = true;
    }
    else {
      sidePattern = 2;
      enabled = false;
      return;
    }

    if (binary[3]) { //spin
      //Serial.println("binary3");
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
        sidePattern = 8;
      }
      else {
        sidePattern = 7;
      }
    }
  }
  else {
    sidePattern = 6;
  }
}

