/**
 * This is the more completely documented example! (see below)
 */

/* ======================= includes ================================= */

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <Wire.h>
#include "RTClib.h"

// How many NeoPixels are attached to the Arduino?

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.

void colorAll(uint32_t c, uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
uint32_t Wheel(byte WheelPos);
void heartShape(uint8_t wait);
void timeDisplay(uint8_t h, uint8_t m);

/* ======================= extra-examples.cpp ======================== */


// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_COUNT 110
#define PIXEL_PIN 6
#define PIXEL_TYPE WS2812B2

// Parameter 1 = number of pixels in strip
//               note: for some stripes like those with the TM1829, you
//                     need to count the number of segments, i.e. the
//                     number of controllers in your stripe, not the number
//                     of individual LEDs!
//
// Parameter 2 = pin number (if not specified, D2 is selected for you)
//
//               On Photon, Electron, P1, Core and Duo, any pin can be used for Neopixel.
//
//               On the Argon, Boron and Xenon, only these pins can be used for Neopixel:
//               - D2, D3, A4, A5
//               - D4, D6, D7, D8
//               - A0, A1, A2, A3
//
//               In addition on the Argon/Boron/Xenon, only one pin per group can be used
//               at a time. So it's OK to have one Adafruit_NeoPixel instance on pin D2 and
//               another one on pin A2, but it's not possible to have one on pin A0 and
//               another one on pin A1.
//
// Parameter 3 = pixel type [ WS2812, WS2812B, WS2812B2, WS2813, WS2811,
//                            TM1803, TM1829, SK6812RGBW, WS2812B_FAST,
//                            WS2812B2_FAST ]
//               note: if not specified, WS2812B is selected for you which
//                     is the same as WS2812 or WS2813 in operation.
//               note: RGB order is automatically applied to WS2811,
//                     WS2812/WS2812B/WS2812B2/WS2813/TM1803 is GRB order.
//               note: For legacy 50us reset pulse timing on WS2812/WS2812B
//                     or WS2812B2, select WS2812B_FAST or WS2812B2_FAST
//                     respectively.  Otherwise 300us timing will be used.
//
// 800 KHz bitstream 800 KHz bitstream (most NeoPixel products
//               WS2812/WS2813 (6-pin part)/WS2812B (4-pin part)/SK6812RGBW (RGB+W) )
//
// 400 KHz bitstream (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//                   (Radio Shack Tri-Color LED Strip - TM1803 driver
//                    NOTE: RS Tri-Color LED's are grouped in sets of 3)

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


/* ---------- Clock part ---------- */
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


/* ---------- Button part ---------- */
// this constant won't change:
const int  btnPin1 = 2;    // the pin that the pushbutton is attached to
const int  btnPin2 = 3;    // the pin that the pushbutton is attached to
const int  btnPin3 = 4;    // the pin that the pushbutton is attached to
volatile int state = 0;

// Variables will change:
int btnPushCount1 = 0;   // counter for the number of button presses
int btnPushCount2 = 0;   // counter for the number of button presses
int btnPushCount3 = 0;   // counter for the number of button presses
int btnState1 = 0;         // current state of the button
int btnState2 = 0;         // current state of the button
int btnState3 = 0;         // current state of the button
int lastBtnState1 = 0;     // previous state of the button
int lastBtnState2 = 0;     // previous state of the button
int lastBtnState3 = 0;     // previous state of the button

int brightness [] = {10, 50, 90};
int color[4][3] = { { 255, 255, 255 },
                    { 255, 128, 0 },
                    { 64, 255, 64 },
                    { 32, 64, 255 } };
int timeResetStep = 0;
int snowstep = 0; // timesteps for the snowing animation
void setup() {

  // initialize the button pin as a input:
  pinMode(btnPin1, INPUT_PULLUP);
  pinMode(btnPin2, INPUT_PULLUP);
  pinMode(btnPin3, INPUT_PULLUP);

  
  Serial.begin(9600);
  
  strip.begin();
  strip.setBrightness(brightness[state]); //max is 255
  strip.show(); // Initialize all pixels to 'off'
  colorWipe(strip.Color(0, 0, 50), 30);
  
  //delay(3000); // wait for console opening
 
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    rtc.adjust(DateTime(2019, 07, 06, 8, 43, 0));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  //rtc.adjust(DateTime(2019, 07, 07, 21, 15, 20));

}

void loop() {
  btnState1 = digitalRead(btnPin1);
  if (btnState1 != lastBtnState1) {
    if (btnState1 == HIGH) {
      btnPushCount1++;
      blink();
      Serial.println("btn1");
      Serial.print("number of button pushes: ");
      Serial.println(btnPushCount1);
    }
    // Delay a little bit to avoid bouncing
    delay(50);
    lastBtnState1 = btnState1;
  }

 
  btnState2 = digitalRead(btnPin2);
  if (btnState2 != lastBtnState2) {
    if (btnState2 == HIGH) {
      btnPushCount2++;
      adjustTime(5);
      Serial.println("btn2");
      Serial.print("number of button pushes: ");
      Serial.println(btnPushCount2);
    }
    // Delay a little bit to avoid bouncing
    delay(50);
    lastBtnState2 = btnState2;
  }

  btnState3 = digitalRead(btnPin3);
  if (btnState3 != lastBtnState3) {
    if (btnState3 == HIGH) {
      btnPushCount3++;
      adjustTime(-5);
      Serial.println("btn3");
      Serial.print("number of button pushes: ");
      Serial.println(btnPushCount3);
    }
    // Delay a little bit to avoid bouncing
    delay(50);
    lastBtnState3 = btnState3;
  }

 
  if (state == 12) {
    strip.setBrightness(40);
    rainbowCycle(5);
  } else if (state == 13) {
    strip.setBrightness(100);
    heartShape();
  } else if (state == 14) {
    strip.setBrightness(40);
    snow();
  } else if (state == 15) {
    strip.setBrightness(20);
    colorAll(strip.Color(100, 255, 100), 10);
  } else {
    DateTime now = rtc.now();
    if (now.second()%2 == 0) {
      if (timeResetStep == 1) return;
      timeResetStep = 1;
      timeDisplay(now.hour(), now.minute());
      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print(" (");
      Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
      Serial.print(") ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();
    
      /*Serial.print(" since midnight 1/1/1970 UNIXTIME = ");
      Serial.print(now.unixtime());
      Serial.print("s");
      Serial.println();*/
    } else {
      timeResetStep = 0;
    }
  }


  
  // Some example procedures showing how to display to the pixels:
  // Do not run more than 15 seconds of these, or the b/g tasks
  // will be blocked.
  //--------------------------------------------------------------

  //strip.setPixelColor(0, strip.Color(50, 0, 0));
  //strip.show();

  //colorWipe(strip.Color(255, 0, 0), 1000); // Red

  //colorWipe(strip.Color(0, 255, 0), 1000); // Green

  //colorWipe(strip.Color(0, 0, 255), 1000); // Green

  //rainbow(20);

  //rainbowCycle(5);

  //colorAll(strip.Color(0, 0, 255), 50); // Cyan
  
  //heartShape(); delay(5000);

}

// Set all pixels in the strip to a solid color, then wait (ms)
void colorAll(uint32_t c, uint8_t wait) {
  uint16_t i;

  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

// Fill the dots one after the other with a color, wait (ms) after each one
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(109-i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout, then wait (ms)
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
    btnState1 = digitalRead(btnPin1);
    if (btnState1 != lastBtnState1) {
      if (btnState1 == HIGH) {
        btnPushCount1++;
        state++;
        Serial.println("btn1");
        Serial.print("number of button pushes: ");
        Serial.println(btnPushCount1);
      }
      // Delay a little bit to avoid bouncing
      delay(50);
      lastBtnState1 = btnState1;
      break;
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void heartShape() {
  int shape [] = {0,0,1,1,0,0,0,1,1,0,0,0,1,0,0,1,0,1,0,0,1,0,1,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0};
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if (shape[i] == 0) {
      strip.setPixelColor(109-i, strip.Color(0, 0, 0));
    } else {
      strip.setPixelColor(109-i, strip.Color(255, 0, 0));
    }
  }
  strip.show();
}

void snow() {
  int start [] = {3, 7, 4, 0, 2, 5, 8, 1, 6, 10, 9};
  for (int i=0; i<11; i++) {
    for (int m=0; m<5; m++) {
      int j = -66*i + start[i] + (snowstep-i-m)*11 + (snowstep-i-m)%2*(5-start[i])*2;
      for (int k=0; k<10; k++) { // 10 would make the animation end before beginning again
        if (j>=0 && j<110) { strip.setPixelColor(109-j, strip.Color( 245/(m+1)-245/5,  245/(m+1)-245/5, 255/(m+1)-255/5)); }
        j -= 594;
      }
    }
  }
  snowstep++;
  if (snowstep > 570) { snowstep = 0; Serial.print("most!!!!!!!!"); }
  strip.show();
  //Serial.print(snowstep);Serial.println();
  delay(50);
  btnState1 = digitalRead(btnPin1);
  if (btnState1 != lastBtnState1) {
    if (btnState1 == HIGH) {
      btnPushCount1++;
      state++;
      Serial.println("btn1");
      Serial.print("number of button pushes: ");
      Serial.println(btnPushCount1);
    }
    // Delay a little bit to avoid bouncing
    delay(50);
    lastBtnState1 = btnState1;
  }
}

void timeDisplay(uint8_t h, uint8_t m) {
  int shape [] = {1,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0};
  int eins [] = {82, 83, 84, 85};
  int zwei [] = {62, 63, 64, 65};
  int drei [] = {84, 85, 86, 87};
  int vier [] = {73, 74, 75, 76};
  int funf [] = {69, 70, 71, 72};
  int sechs [] = {49, 50, 51, 52, 53};
  int sieben [] = {77, 78, 79, 80, 81, 82};
  int acht [] = {89, 90, 91, 92};
  int neun [] = {103, 104, 105, 106};
  int zehn [] = {106, 107, 108, 109};
  int elf [] = {67, 68, 69};
  int zwolf [] = {94, 95, 96, 97, 98};
  int funf_min [] = {7, 8, 9, 10};
  int zehn_min [] = {18, 19, 20, 21};
  int viertel [] = {26, 27, 28, 29, 30, 31, 32};
  int zwanzig [] = {11, 12, 13, 14, 15, 16, 17};
  int dreiviertel [] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
  int vor [] = {36, 37, 38};
  int nach [] = {40, 41, 42, 43};
  int halb [] = {44, 45, 46, 47};

  // let's do the minutes first
  if (m>2 && m<=7) { // 05 funf_min nach
    for (byte i = 0; i < (sizeof(funf_min) / sizeof(funf_min[0])); i++) { shape[funf_min[i]] = 1; }
    for (byte i = 0; i < (sizeof(nach) / sizeof(nach[0])); i++) { shape[nach[i]] = 1; }
    
  } else if (m>7 && m<=12) { // 10 zehn_min nach
    for (byte i = 0; i < (sizeof(zehn_min) / sizeof(zehn_min[0])); i++) { shape[zehn_min[i]] = 1; }
    for (byte i = 0; i < (sizeof(nach) / sizeof(nach[0])); i++) { shape[nach[i]] = 1; }
     
  } else if (m>12 && m<=17) { // 15 viertel nach
    for (byte i = 0; i < (sizeof(viertel) / sizeof(viertel[0])); i++) { shape[viertel[i]] = 1; }
    for (byte i = 0; i < (sizeof(nach) / sizeof(nach[0])); i++) { shape[nach[i]] = 1; }
     
  } else if (m>17 && m<=22) { // 20 zwanzig nach
    for (byte i = 0; i < (sizeof(zwanzig) / sizeof(zwanzig[0])); i++) { shape[zwanzig[i]] = 1; }
    for (byte i = 0; i < (sizeof(nach) / sizeof(nach[0])); i++) { shape[nach[i]] = 1; }
    
   } else if (m>22 && m<=27) { // 25 funf_min vor halb
    h++;
    for (byte i = 0; i < (sizeof(funf_min) / sizeof(funf_min[0])); i++) { shape[funf_min[i]] = 1; }
    for (byte i = 0; i < (sizeof(vor) / sizeof(vor[0])); i++) { shape[vor[i]] = 1; }
    for (byte i = 0; i < (sizeof(halb) / sizeof(halb[0])); i++) { shape[halb[i]] = 1; }
    
  } else if (m>27 && m<=32) { // 30 halb
    h++;
    for (byte i = 0; i < (sizeof(halb) / sizeof(halb[0])); i++) { shape[halb[i]] = 1; }
    
  } else if (m>32 && m<=37) { // 35 funf_min nach halb
    h++;
    for (byte i = 0; i < (sizeof(funf_min) / sizeof(funf_min[0])); i++) { shape[funf_min[i]] = 1; }
    for (byte i = 0; i < (sizeof(nach) / sizeof(nach[0])); i++) { shape[nach[i]] = 1; }
    for (byte i = 0; i < (sizeof(halb) / sizeof(halb[0])); i++) { shape[halb[i]] = 1; }
    
  } else if (m>37 && m<=42) { // 40 zwanzig vor
    h++;
    for (byte i = 0; i < (sizeof(zwanzig) / sizeof(zwanzig[0])); i++) { shape[zwanzig[i]] = 1; }
    for (byte i = 0; i < (sizeof(vor) / sizeof(vor[0])); i++) { shape[vor[i]] = 1; }
    
  } else if (m>42 && m<=47) { // 45 dreiviertel
    h++;
    for (byte i = 0; i < (sizeof(dreiviertel) / sizeof(dreiviertel[0])); i++) { shape[dreiviertel[i]] = 1; }
    
  } else if (m>47 && m<=52) { // 50 zehn_min vor
    h++;
    for (byte i = 0; i < (sizeof(zehn_min) / sizeof(zehn_min[0])); i++) { shape[zehn_min[i]] = 1; }
    for (byte i = 0; i < (sizeof(vor) / sizeof(vor[0])); i++) { shape[vor[i]] = 1; }
    
  } else if (m>52 && m<=57) { // 55 funf_min vor
    h++;
    for (byte i = 0; i < (sizeof(funf_min) / sizeof(funf_min[0])); i++) { shape[funf_min[i]] = 1; }
    for (byte i = 0; i < (sizeof(vor) / sizeof(vor[0])); i++) { shape[vor[i]] = 1; }
    
  } else if (m>57) {
    h++;
  }

  // now let's get to the hours
  if (h == 0 || h == 12 || h == 24) {
    for (byte i = 0; i < (sizeof(zwolf) / sizeof(zwolf[0])); i++) { shape[zwolf[i]] = 1; }
    
  } else if (h == 1 || h == 13) {
    for (byte i = 0; i < (sizeof(eins) / sizeof(eins[0])); i++) { shape[eins[i]] = 1; }
     
  } else if (h == 2 || h == 14) {
    for (byte i = 0; i < (sizeof(zwei) / sizeof(zwei[0])); i++) { shape[zwei[i]] = 1; }
     
  } else if (h == 3 || h == 15) {
    for (byte i = 0; i < (sizeof(drei) / sizeof(drei[0])); i++) { shape[drei[i]] = 1; }
     
  } else if (h == 4 || h == 16) {
    for (byte i = 0; i < (sizeof(vier) / sizeof(vier[0])); i++) { shape[vier[i]] = 1; }
     
  } else if (h == 5 || h == 17) {
    for (byte i = 0; i < (sizeof(funf) / sizeof(funf[0])); i++) { shape[funf[i]] = 1; }
     
  } else if (h == 6 || h == 18) {
    for (byte i = 0; i < (sizeof(sechs) / sizeof(sechs[0])); i++) { shape[sechs[i]] = 1; }
     
  } else if (h == 7 || h == 19) {
    for (byte i = 0; i < (sizeof(sieben) / sizeof(sieben[0])); i++) { shape[sieben[i]] = 1; }
     
  } else if (h == 8 || h == 20) {
    for (byte i = 0; i < (sizeof(acht) / sizeof(acht[0])); i++) { shape[acht[i]] = 1; }
     
  } else if (h == 9 || h == 21) {
    for (byte i = 0; i < (sizeof(neun) / sizeof(neun[0])); i++) { shape[neun[i]] = 1; }
     
  } else if (h == 10 || h == 22) {
    for (byte i = 0; i < (sizeof(zehn) / sizeof(zehn[0])); i++) { shape[zehn[i]] = 1; }
     
  } else if (h == 11 || h == 23) {
    for (byte i = 0; i < (sizeof(elf) / sizeof(elf[0])); i++) { shape[elf[i]] = 1; }
     
  }
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if (shape[i] == 0) {
      strip.setPixelColor(109-i, strip.Color(0, 0, 0));
    } else {
      strip.setPixelColor(109-i, strip.Color(color[state/3][0], color[state/3][1], color[state/3][2]));
    }
  }
  strip.show();
}

void blink() {
  state++;
  if (state > 15) { //0-11: Color modes. 12-15: fancy effects
    state = 0;
  }
  if (state < 12) {
    strip.setBrightness(brightness[state%3]); //max is 255
    DateTime now = rtc.now();
    timeDisplay(now.hour(), now.minute());
  }
}

void adjustTime(int adj) {
  DateTime now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute()+adj, now.second()));
  //colorAll(strip.Color(color[state/3][0], color[state/3][1], color[state/3][2]), 50);
  now = rtc.now();
  timeDisplay(now.hour(), now.minute());
}
