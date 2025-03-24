// Beat detection using hardware FFT (MSGEQ7)
//
// let's create a peak detector using a sliding window and a standard deviation
// we can tweak this by altering the window size in terms of stdev and
// the number of samples in the window, but this works well.

#include <Arduino.h>
#include "LuxPulseBeat.h"

int LuxPulseBeat::eqlevel[eq_buckets] = {0};

LuxPulseBeat::LuxPulseBeat() {}

void LuxPulseBeat::setup(int resetpin, int strobepin, int analogpin) {
    MSGEQ7ResetPin = resetpin;
    MSGEQ7StrobePin = strobepin;
    MSGEQ7AnalogPin = analogpin;

    pinMode(MSGEQ7ResetPin, OUTPUT);
    pinMode(MSGEQ7StrobePin, OUTPUT);
    digitalWrite(MSGEQ7ResetPin, LOW);
    digitalWrite(MSGEQ7StrobePin, HIGH);
}

void LuxPulseBeat::readmsgeq7()  {  
    // we follow the datasheet and based on experimentation
    // this routines takes about 624uS.  We could make it faster ignoring the higher bands if we don't need them,
    // or even removing the reset after the first one, but it's under a mS so don't care
    noInterrupts();  
    digitalWrite(MSGEQ7ResetPin, HIGH);
    delayMicroseconds(2); // 75 on original led skirt -- datasheet says 100nS (=0.1uS) min
    digitalWrite(MSGEQ7ResetPin, LOW);
    delayMicroseconds(76); // missing on original led skirt, nointerrupts is here -- datasheet: should be ts 18uS min
    for (byte band = 0; band < eq_buckets-1; band++) {
      digitalWrite(MSGEQ7StrobePin, LOW);
      delayMicroseconds(48); // datasheet: should be 18uS min
      eqlevel[band] = analogRead(MSGEQ7AnalogPin); // valid after 36uS from LOW, until strobe is high
      digitalWrite(MSGEQ7StrobePin, HIGH);
      delayMicroseconds(30); // datasheet: should be 18uS min but 72uS min total
    }
    interrupts();
  }

  void LuxPulseBeat::slidingwindow() {
    // create an average of the last n (30) eq values we received
    // then figure out the standard deviation and set our window level
    average[naverage++] = eqlevel[whichfrequencybucket];
    if (naverage >= sliding ) {
      enoughsamples = true;
      naverage = 0;
    }
    if (!enoughsamples) 
      return; // first run, wait until we've got a full buffer

    int slidingaverage = 0;
    for (int i = 0; i < sliding; i++) {
      slidingaverage += average[i];
    }
    slidingaverage /= sliding;
    long total = 0;
    for (int i = 0; i < sliding; i++) {
      total = total + (average[i] - slidingaverage) * (average[i] - slidingaverage);
    }
    float variance = total / (float)sliding;
    float stddev = sqrt(variance);
    int window = 2 * int(stddev);
    winmin = max(50, slidingaverage - window);
    winmax = min(1000, slidingaverage + window);
  }

// Detect the beat from the hardware FFT bucket
//
// We've tried so so many beat detection things over the years with
// sliding windows, exponential, differentiation, and so on, but
// this seems to work quite well

int LuxPulseBeat::isbeat() {
    static unsigned long lastmillis = 0;
    static int beattemp = 0;
    int isabeat = 0;
    int k = eqlevel[whichfrequencybucket];
    if (k >= beattemp) {  // eq level is going up
        beattemp = k;
        return isabeat;
    }
    // eq level is going down
    if (beattemp > winmax ) {
      if ((millis() - lastmillis > minms)) { 
        isabeat = beattemp;
        lastbeats[lastbeatn] = millis() - lastmillis;
        lastbeatn++;
        if (lastbeatn > nbeats)
          lastbeatn = 0;
        int times = 0;
        byte ntimes = 0;
        for (byte i = 0; i < nbeats; i++) {
          times += lastbeats[i];
          if (lastbeats[i] != 0) ntimes++;
        }
        if (times > 0 && ntimes > 0) {
          int averageTime = times / ntimes;
          // Filter outliers based on a threshold, e.g., 15% deviation
          int filteredSum = 0;
          byte filteredCount = 0;

          for (byte i = 0; i < nbeats; i++) {
            if (lastbeats[i] != 0 && abs(lastbeats[i] - averageTime) < averageTime * .15) {
              filteredSum += lastbeats[i];
              filteredCount++;
            }
          }    
          // Calculate BPM using filtered values
          if (filteredCount > nbeats/2 && filteredSum > 0) {
            bpm = 60000 * filteredCount / filteredSum;
            laststablebpm = millis();
            if (outputbpm) Serial.println(bpm);
          }

        }
  #ifdef BEATDEBUG
        Serial.print(millis());
        Serial.print(",");
        Serial.print(millis() - lastmillis);
        Serial.print(",");
        Serial.print(beattemp);
        Serial.print(",");
        Serial.println(bpm);
  #endif  
        lastmillis = millis();
      }
    }
    beattemp = 0;

    return isabeat;
}

void LuxPulseBeat::loop()
{
    readmsgeq7();
    slidingwindow();
    // just makes the display look a bit better when we do a FFT display
    eqlevel[3] = min(4095,eqlevel[3] * 3);
    eqlevel[4] = min(4095,eqlevel[4] * 4);
}

// Return the last stable bpm (given a timeout on how long is stable to allow for beat drops and stuff vs 'not likely music')
int LuxPulseBeat::getBPM(int ms)
{
  if (millis() - laststablebpm < ms)
    return bpm;
  return 0;
}