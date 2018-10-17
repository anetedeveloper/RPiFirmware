/*
#include "serialcardread.h"

SerialCardRead::SerialCardRead(QObject *parent) : QObject(parent)
{
}

SerialCardRead::~SerialCardRead()
{
    serialPort.close();
}

void SerialCardRead::StartReading()
{
    isReading = false;

    serialPort.setPortName("/dev/serial0");
    serialPort.setBaudRate(QSerialPort::Baud9600);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    if (!serialPort.open(QIODevice::ReadOnly)) {
        qWarning() << QObject::tr("Failed to open port /dev/serial0, error: %2").arg(serialPort.errorString()) << endl;
    }
    else
    {
        qDebug() << "Started readind on serial port";
    }
    connect(&serialPort, &QSerialPort::readyRead, this, &SerialCardRead::handleReadyRead);
}

void SerialCardRead::handleReadyRead()
{
    char *data = new char[2];
    if (serialPort.read(data, 1) == 0)
    {
        qWarning() << "CardRead received 0 bytes";
        return;
    }
    if (data[0] == 2)
    {
        qDebug() << "CardRead received START symbol !!";
        cardNumber.clear();
        isReading = true;
    }
    else if (data[0] == 3)
    {
        qDebug() << "CardRead received STOP  symbol, card number: " << cardNumber;
        isReading = false;
        emit CardReaded("<CardRead TimeStamp=\"" + QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss") + "\" Number=\"" + cardNumber + "\"/>");
        //emit CardReaded(cardNumber);
        cardNumber.clear();
    }
    else if (isReading)
    {
        cardNumber.append(data[0]);
    }
    else
    {
        qDebug() << "CardRead (without start symbol) received symbol: " << data[0];
    }
}
*/
