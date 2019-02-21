// Magical Latex Girl LED skirt
// By Yui January 2016
//
// A Neopixel-based LED latex skirt which displays sparkles by default
// but can also do sound to light and if you have a hand sensor it
// will react to the acceleration of that sensor (how hard you are
// hitting something)
//
// We use a HC-05 bluetooth module, a MSGEQ7 for the spectrum
// analysis and some hand soldered neopixels in a 3d printed
// tray.
//
// I use a dfrobot Beetle, but anything arduino like with a spare
// hardware serial port that runs on 5v will do
//

#include <FastLED.h>  //  from https://github.com/FastLED/FastLED/
// we don't use the adafruit neopixel library to avoid interrupts and mess with serial port

// button stuff from http://forum.arduino.cc/index.php?topic=14479.0
#define PIN_PUSHBUTTON A0

// neopixel string on pin 9, 48 leds total
#define PIN_NEOPIXELDATA    9
#define NEOPIXEL_NUMLEDS    48
CRGB leds[NEOPIXEL_NUMLEDS + 1];
int level[7];

// bluetooth module
// connected to Sense of bluetooth hc05 link
#define PIN_HC05_BLUETOOTH_SENSE 10

// mseq7 equalizer module
#define HARDWARE_HAS_MSEQ7 1
#define PIN_MSEQ7_A A1
#define PIN_MSEQ7_S 11
#define PIN_MSEQ7_R A2

// if we want to trigger an estim box
#define HARDWARE_HAS_RFTX 0
#define PIN_ET 11

ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
{
    FastLED.clear();
}  

void setup() {
    pinMode(PIN_HC05_BLUETOOTH_SENSE, INPUT); 
#ifdef HARDWARE_HAS_RFTX
    setup_rftx();
#endif
    pinMode(PIN_PUSHBUTTON, INPUT);
    digitalWrite(PIN_PUSHBUTTON, HIGH);
#ifdef HARDWARE_HAS_MSEQ7
    pinMode(PIN_MSEQ7_A, INPUT);
    pinMode(PIN_MSEQ7_S, OUTPUT);
    pinMode(PIN_MSEQ7_R, OUTPUT);
    digitalWrite(PIN_MSEQ7_R, LOW);
    digitalWrite(PIN_MSEQ7_S, HIGH);
#endif
    Serial1.begin(9600);
    Serial1.setTimeout(100);
    Serial.begin(38400);
    delay(500);
    FastLED.addLeds<NEOPIXEL, PIN_NEOPIXELDATA>(leds, NEOPIXEL_NUMLEDS);
}

#ifdef HARDWARE_HAS_MSEQ7
// This is the mapping to the skirt, we connect all the pixels serially but
// we arrange them in 7 columns (col 1 starts at led 0, col 2 with led 7 etc)
int startrowled[] = {0, 7, 14, 20, 27, 33, 39};
int ledsperrow[] = {7, 7, 6, 7, 6, 6, 6 };
int levelmax[7];
int levelmin[7];
// How we map the sound to light channel to the skirt rows
int letsmap[] = {3, 2, 1, 0, 2, 3, 1 };
#endif

String command = "";
byte rhue = 190;
byte sendcmd;
byte sendloop = 0;
int soundtolight = 0; //2
int masterbrightness = 255;
int fixedhue = 0;
int cycle = 0;
uint32_t cycletime;

#if HARDWARE_HAS_MSEQ7
void do_equalizer_stuff() {
        digitalWrite(PIN_MSEQ7_R, HIGH);
        delayMicroseconds(75);
        digitalWrite(PIN_MSEQ7_R, LOW);
        noInterrupts();

        for (int i = 0; i < 7; i++) {
            digitalWrite(PIN_MSEQ7_S, LOW);
            delayMicroseconds(40); // should only need 40
            level[i] = analogRead(PIN_MSEQ7_A);
            digitalWrite(PIN_MSEQ7_S, HIGH);
            delayMicroseconds(40);
        }
        interrupts();

        for (int channel = 0; channel < 7; channel++) {
            // first establish a floor
            if (levelmax[channel] > 7000)
                levelmax[channel]-=10;  // so we refresh to new things!
            else
                levelmax[channel] = 7000;
            if (level[channel] * 10 > levelmax[channel]) {
                levelmax[channel] = level[channel] * 10;
                //Serial.print("Got a new max for channel ");
                //Serial.print(channel);
                //Serial.print(" : ");
                //Serial.println(levelmax[channel]);
            }
            if (levelmin[channel] > 5000)
                levelmin[channel] = 5000;
            else
                levelmin[channel]+=10;

            if (levelmin[channel] > level[channel]*10) {
                levelmin[channel] = level[channel]*10;
                //Serial.print("Min for ");
                //Serial.print(channel);
                //Serial.print(" : ");
                //Serial.println(levelmin[channel]);
            }
        }
        //Serial.print(levelmin[0]);
        //Serial.print(" - ");
        //Serial.println(levelmax[0]);
}
#endif

