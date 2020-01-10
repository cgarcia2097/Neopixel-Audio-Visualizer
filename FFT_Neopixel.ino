/*********************************************************************
 * Global Libraries
 *********************************************************************/
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>

/*********************************************************************
 * Global Variables and definitions
 *********************************************************************/

#define LED_PIN 32
#define NUM_LEDS 9

// Use these with the Teensy 3.5 & 3.6 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

/*
 * Colour Modes
 */
#define TRANSISTOR_RAINBOW
//#define RAINBOW_MODE
//#define TRANSISTOR_GREEN
//#define TRANSISTOR_GREEN_2
//#define TRANSISTOR_GREEN_3
//#define TRANSISTOR_GREEN_4
//#define TRANSISTOR_RED
//#define SINGLE_MODE

#define LED_CURR_CHANNEL 1

/*
 * FFT Modes
 */
#define FFT_MODE_1
//#define FFT_MODE_2

/**
 * Buttons
 */
#define BUTTON_PIN 34
Bounce key = Bounce();

/**
 * Automatically generated code from Teensy Audio Design tool
 */
// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav1;     //xy=130,323
AudioInputUSB            usb1;           //xy=143,380
AudioInputAnalog         adc1;           //xy=147,269
AudioMixer4              mixer1;         //xy=315,291
AudioAmplifier           amp1;           //xy=462,290
AudioAnalyzeRMS          rms1;           //xy=626,303
AudioAnalyzeFFT256       myFFT;      //xy=638,255
AudioOutputAnalogStereo  dacs1;          //xy=641,438
AudioConnection          patchCord1(playSdWav1, 0, mixer1, 1);
AudioConnection          patchCord2(playSdWav1, 1, mixer1, 2);
AudioConnection          patchCord3(adc1, 0, mixer1, 0);
AudioConnection          patchCord4(mixer1, amp1);
AudioConnection          patchCord5(amp1, myFFT);
AudioConnection          patchCord6(amp1, rms1);
AudioConnection          patchCord7(amp1, 0, dacs1, 0);
AudioConnection          patchCord8(amp1, 0, dacs1, 1);
AudioConnection          patchCord9(usb1, 0, mixer1, 3);
// GUItool: end automatically generated code

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
int i, pixelValue; 
float rgb[3], rgb_prev[3] ;


/*********************************************************************
 * Functions
 *********************************************************************/

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

// Fill the dots one after the other with a color
void colorWipe(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

// Play files from SD Card
void playFile(const char *filename)
{
  playSdWav1.play(filename);
  // A brief delay for the library read WAV info
  delay(5);


}

/*********************************************************************
 * Setup 
 *********************************************************************/

void setup() {

  // Allocate memory to audio functions
  AudioMemory(50);

  // Start communications
  Serial.begin(921600);

  // Set up buttons
  key.attach(BUTTON_PIN, INPUT_PULLUP);
  key.interval(8);

  // Set up SD Card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }  
    
  // Setup LED Strip
  strip.begin();
  strip.setBrightness(255);
  strip.show(); 
    
  // Configure the window algorithm to use
  myFFT.windowFunction(AudioWindowBlackmanNuttall256);

  // Set pre amp
  amp1.gain(3);

}

/*********************************************************************
 * Main 
 *********************************************************************/

void loop() {

  // Store previous value
  for(i = 0; i < 3; i++){
    rgb_prev[i] = rgb[i];
  }
  
    // Read FFT
    if (myFFT.available()) {
  
      #ifdef FFT_MODE_1
        // Sum the bass-related bins
        rgb[0] = (rgb_prev[0] + myFFT.read(0, 1))/2.0;
        
        // Sum the mid-related bins
        rgb[1] = (rgb_prev[1] + myFFT.read(3,20))/2.0;
        
        // Sum the treble-related bins
        rgb[2] = (rgb_prev[2] + myFFT.read(22,127))/2.0;
      #endif
      
      #ifdef FFT_MODE_2
        // TODO: Different FFT mode 
      #endif
    }
  
    // Set strip color
      #ifdef SINGLE_MODE
        for(i = 0; i < 3; i++){
          strip.setPixelColor(i, strip.Color(rgb[0]*255, 0, 0));
          strip.setPixelColor(i+3, strip.Color(0, rgb[1]*255, 0));
          strip.setPixelColor(i+6, strip.Color(0, 0, rgb[2]*255));
        }
      #endif
  
      #ifdef RAINBOW_MODE
        for(i = 0; i < 3; i++){
          strip.setPixelColor(i, Wheel(rgb[0]*255));
          strip.setPixelColor(i+3, Wheel(rgb[1]*255));
          strip.setPixelColor(i+6, Wheel(rgb[2]*255));    
        }
      #endif
  
      #ifdef TRANSISTOR_GREEN
        for(i = 0; i < 3; i++){
          strip.setPixelColor(i, strip.Color(0, rgb[LED_CURR_CHANNEL]*255, 0));
          strip.setPixelColor(i+3, strip.Color(0, rgb[LED_CURR_CHANNEL]*255, 0));
          strip.setPixelColor(i+6, strip.Color(0, rgb[LED_CURR_CHANNEL]*255, 0));        
        }
      #endif
  
      #ifdef TRANSISTOR_GREEN_2
        for(i = 0; i < 3; i++){
          strip.setPixelColor(i, strip.Color(0, rgb[0]*255, 0));
          strip.setPixelColor(i+3, strip.Color(0, rgb[1]*255, 0));
          strip.setPixelColor(i+6, strip.Color(0, rgb[2]*255, 0));        
        }
      #endif
  
      #ifdef TRANSISTOR_GREEN_3
        for(i = 0; i < 3; i++){
          strip.setPixelColor(i, strip.Color(0, rgb[0]*255 - i*64, 0));
          strip.setPixelColor(i+3, strip.Color(0, rgb[1]*255 - i*64, 0));
          strip.setPixelColor(i+6, strip.Color(0, rgb[2]*255 - i*64, 0));  
        }
      #endif
  
      // Faded bar graph with background brightness
      #ifdef TRANSISTOR_GREEN_4
        pixelValue = rgb[0]*192;
        
        for(i = 0; i < NUM_LEDS; i++){
  
          if(pixelValue >= (i*28)){
            strip.setPixelColor(i, strip.Color(0, pixelValue + 32, 0));
          }
          else{
            strip.setPixelColor(i, strip.Color(0, 16, 0));        
          }
        }
      #endif
      
      #ifdef TRANSISTOR_RAINBOW
        pixelValue = rgb[2]*255;
        
        for(i = 0; i < NUM_LEDS; i++){
  
          if(pixelValue >= (i*28)){
            strip.setPixelColor(i, Wheel(pixelValue + 32));
          }
          else{
            strip.setPixelColor(i, strip.Color(0, 32, 0));        
          }
        }
      #endif      
    
    strip.show(); 

}
