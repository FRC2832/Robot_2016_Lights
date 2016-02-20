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
	case 1:
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

