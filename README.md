# Neopixel-Audio-Visualizer
Utilizing Adafruit's strandtest code and PJRC's PassThroughUSB, a small frankenstein of the two is born.

###### Quick functionailty overview
Playback
- Audio is sampled through USB or SD card
- Audio is played back through the Teensy's stereo DAC

Lights
- An FFT of the audio samples is performed, and all FFT bins are summed
- The sum is fed into Adafruit's Neopixel algorithm, and so..
-  Let there be light

###### Quick BOM
- Teensy 3.6
- 100 uF capacitors
- SD card
- Level shifter
- Neopixel Strip --> I used the 144-strip
- Pushbuttons

###### Credits
Credit towards Adafruit and PJRC for providing example code and libraries.