// A command we got from the bluetooth connection or from serial console for debug

void handle_command(String command)
{
    Serial.println(command);

    if (command[0] == 'h') {
        Serial.print("hue=");
        rhue = command.substring(1).toInt();
        Serial.println(rhue);
        for (byte xj = 0; xj < NEOPIXEL_NUMLEDS; xj++) {
            leds[xj] = h1v_to_rgb(rhue, 255);
        }
    } else if (command[0] == 'e') {
        soundtolight = command.substring(1).toInt();
        cycle = 0;
    } else if (command[0] == 'c') {
        soundtolight = 1;
        cycle = command.substring(1).toInt();
        cycletime = millis();
    } else if (command[0] == 'b') {
        masterbrightness = command.substring(1).toInt();
    } else if (command[0] == 'f') { // fix hue (256 means don't fix it)
        if (command.substring(1).toInt() > 255) {
            fixedhue = 0;
        } else {
            rhue = command.substring(1).toInt();
           fixedhue = 1;
        }
#ifdef HARDWARE_HAS_RFTX
    } else if (command[0] == 's') {
        rftx_send(command);
#endif
    } else {
        // Got an intensity from the hand held client
        int y = command.toInt();
#ifdef HARDWARE_HAS_RFTX        
        if (y > 120) {     // hard enough to trigger the estim box?
            sendcmd = 1;
            sendloop = 10;
        }
#endif
        // greenish through redish
        byte hue = y + 80;
        if (y > 165) {
            hue = 245;
        } else if (y > 300) {
            y = 300;
        }
        // rather than a bargraph we'll do a sparkle display, towards red for
        // a harder hit, and the higher the hit the more lights so they build
        // up.  Most of these variables are arbitrary based on trial and error
        // avoid having all the new LEDs being perfectly the same colour, a little randomness
        //
        for (int i = 0; i < (y / 5); i++) {
            leds[random8(NEOPIXEL_NUMLEDS)] += h1v_to_rgb(hue - 10 + random(20), 255);
        }
    }
}

byte counter = 0;
int cmaxlevel = 0;
int lightsoff = 0;

