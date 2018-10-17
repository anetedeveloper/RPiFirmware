#include "rfid_rc522.h"
/*
RFID_RC522::RFID_RC522()
{
}

void RFID_RC522::StartReading()
{
#ifdef NotWorking
    proc = new QProcess();
    connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(handleRead()));
    proc->start("python /home/pi/Documents/MFRC522-python/DumpLight.py");
#endif
}

void RFID_RC522::handleRead()
{
#ifdef NotWorking
    CardNumber.append(proc->readAll());
    qDebug() << "In CardNumber: " << CardNumber;
    int startIndex = CardNumber.indexOf(StartKey);
    if (startIndex == -1)
        return;
    int stopIndex = CardNumber.indexOf(StopKey);
    if (stopIndex == -1)
        return;
    emit CardReaded(CardNumber.mid(startIndex, stopIndex - startIndex));
    CardNumber.clear();
#endif
}

void RFID_RC522::SetStartKey(int StartKey)
{
    this->StartKey = StartKey;
}

void RFID_RC522::SetStopKey(int StopKey)
{
    this->StopKey = StopKey;
}
*/
