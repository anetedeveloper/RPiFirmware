#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTime>
#include <QtWidgets>
#include <QtNetwork>
#include <stdlib.h>
#include <QUiLoader>
#include <QDebug>
#include <QLabel>
#include <QFile>
#include <QBuffer>
#include <typeinfo>
#include <QMutex>

#include "layout.h"
#include "config.h"



class Rallo : public QObject
{
    Q_OBJECT

public:
    Rallo(Config *AppConfig, QWidget *parent = 0);
    ~Rallo();
    Layout *layout;
    void Init(KeyboardHandler *keyboardCardRead);

    vector<QTcpSocket*> clientConnection;
private slots:
    void Start();
    void sessionOpened();
    void readFromServer();
    void newConnectionHandler();
    void clientDisconected();
    void sendMessage(QString msg);

private:
    Config *AppConfig;
    QString inputData;
    bool SendMessageToServer(QString message);
    QTcpServer *tcpServer;
    QMutex mutex;
    QNetworkSession *networkSession;
    void closeAllConnections();
};

#endif // RALLO_H