void loop()
{
    int b = checkButton();
    if (b >0 && lightsoff == 1) { // was off, turn back on
        lightsoff = 0;
    } else if (b == 1) { // Single Click
        soundtolight++;
        if (soundtolight == 7) // skip 7 with button
            soundtolight++;
        if (soundtolight > 10) // skip 11+ with button
            soundtolight = 0;
    } else if (b == 3) { // Hold
        soundtolight = 0;
        cycle = 0;
    } else if (b == 2) { // Double Click
        cycle = 1;
        soundtolight = 1;
    } else if (b == 4) { // Long Hold, "turn off"
        lightsoff = 1;
        FastLED.clear();
        FastLED.show();
    }
    while (Serial.available() > 0) {
        char c = Serial.read();
        command += c;
        if (c == '\r' || c == '\n') {
            handle_command(command);
            command = "";
        }
    }

    // ***** bluetooth link means hand controller is connected do that *****

    if (digitalRead(PIN_HC05_BLUETOOTH_SENSE)) {
        fadeToBlackBy(leds, NEOPIXEL_NUMLEDS, 40); // was 50

        while (Serial1.available() > 0) {
            char c = Serial1.read();
            command += c;
            if (c == '\r' || c == '\n') {
                handle_command(command);
                command = "";
            }
        }
    } 

    // **** default idle mode is just a random sparkle shifting colours through time *****
    
    else if (soundtolight == 0) {
        fadeToBlackBy(leds, NEOPIXEL_NUMLEDS, 20); // was 50

        for (int j = 0 ; j < 1; j++) {
            int pos = random16(NEOPIXEL_NUMLEDS);
            leds[pos] += CHSV( random8(50) + rhue, 200, masterbrightness);
        }
        counter++;
        if (counter == 0 && fixedhue == 0)
            rhue += 16;
    } else {
#if HARDWARE_HAS_MSEQ7
        do_equalizer_stuff();

        // ***** Sound to light bargraph modes *****

        if (soundtolight == 1 || soundtolight == 2 || soundtolight == 3) {  // we're saying 0 to 960
            FastLED.clear();
            for (int row = 0; row < 7; row++) {
                int xmax = ledsperrow[row];
                int lmrow = letsmap[row];
                if (soundtolight ==1) 
                    lmrow = 1; // force all to same!
                //      s = min((level[letsmap[row]]-150)/114,xmax);    //146
                int scaler = (levelmax[lmrow] - 5000) / xmax;
                //int scaler = (levelmax[letsmap[row]] - levelmin[letsmap[row]]) / xmax;

                int s = ((level[lmrow] * 10) - 5000) / scaler;
                if (s != 0) {
                    for (int i = 0; i < s; i++) {
                        // 96 is green, 0 is red
                        if (soundtolight == 2)
                            leds[startrowled[row] + xmax - 1 - i] = CHSV(96 - (((i + 1) * 96 / xmax)), 255, masterbrightness);
                        else if (soundtolight == 1 or soundtolight == 3) 
                           leds[startrowled[row] + i] = CHSV(180 + (((i + 1) * 80 / xmax)), 255, masterbrightness);
                    }
                }
            }
            delay(15);
        }

        // ***** Sound to light sparkle on bass *****

        else if (soundtolight == 4) { 
            fadeToBlackBy( leds, NEOPIXEL_NUMLEDS, 30);
            for (int i = 0; i < (level[1] - 736) / 50; i++) {
                int pos = random16(NEOPIXEL_NUMLEDS);
                int col = (level[1] - 736) / 3;
                leds[pos] += CHSV(190 + col, 255, level[1] / 5);
            }
        }

        // ***** Sound to light rotate bars on bass *****

        else if (soundtolight == 5) { 
            fadeToBlackBy( leds, NEOPIXEL_NUMLEDS, 20);
            if (level[1] > 800 && cmaxlevel == 0) {
                counter++;
                if (counter > 6) 
                    counter = 0;
                for (int i = 0; i < ledsperrow[counter]; i++) 
                    leds[startrowled[counter] + i] += CHSV(190 + ((level[1] - 700) / 4) + random8(20), 255, masterbrightness);
                cmaxlevel = 1;
            } // should be level[1] but level[0] works so well here!
            if (cmaxlevel != 0 && level[0] < 700) 
                cmaxlevel = 0;
        }

        // ***** Sound to light move leds down the row on bass *****

        else if (soundtolight == 6) {
            fadeToBlackBy( leds, NEOPIXEL_NUMLEDS, 10);
            int blah = level[1];
            if (blah > 800 && cmaxlevel == 0) {
                counter++;
                if (counter > 5) 
                    counter = 0;
                for (int i = 0; i < 7; i++) {
                    leds[startrowled[i] + counter] += CHSV(190 + ((blah - 700) / 4) + random8(20), 255, masterbrightness);
                    if (counter == 5 && ledsperrow[i] == 7) 
                        leds[startrowled[i] + 6] += CHSV(190 + ((blah - 700) / 4) + random8(20), 255, masterbrightness);
                }
                cmaxlevel = blah;
            }
            if (cmaxlevel != 0 && blah < cmaxlevel-200) 
                cmaxlevel = 0;
        }

        // ***** Sound to light sparkle on bass *****

        else if (soundtolight == 8) {
            FastLED.clear();
            for (int i = 0; i < NEOPIXEL_NUMLEDS; i++) 
                leds[i] = CHSV(190 + 50, 255, level[1] / 4);
        }

        // ***** Pretend we are an Fruity phone device *****

        else if (soundtolight == 9) {
            float breath = (exp(sin(millis() / 2000.0 * PI)) - 0.36787944) * 108.0; // from https://www.youtube.com/watch?v=5O94PDd1aOM
            for (int i = 0; i < NEOPIXEL_NUMLEDS; i++)
                leds[i] = CHSV(190 + 50, 0, breath);
        }

        // ***** Just a test display *****

        else if (soundtolight == 10) {
            fadeToBlackBy( leds, NEOPIXEL_NUMLEDS, 60);
            int startrowled[] = {0, 7, 14, 20, 27, 33, 39};
            counter++;
            for (int i = 0; i < 7; i++) {
                if (counter < 7)
                    leds[startrowled[i] + counter - 1] = CHSV(rhue, 255, 255);
                else if (counter < 13)
                    leds[startrowled[i] + 12 - counter] = CHSV(rhue, 255, 255);
                if (ledsperrow[i] == 7 && counter == 6)
                    leds[startrowled[i] + counter] = CHSV(rhue, 255, 255);
            }
            if (counter > 13)
                counter = 0;
            delay(30);
        }

        // ***** Back to the start *****

        else 
            soundtolight = 1;

        if (cycle == 1 && (millis() > (cycletime + 30000))) { // 30 seconds
            soundtolight++;
            cycletime = millis();
        }
        delay(5); // extra delay
#endif
    }
    if (!lightsoff)
       FastLED.show();
#ifdef HARDWARE_HAS_RFTX
    if (sendloop > 0) {
        etsend(sendcmd, PIN_ET); // this takes 57mS you don't really notice
        sendloop--;
        return;
    }
#endif
    delay(40);  // don't go any higher or the fade out looks too forced
}

