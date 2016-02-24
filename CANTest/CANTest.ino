#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel eyes = Adafruit_NeoPixel(7, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel sides = Adafruit_NeoPixel(49, 9, NEO_GRB + NEO_KHZ800);

int const EYEPIXELCOUNT = 7;
int const SIDEPIXELCOUNT = 49;
int const CHARGETIME = 100; //centiseconds
int const DISCHARGETIME = 30; //same
int const TWINKLEMAX = 100;
int const TWINKLEMIN = 60;
int const BINARYATTEMPT = 0;
bool const INVERTBINARY = false;

int eyeLights[EYEPIXELCOUNT];
int eyeIncrement[EYEPIXELCOUNT];
int invertColor = 0;
int eyePattern = 0;
int teamColour = 70; //green
int sidePattern = 2;
int sideStage = -1;
int sideIncrement = 0;
int chargeLightsIncrement = 0;
int checkCount = 0;
long currentTime;
long previousTime;
char buffer[456];
bool enabled = true;
bool fire = false;
bool binary[8];
bool teleop = true;

MCP_CAN CAN(10);  //select pin 10

void setup() {
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
    CAN.init_Filt(0, 0, 0x12c);
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
  currentTime = millis();
  if (currentTime > previousTime + 10) {
    previousTime = currentTime;
    ctrl();
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

void sideSpeed() {
  if (sideIncrement >= 99) {
    sideIncrement = 0;
  }
  sideIncrement++;
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
        sideStage++;
        sideIncrement = 0;
        invertColor = abs(invertColor - 255);
        for (int i = 0; i < SIDEPIXELCOUNT; i++) {
          sides.setPixelColor(i, invertColor, 0, 0);
        }
      }
      sideIncrement++;
      break;

    case 2:
      if ( !fire) {
        sideIncrement++;
      }
      break;

    case 3:
      //Serial.println(chargeLightsIncrement);
      if (chargeLightsIncrement >= SIDEPIXELCOUNT) {
        //Serial.println("done");
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

void sideColor() {
  for (int i = 0; i < SIDEPIXELCOUNT; i++) {
    sides.setPixelColor(i, Wheel(teamColour));
  }
}

void sideExpel() {
  if (sideIncrement >= 30) {
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
  if (sideIncrement >= 30)
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

  if (CAN_MSGAVAIL == CAN.checkReceive())           // check if data coming
  {
    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

    INT32U canId = CAN.getCanId();

    //teleop/auton,red/blue,enable/disable,spin,shoot,expel,ingest,null
    //Serial.println();
    if (canId == 0x12c)
    {
      /*
        if (BINARYATTEMPT == 0) {
        for (int i = 7; i >= 0; i--) {
          if (111 & (1 << i)) {
            //Serial.println("1");
            binary[i] = true;
          }
          else {
            //Serial.print("0");
            binary[i] = false;
          }
        }
        }
        else*/{
        for (int i = 7; i >= 0; i--) {
          binary[i] = (buf[i] != 0);
        }
      }

      if (INVERTBINARY) {
        for (int i = 0; i <= 3; i++) {
          binary[i] = binary[7 - i];
        }
      }
    }
    /*
        for (int i = 0; i <= 7; i++) {
          Serial.print( binary[i] );
        }//teleop/auton,red/blue,enable/disable,spin,shoot,expel,ingest,null
        Serial.println();
    */
    //Serial.println("message ");
    teleop = binary[0]; //teleop/auton

    if (binary[1]) { //red/blue
      teamColour = 0; //red
    }
    else {
      teamColour = 170; //blue
    }

//Serial.println();


    enabled = binary[2]; //enable/disable

    if (binary[3]) { //spin

    }

    fire = binary[4];  //shoot

    if (binary[5]) { //expell
      sidePattern = 3;
    }

    if (binary[6]) { //ingest
      sidePattern = 4;
    }

    checkCount = 0;
    for (int i = 3; i < 7; i++) {
      if (binary[i]) {
        checkCount++;
      }
    }
    if (checkCount != 0) {
      if (teleop) {
        sidePattern = 2;
      }
      else {
        sidePattern = 2;
      }
    }

  }
}


