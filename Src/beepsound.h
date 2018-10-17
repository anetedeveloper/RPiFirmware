#ifndef BEEPSOUND_H
#define BEEPSOUND_H
#include <QDebug>
#include <QThread>
#include "gpioclass.h"
#ifndef ForRaspberryPi
#include "Windows.h" // needed to play Beep sound
#include <QSound>
#endif

class BeepSound
{
private:
    GPIOClass *gpio;
    void OneBeep(double mSec);
public:
    BeepSound(string gpioNum);
    ~BeepSound();
    void BeepGPIO(double lengthMSec, double delayMSec, int count);
};

#endif // BEEPSOUND_H
