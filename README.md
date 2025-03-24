# LED beat-reactive outfits - LuxPulse!

The holy grail of LED outfits is to be able to be in a dance club and
have the outfit react to the beats. But this is a lot trickier than it
seems. You need to handle different sound levels, being near super
loud speakers, properly filtering the bass frequencies.

There are plenty of libraries and outfits that claim to do beat
matching but they generally don't. The best ones use software FFTs but
those have problems too.

These files and source code document over 8 years of experimentation
to get something that works really well in real life settings.

The summary of what works:

- Use the [MAX9814 Adafruit](https://www.adafruit.com/product/1713) mic amplifier
  - This has auto gain control that works well if you are near or far from a speaker
  - For clubs or loud parties swap out the electret mic for a 130dB one. I used [a CMC-9745-130T](https://www.digikey.co.uk/en/products/detail/cui-devices/CMC-9745-130T/7784321). I only started using this recently, it's not strictly needed.
- A cheap small Arduino-ish processor
  - I generally use the [Adafruit Trinket M0](https://www.adafruit.com/product/3500) as it's totally sufficient as no software FFT involved
- Hardware FFT (a [MSGEQ7](https://www.sparkfun.com/products/10468) chip)
  - A PCB makes the above easier as there are a few passive components needed too
  - Don't buy ones from Aliexpress or other cheap marketplaces, they're often non-functional [fakes](https://forum.arduino.cc/t/what-are-the-symptoms-of-fake-msgeq7/896820). I have quite a lot of fake chips trying to save a few $.
  - (I've never found software FFT to work as well as hardware)
- Use a small rechargable battery bank from an established brand designed for phones
  - Even the smallest runs the outfit for 4-5 hours
  - I generally don't consider using LIPO batteries for wearables as they could easily get crushed without lots of extra protection. (You don't want something that could catch fire if you drop it or sit on or puncture it)
  - Something [like this](https://amazon.co.uk//dp/B01CU1EC6Y) (I used one even smaller)
- Some sort of fully addressable RGB strips (NeoPixels or otherwise)
  - In a way the outfit is still waterproof/washable
  - I've used custom strips, existing rings, and pebble strings

If you find anything here useful I'd really appreciate letting me know as it helps motivate me to publish more of my projects!

## ledcorset - 2024 Corset

[For emfcamp in 2024 I made a LED corset for a friend to
wear](ledcorset/README.md) that worked really well. The full details
of that will be here shortly with circuit diagram and software similar
to all the other items. The actual circuit hasn't changed from the
2018 ledskirt below, but just having swapped out the electret mic for
a 130db rated one.

## ledcatears - 2023 Catears

[For GFB in 2024 I made LED catears using the same design code and circuit](ledcatears/README.md)

## ledbag - 2020 Bag

For various events over the last 5 years I've had a light-up bag that contains the same code and circuit

## ledskirt - 2018 Magical latex skirt topper

[My original design was to enhance a latex dress with sound reactive LEDs](ledskirt/README.md)

