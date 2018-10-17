#ifndef LAYOUT_H
#define LAYOUT_H

#include <QMap>
#include <QQueue>
#include <typeinfo>
#include <QUiLoader>
#include <QLabel>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QPushButton>
#include <QToolButton>
#include <QMainWindow>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QXmlStreamReader>

#include "beepsound.h"
#include "rfid_rc522.h"
#include "serialcardread.h"
#include "keyboardhandler.h"
#include "modalwindow.h"
#include "ui_rallo.h"
#include "config.h"

namespace Ui {
class Layout;
}

class Layout : public QMainWindow
{
    Q_OBJECT

public:
    Layout(Config *AppConfig, QWidget *parent = 0);
    ~Layout();
    ModalWindow *modalWindow;
    void Init();
    void ParseXmlData(QXmlStreamReader *xml);
    void StopTimers();
    // RFID proxy ctecka
    //SerialCardRead *serialCard;

#ifdef QT_DEBUG
    const bool Debug = false;
    const bool DebugErrors = true;
    const bool DebugIOData = false;
    const bool DebugFunction = true;
    const bool DebugWarnings = true;
    const bool DebugFindButton = false;
    const bool DebugStoredData = false;
    const bool DebugFunctionAll = false;
#else
    // ForRaspberryPi & RELEASE
    const bool Debug = false;
    const bool DebugErrors = true;
    const bool DebugIOData = false;
    const bool DebugFunction = true;
    const bool DebugWarnings = true;
    const bool DebugFindButton = false;
    const bool DebugStoredData = false;
    const bool DebugFunctionAll = false;
#endif
    QString NewConnectionEstablished = "Server bol pripojeny";
    QString ServerWasDisconnected = "Server bol odpojený";
    QString WaitingForServerToConnect = "Čakám na pripojenie servera";
    QString UnableToStartServer = "Nepodarilo sa spustiť server: ";
    QString initLayoutName = "InitLayout.ui";
    QString VerziaTitle = "Verzia: ";
    QString InitInProgress = "Prebehla inicializacia";

signals:
    void MessageToSend(QString msg);

public slots:
    void UpdateTime();
    void UpdateIpAddressOnLayout();
    void ShowMessage(QString message);

private:
    Ui::Layout *ui;
    QVBoxLayout *mainLayout;

    Config *AppConfig;
    BeepSound *beep;
    QString CurrentLayout;
    QString messageToSend;
    QTimer *CurrTimeTimer;
    QTimer *IPUpdateTimer;
    QMap<QString, QString> Layouts;
    QMap<QString, QString> References;
    QString ProcessLayoutButtons(QString inputLayout);
    bool EditQObjectAtribute(QString id, QString attribute, QString newValue);
    void SetTextsOnLayout(QString message);
    QString ModifyAttributeFromXML(QXmlStreamReader *xml);
    void ProcessXmlBodyOfShowLayout(QXmlStreamReader *xml);
    void ShowInitLayout();
    void SetTimersForTime();
    void xmlInitFunction(QXmlStreamReader *xml);
    void xmlMessageFunction(QXmlStreamReader *xml);
    void xmlDisplayFunction(QXmlStreamReader *xml);
    void xmlStoreRefFunction();
    void xmlStoreLayoutFunction(QXmlStreamReader *xml);
    void xmlStoreDataFunction(QXmlStreamReader *xml);
    void xmlShowFunction(QXmlStreamReader *xml);
    void xmlModalFunction(QXmlStreamReader *xml);
    void ShowLayout(QString layoutName);
    void xmlBeepFunction(QXmlStreamReader *xml);
    void emitError(QString msg);
};

#endif // LAYOUT_H
