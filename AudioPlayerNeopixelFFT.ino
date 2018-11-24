#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_NeoPixel.h>

// GUItool: begin automatically generated code
AudioAnalyzeFFT256    myFFT;
AudioSynthKarplusStrong  string1;        //xy=257,633
AudioInputUSB            usb1;           //xy=258,396
AudioSynthSimpleDrum     drum1;          //xy=258,595
AudioPlaySdWav           playSdWav1;     //xy=272,525
AudioMixer4              mixer2;         //xy=423,446
AudioMixer4              mixer1;         //xy=425,376
AudioMixer4              mixer3;         //xy=429,537
AudioMixer4              mixer4;         //xy=434,629
AudioMixer4              mixer5;         //xy=759,411
AudioMixer4              mixer6;         //xy=760,553
AudioOutputAnalogStereo  dacs1;          //xy=960,476
AudioConnection          patchCord16(mixer5, 0, myFFT, 0);
AudioConnection          patchCord1(string1, 0, mixer4, 1);
AudioConnection          patchCord2(usb1, 0, mixer1, 0);
AudioConnection          patchCord3(usb1, 1, mixer2, 0);
AudioConnection          patchCord4(drum1, 0, mixer4, 0);
AudioConnection          patchCord5(playSdWav1, 0, mixer3, 0);
AudioConnection          patchCord6(playSdWav1, 1, mixer3, 1);
AudioConnection          patchCord7(mixer2, 0, mixer6, 0);
AudioConnection          patchCord8(mixer1, 0, mixer5, 0);
AudioConnection          patchCord9(mixer3, 0, mixer5, 1);
AudioConnection          patchCord10(mixer3, 0, mixer6, 1);
AudioConnection          patchCord11(mixer4, 0, mixer5, 2);
AudioConnection          patchCord12(mixer4, 0, mixer6, 2);
AudioConnection          patchCord13(mixer5, 0, dacs1, 0);
AudioConnection          patchCord15(mixer6, 0, dacs1, 1);
// GUItool: end automatically generated code

#define PIN 33

// Volume Settings
#define VOL_INCREMENT 1.1
#define VOL_LIMIT 5
#define VOL_MIN 0.01

// Drum Settings
#define DRUM_FREQ 266
#define DRUM_TIME 250
#define DRUM_PITCH_MOD 0.5

// Button Settings
#define NUM_KEYS 8

// Mixer Settings
#define NUM_CHANNEL 4

// Use these with the Teensy 3.5 & 3.6 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

const uint8_t buttons[NUM_KEYS] = {25, 26, 27, 28, 29, 30, 31, 32};
byte i, j, k;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(144, PIN, NEO_GRB + NEO_KHZ800);


// State variables for debounce
int keyCurr[NUM_KEYS] = {0, 0, 0, 0}, 
    keyPrev[NUM_KEYS] = {0, 0, 0, 0};

// Volume variables
double vol = 1;

void setup() {                
  AudioMemory(12);

  // Setup our buttons
  for(i = 0; i < NUM_KEYS; i++){
    pinMode(buttons[i], INPUT_PULLUP);
  }

  // Setup our drums
  drum1.frequency(DRUM_FREQ);
  drum1.length(DRUM_TIME);
  drum1.pitchMod(DRUM_PITCH_MOD);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  // Configure the window algorithm to use
  myFFT.windowFunction(AudioWindowHanning1024);
  
  // Configure analog reference
  analogReference(EXTERNAL);

  // Begin serial comunnications
  Serial.begin(115200);

  // Configure strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
float n, sum;
int ledcount;

// Reset button states
byte buttonCurr, buttonPrev;
sum = 0;

  buttonPrev = buttonCurr;
  buttonCurr = 0;

  // Read our buttons
  for(i = 0; i < NUM_KEYS; i++){
    keyPrev[i] = keyCurr[i];
    keyCurr[i] = !digitalRead(buttons[i]);

    // Store our button states
    if(keyCurr[i] != keyPrev[i] && keyCurr[i] == HIGH && keyPrev[i] == LOW){
      bitSet(buttonCurr, i);
    }
  }
//Effects

  if(buttonPrev != buttonCurr) {
    switch(buttonCurr){
      case 128:
        break;

      case 64:
        break;
     
      case 32:
        playSdWav1.stop();
        break;
      
      case 16:
        playSdWav1.play("zucchini.wav");
        break;

      
      case 8: 
        if( vol < VOL_LIMIT)
          vol = vol*VOL_INCREMENT + 0.01;
        else
          vol = VOL_LIMIT;
        break;
  
      case 4:
        if (vol >= VOL_MIN)
          vol = vol/VOL_INCREMENT;
        else 
          vol = 0;
        break;
  
      case 2:
          drum1.noteOn();
        break;
  
      case 1:
        string1.noteOn(267, 1);
        break;
      
      default:
        break;
    }
  }

    if (myFFT.available()) {
      for (i=0; i<127; i++) {
        n = myFFT.read(i);
        sum = sum + n;
        }
      Serial.println(sum);
      colorWipe(Wheel(sum*255));
    }
  

  // Volume Settings
  for(i = 0; i < NUM_CHANNEL; i++){
    mixer1.gain(i, vol);
    mixer2.gain(i, vol);
    mixer3.gain(i, vol);
    mixer4.gain(i, vol);

      if (i < 3) {
        mixer5.gain(i, vol);
        mixer6.gain(i, vol);
      }
  }

  mixer5.gain(3, 0);
  mixer6.gain(3, 0);
  

}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
