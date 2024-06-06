# LED beat reactive Outfits

The holy grail of LED outfits is to be able to be in a dance club and have the outfit react to the beats. But this is a lot trickier than it seems. You
need to contend with different sound levels, being near super loud speakers, and many issues with software FFTs.

These files document over 6 years of experimentation to get something that works really well.

The summary is

- Use the MAX9814 Adafruit mic amplifier and AGC
  - Swap out the electret mic for a 130dB one
- Use a simple Arduino with a hardware FFT (a MSGEQ7 chip)
  - A PCB makes the above easier as there are a few passive components too
- Some sort of fully addressable RGB strips (NeoPixels or otherwise)
  - In a way the outfit is still waterproof/washable

## ledcorset - 2024 LED corset

[For empcamp in 2024 I made a LED corset for a friend that worked really well. The full details of that will be here shortly with circuit diagram, notes,
and software.](ledcorset/README.md) The actual circuit is the same as the ledskirt below, but having swapped out the electret mic on the max8914 with one that could handle
130db.

## ledcatears - 2023 LED catears

For GFB in 2024 I made LED catears using the same design code and circuit

## ledbag

For various events over the last 4 years I've had a light-up bag that contains the same code and circuit

## ledskirt - 2018 Magical Latex Girl LED skirt

[Included here are the OpenSCAD files for 3d printing the sled and the
arduino code used to control the LEDs and construction details](ledskirt/README.md)
