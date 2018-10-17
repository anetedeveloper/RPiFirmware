#ifndef SERIALCARDREAD_H
#define SERIALCARDREAD_H
/*

#include <QDebug>
#include <QObject>
#include <QSerialPort>
#include <QDateTime>

class SerialCardRead : public QObject
{
    Q_OBJECT
public:
    SerialCardRead(QObject *parent = 0);
    ~SerialCardRead();
    void StartReading();

signals:
    void CardReaded(QString cardNumber);

private slots:
    void handleReadyRead();

private:
    QSerialPort serialPort;
    QString cardNumber;
    bool isReading;
};

*/
#endif // SERIALCARDREAD_H
