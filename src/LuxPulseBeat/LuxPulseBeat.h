#ifndef LuxPulseBeat_h
#define LuxPulseBeat_h

class LuxPulseBeat
{
public:
    LuxPulseBeat();
    void setup(int resetpin, int strobepin, int analogpin);
    void loop();
    #define eq_buckets 7
    static int eqlevel[eq_buckets]; // eqlevel: 0=80Hz 1=160Hz 2=400Hz 3=1kHz 4=2.5kHz 5=6.5kHz 6=16kHz
    int whichfrequencybucket = 1; // which msgeq7 frequency bucket to use for beat detection  
    int isbeat(); // returns not 0 if there's a beat detected
    int getBPM(int ms); // get the bpm if it's stable or 0 if not
    bool outputbpm = false; // serial.println the bpm when it's calculated as steady debug

private:
    void readmsgeq7();
    void slidingwindow();

    int MSGEQ7ResetPin;
    int MSGEQ7StrobePin;
    int MSGEQ7AnalogPin;

    #define sliding 30 // for figuring out beats
    #define nbeats 8 // for our bpm calculation

    int lastbeats[nbeats];
    int lastbeatn = 0;
    int average[sliding+1];
    bool enoughsamples = false; // wait at start until we've filled the buffer once
    int winmax = 0;
    int winmin = 0;
    int naverage = 0;
    #define maxbpm 170 // to help calculation set a max bpm that's likely
    int minms = 60000/maxbpm;

    int bpm; // calculated bpm output
    unsigned long laststablebpm; // when we last updated bpm (i.e. had a stable bpm)
};

#endif