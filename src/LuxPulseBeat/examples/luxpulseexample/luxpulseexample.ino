#include "LuxPulseBeat.h"

// Our MSGEQ7 is connected to these pins (A for analog in, R for Reset, S for Strobe)
#define PIN_MSEQ7_A A1
#define PIN_MSEQ7_S 11
#define PIN_MSEQ7_R A2

LuxPulseBeat myBeats;

void setup() {
  Serial.begin(38400);
  myBeats.setup(PIN_MSEQ7_R, PIN_MSEQ7_S, PIN_MSEQ7_A);
}

void loop() {
  myBeats.loop();
  if (myBeats.isbeat() >0) {
    int bpm = myBeats.getBPM(5000);
    Serial.print("Beat");
    if (bpm !=0) {
      Serial.print(", BPM approx ");
      Serial.print(bpm);
    }
    Serial.println();
  }
  delay(60);
}
