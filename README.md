# LED beat reactive Outfits

The holy grail of LED outfits is to be able to be in a dance club and have the outfit react to the beats. But this is a lot trickier than it seems. You
need to contend with different sound levels, being near super loud speakers, and many issues with software FFTs.

These files document over 6 years of experimentation to get something that works really well.

The summary:

- Use the [MAX9814 Adafruit](https://www.adafruit.com/product/1713) mic amplifier
  - This has auto gain control that works well
  - For clubs or loud parties swap out the electret mic for a 130dB one. I used [a CMC-9745-130T](https://www.digikey.co.uk/en/products/detail/cui-devices/CMC-9745-130T/7784321)
- Use a simple Arduino with a hardware FFT (a [MSGEQ7](https://www.sparkfun.com/products/10468) chip)
  - A PCB makes the above easier as there are a few passive components too
  - Don't buy ones from Aliexpress, they're often non-functional [fakes](https://forum.arduino.cc/t/what-are-the-symptoms-of-fake-msgeq7/896820)
- A cheap small Arduino-ish processor
  - I generally use the [Adafruit Trinket M0](https://www.adafruit.com/product/3500) as it's totally sufficient
- Some sort of fully addressable RGB strips (NeoPixels or otherwise)
  - In a way the outfit is still waterproof/washable
- Use a small rechargable battery bank from an established brand designed for phones
  - Even the smallest runs the outfit for 4-5 hours
  - (Don't even consider using LIPO batteries for wearables)
  - Something [like this](https://amazon.co.uk//dp/B01CU1EC6Y) (I used one even smaller)

If you find anything here useful I'd really appreciate letting me know as it helps motivate me to publish more of my projects!

## ledcorset - 2024 LED corset

[For empcamp in 2024 I made a LED corset for a friend that worked really well. The full details of that will be here shortly with circuit diagram, notes,
and software.](ledcorset/README.md) The actual circuit is the same as the ledskirt below, but having swapped out the electret mic on the max8914 with one that could handle
130db.

## ledcatears - 2023 LED catears

[For GFB in 2024 I made LED catears using the same design code and circuit](ledcatears/README.md)

## ledbag

For various events over the last 4 years I've had a light-up bag that contains the same code and circuit

## ledskirt - 2018 Magical Latex Girl LED skirt

[Included here are the OpenSCAD files for 3d printing the sled and the
arduino code used to control the LEDs and construction details](ledskirt/README.md)
