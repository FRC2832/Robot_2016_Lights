#include <Adafruit_NeoPixel.h>
#define PIXEL_PIN    6
#define PIXEL_COUNT 7
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN,
		NEO_GRB + NEO_KHZ800);
int incrementy = 0;
int pattern = 0;
int cycleNum[7] = { };
int increment[7] = { };

void setup() {
	Serial.begin(9600);
	strip.begin();
	strip.show();
	for (int i = 0; i <= 6; i++) {
		cycleNum[i] = rand() % 25;
	}
}

void loop() {
	switch (pattern) {
	case 0:
		pattern0(90, 68, 3); //Color range, start color, speed
		break;
	case 1:#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel eyes = Adafruit_NeoPixel(7, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel sides = Adafruit_NeoPixel(49, 9, NEO_GRB + NEO_KHZ800);

int const eyePixelCount = 7;
int const sidePixelCount = 49;
int const chargeTime = 100; //centiseconds
int const dischargeTime = 30; //same
int const twinkleMax = 100;
int const twinkleMin = 60;
boolean const debug = true;
boolean const demoOn = true;

long previousTime = 0;
long currentTime = 0;
int sideIncrement = 0;
int eyePattern = 0;
int sidePattern = 0;
int sideLights[sidePixelCount] = { };
int eyeLights[eyePixelCount] = { };
int eyeIncrement[eyePixelCount] = { };
int sideStage = -1;
int chargeLightsIncrement = 0;
int invertColor = 0;
int demoStage = 0;
int demoIncrement = 0;
boolean spinButton = false;
boolean fireButton = false;
boolean ingestButton = false;

void setup() {
  for (int i = 0; i < eyePixelCount; i++) {
    eyeIncrement[i] = 1;
    eyeLights[i] = twinkleMin + rand() % (twinkleMax - twinkleMin);
  }
  if (debug) {
    Serial.begin(9600);
  }
  eyes.begin();
  sides.begin();
}

void loop() {
  currentTime = millis();
  if (currentTime > previousTime + 10) {
    previousTime = currentTime;
    //Serial.println(chargeLightsIncrement);
    if (demoOn) {
      demo();
    }
    if (ingestButton) {
      sideStage = 3;
    }
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
    sides.show();
    eyes.show();
  }
}

void eyePattern0() {
  for (int i = 0; i < eyePixelCount; i++) {
    if (eyeLights[i] > twinkleMax || eyeLights[i] < twinkleMin) {
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
  
  if (spinButton == false) {
    sidePattern = 0;
  }
  switch (sideStage) {
    case -1:
      sideIncrement = 0;
      chargeLightsIncrement = 0;
      sideStage++;
      break;
    case 0:
      if (chargeLightsIncrement >= sidePixelCount) {
        sideIncrement = 0;
        chargeLightsIncrement = 0;
        sideStage++;
      }
      else {
        if (sideIncrement > chargeTime / sidePixelCount) {
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
        if (fireButton) {
          sideStage++;
        }
        sideIncrement = 0;
        invertColor = abs(invertColor - 255);
        for (int i = 0; i < sidePixelCount; i++) {
          sides.setPixelColor(i, invertColor, 0, 0);
        }
      }
      sideIncrement++;
      break;
    case 2:
            Serial.println(chargeLightsIncrement);
      if (chargeLightsIncrement >= sidePixelCount) {
        Serial.println("done");
        sideIncrement = 0;
        chargeLightsIncrement = 0;
        sideStage = 1;
      }
      else {
        if (sideIncrement > dischargeTime / sidePixelCount) {
          sides.setPixelColor(sidePixelCount - chargeLightsIncrement, 0, 0, 0);
          chargeLightsIncrement++;
          sideIncrement = 0;
        }
        sideIncrement++;
      }
      break;
    case 3:
    if (sideIncrement >= 50) {
        sideIncrement = 0;
      }
      sideIncrement++;
      break;
  }
}

void demo() {
  //Serial.println(demoIncrement);
  switch (demoStage) {
    case 0:
      if (demoIncrement >= 500) {
        spinButton = true;
        sidePattern = 1;
        demoStage++;
        demoIncrement = 0;
      }
      demoIncrement++;
      break;
    case 1:
      if (demoIncrement >= 500) {
        fireButton = true;
        demoStage++;
      }
      demoIncrement++;
      break;
      case 2:
       if (demoIncrement >= 50) {
        fireButton = false;
        demoStage++;
      }
      demoIncrement++;
      break;
    case 3:
      if (demoIncrement >= 500) {
        //ingestButton = true;
        demoStage++;
      }
      demoIncrement++;
      break;
    case 4:
      if (demoIncrement >= 500) {

      }
      demoIncrement++;
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
		pattern1(1);
	}
	strip.show();
}
void pattern0(int colorMax, int color, int speeed) {
	Serial.println(increment[0]);
	incrementy++;
	if (incrementy >= speeed) {
		for (int i = 0; i <= 6; i++) {
			strip.setPixelColor(i, Wheel((color + cycleNum[i]) % 255));
			cycleNum[i] = cycleNum[i] + increment[i];
			if (cycleNum[i] + color >= colorMax
					|| cycleNum[i] + color <= color) {
				increment[i] = increment[i] * -1;
			}
		}
		incrementy = 0;
	}
}
void pattern1(int value) {

}
uint32_t Wheel(byte WheelPos) {
	WheelPos = 255 - WheelPos;
	if (WheelPos < 85) {
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if (WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

