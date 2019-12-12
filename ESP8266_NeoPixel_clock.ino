
#include <NtpClientLib.h>
#include <ESP8266WiFi.h>          
#include <DNSServer.h>            
#include <ESP8266WebServer.h>     
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Adafruit_NeoPixel.h>
#include <TM1637Display.h>

const int CLK = D2; //Set the CLK pin connection to the display
const int DIO = D3; //Set the DIO pin connection to the display
const int VCC = D4; //Set the VCC pin connection to the display
TM1637Display display(CLK, DIO); //set up the 4-Digit Display.

#define NUMPIXELS      24       // number of NeoPixel LEDs
#define PIN            D5        // digital pin on ESP8266 for the NeoPixel data line
#define mirror_hands   false     // In case the NeoPixel ring is wired ant-clockwise. 

byte hour_hand, minute_hand, second_hand, previous_second;
byte ring_offset = 6;               // Adjument ring start position
byte blink_sec = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// clear all the leds to off
void clearHands() {
  for (byte i=0; i<=NUMPIXELS;i++) {
    pixels.setPixelColor(i,pixels.Color(0,0,0));
  }
}

void drawHands(){
  clearHands();
  
  pixels.setPixelColor(0,pixels.Color(0,0,100));
  pixels.setPixelColor(6,pixels.Color(0,0,100));
  pixels.setPixelColor(12,pixels.Color(0,0,100));
  pixels.setPixelColor(18,pixels.Color(0,0,100));

  pixels.setPixelColor(hour_hand,pixels.Color(100,0,0));
  // if hour and minute are the same led, use a different color to show that
  if (hour_hand==minute_hand) {
    pixels.setPixelColor(minute_hand,pixels.Color(100,100,0));
  } else {
    pixels.setPixelColor(minute_hand,pixels.Color(0,100,0));
  }

  if(blink_sec = 1 - blink_sec)
  {
    // draw the second LED, using medium brightness white
    pixels.setPixelColor(second_hand,pixels.Color(100,100,100));
  }
  
  // show all the LED's, only the ones we have set with a color will be visible.
  pixels.show();

  // just a debug string, can be removed
  Serial.printf("hour:%d (%d), minute:%d second:%d (%d) \n",hour(),hour_hand,minute_hand,previous_second,second_hand);
}

void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager; // wifi configuration wizard
  wifiManager.autoConnect("NeoPixel_Clock2", "secret"); // configuration for the access point, set your own secret. 
  Serial.println("WiFi Client connected!)");
  NTP.begin("es.pool.ntp.org", 9, true); // get time from NTP server pool.
  NTP.setInterval(63);
  pixels.begin();
  pixels.setBrightness(254);

  pinMode(VCC, OUTPUT);
  digitalWrite(VCC, HIGH);
  display.setBrightness(0x04); //set the diplay to 0..7 brightness

}

void loop() {
  
  minute_hand = (minute() * NUMPIXELS / 60 + ring_offset) % NUMPIXELS;
  
  if (hour() >= 12) {
    hour_hand = (((hour() - 12) * NUMPIXELS / 12) + (minute() *2 / 60) + ring_offset) % NUMPIXELS;
  }
  else {
    hour_hand = ((hour() * NUMPIXELS / 12) + (minute() *2 / 60) + ring_offset) % NUMPIXELS;
  }
   
  second_hand = (second() * NUMPIXELS / 60 + ring_offset) % NUMPIXELS;

  if (mirror_hands) {
    hour_hand=NUMPIXELS-hour_hand;
    minute_hand=NUMPIXELS-minute_hand;
    second_hand=NUMPIXELS-second_hand;
  }
  
  drawHands();
  if (second_hand!=previous_second) {
    previous_second=second_hand;
    // drawHands();
  }
  delay(490);
  
  display.showNumberDecEx(hour() * 100 + minute(), 0x40); //Display the numCounter value;

}
