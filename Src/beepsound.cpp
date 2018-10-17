#include "beepsound.h"

BeepSound::BeepSound(string gpioNum)
{
#ifdef ForRaspberryPi
    this->gpio = new GPIOClass(gpioNum);
    this->gpio->export_gpio();
    QThread::sleep(1);
    this->gpio->setdir_gpio("out");
#endif
}

BeepSound::~BeepSound()
{
    this->gpio->unexport_gpio();
}

void BeepSound::OneBeep(double mSec)
{
    //qDebug() << "Beeeeep for " << mSec << "mili seconds";
    this->gpio->setval_gpio("1");
    QThread::msleep(mSec);
    this->gpio->setval_gpio("0");
}

void BeepSound::BeepGPIO(double lengthMSec, double delayMSec, int count)
{
#ifdef ForRaspberryPi
    this->OneBeep(lengthMSec);

    for (int i = 1; i < count; i++)
    {
        QThread::msleep(delayMSec);
        this->OneBeep(lengthMSec);
    }
#else
    lengthMSec++;delayMSec++; // to supress unused variable warning on windows
    if (count == 1)
        QSound::play("beep.wav");
    else
        QSound::play("error.wav");
#endif
}
