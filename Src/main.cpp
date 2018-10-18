#include "rallo.h"
#include "keyboardhandler.h"
#include "argumentparser.h"
#include <iostream>
#include <QApplication>
#include <QtGlobal>
using namespace std;
#ifdef ForRaspberryPi
const QString LOG_FOLDER = "/home/pi/logs";
#else
const QString LOG_FOLDER = "logs";
#endif
QString outputFileName;

void RemoveOldLogs(int NumberOfStoredLog)
{
    QDir myDir(LOG_FOLDER);
    myDir.setNameFilters(QStringList()<<"AppLog-*");
    QStringList filesList = myDir.entryList();
    for (int i = 0; i < filesList.size()-NumberOfStoredLog; ++i) {
        qDebug() << "Removing old LogFile: " << filesList[i];
        myDir.remove(filesList[i]);
    }
}

void LogHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg: // qDebug()
        txt = "Debug ";
        break;
    case QtWarningMsg: // qWarning()
        txt = "Warni ";
        break;
    case QtCriticalMsg: // qCritical()
        txt = "Criti ";
        break;
    case QtFatalMsg: // qFatal()
        txt = "Fatal ";
        break;
    default:
        txt = "Unknow";
        break;
    }
    txt.append(QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss: ")).append(msg).append("\r\n");
    string outTxt = txt.toStdString();
    cout << outTxt;
    QFile outFile(outputFileName);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    outFile.write(outTxt.c_str());
    outFile.close();
    if (type == QtFatalMsg)
    {
        abort();
    }
    return;
}


int main(int argc, char *argv[])
{
    Config *config = new Config();
    //verzia - 003"; // odtesotavan na RPi dlhodobo
    //verzia - 010"; // implementovane styly aplikacie
    //verzia - 020"; // asynchronny qmessage box
    //verzia - 031"; // pridany argument nastavenia portu (-p 123 / --port 123), pridany DNS nazov
    //verzia - 040"; // nastavovanie rowstretch a columnstretch
    //verzia - 050"; // nastavovanie zmena casu (root) + rozdelenie LOG
    // 17.04.05"; // nova init obrazovka
    // 17.04.06"; // opravena chyba ak sa pripajali dva zariadenia naraz tak server spadol
    // 17.04.14"; // opravena pri nacitani Init obrazovky padala aplikacia
    // 17.05.07"; // oprava SIGSEG u nastavovania VBoxLayout
    // 17.07.14";
    // 17.08.20"; // Pridany parameter -d [display], -e [emulator], Log subor obsahuje port v nazve, layout pridana modifikacia pre QWidget, pri BEEP sa nezobrazuje MSGbox na Windows
    // 17.09.12"; // Pre windows sa beep zvuk nasobi 11x
    // 17.09.19"; // pre Windows prehrava zvuky beep.vaw a error.vaw
    // 17.11.23"; // Implementacia Modalneho okna
    // 17.12.25"; // New: not acknowledge to messages RPO->RLO
    // 17.12.27"; // New: SerialRead, location of LOGs in RPI
    // 18.01.29"; // New: Build for Windows
    //"18.02.12"; // New: PRO file specified for Windows/Linux + UART Serial card reader
    //"18.02.16"; // Disabled UART card reader, init messages modified, log files relative path
    //"18.03.28"; // fixed messages
    //"18.04.09"; // logs to folder "logs", new string builder of logs
    //"18.05.14"; // exit codes in argument parser, log if new server trying to connect while I am connected
    //"18.06.04"; // Save pictures
    // -e 800x480 resolution after emulator parameter added
    // "18.10.15" - podpora vyssi DPI, prevod na novou verzi QT

    config->programVersion = "18.10.15";

    // filename for output from argumentparser
    outputFileName = LOG_FOLDER + "/AppLog-"+QDate::currentDate().toString("yyyy-MM-dd")+".log";
    qInstallMessageHandler(LogHandler); // first to redirect all messages
    if (!QDir(LOG_FOLDER).exists())
    {
        QDir().mkdir(LOG_FOLDER); // first create folder, then write to it
        qDebug() << "Created folder: " << LOG_FOLDER;
    }
    ArgumentParser::Parse(argc, argv, config);

    // reset filename with portnumber
    outputFileName = LOG_FOLDER + "/AppLog-" + QDate::currentDate().toString("yyyy-MM-dd") + "-p" + QString::number(config->portNumber) + ".log";
    // podpora vyssiho DPI
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    qDebug() << "App is starting on port " << config->portNumber << " - version: " << config->programVersion.toStdString().c_str() << ", style: " << QApplication::style();
    KeyboardHandler *keyboardCardRead = new KeyboardHandler;
    Rallo *rallo = new Rallo(config);
    rallo->Init(keyboardCardRead);
    a.installEventFilter(keyboardCardRead);
    qDebug() << "Initialization finished";
    QTimer::singleShot(0, rallo, SLOT(Start()));
    RemoveOldLogs(config->logs);
    return a.exec();
}
