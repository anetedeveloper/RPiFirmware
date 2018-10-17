#include "rallo.h"

Rallo::Rallo(Config *appConfig, QWidget *) : networkSession()
{
    this->AppConfig = appConfig;
    layout = new Layout(appConfig);
    tcpServer = new QTcpServer();
}
void Rallo::closeAllConnections()
{
    mutex.lock();
    while (!clientConnection.empty())
    {
        clientConnection.back()->close();
        clientConnection.pop_back();
        //client.close();
    }
    mutex.unlock();
}

Rallo::~Rallo()
{
    qDebug() << "App is closing................";
    if (layout != nullptr)
        delete layout;
    if (networkSession != nullptr)
    {
        networkSession->close();
        delete networkSession;
    }
    if (tcpServer != nullptr)
    {
        tcpServer->close();
        delete tcpServer;
    }
    closeAllConnections();
}

void Rallo::Init(KeyboardHandler *keyboardCardRead)
{
    layout->Init();

    connect(layout, SIGNAL(MessageToSend(QString)), this, SLOT(sendMessage(QString)));
    connect(layout->modalWindow, SIGNAL(MessageToSend(QString)), this, SLOT(sendMessage(QString)));
    connect(keyboardCardRead, SIGNAL(CardReaded(QString)), this, SLOT(sendMessage(QString)));
#ifdef ForRaspberryPi
    // SerialCardRead
    //connect(layout->serialCard, SIGNAL(CardReaded(QString)), this, SLOT(sendMessage(QString)));
    //layout->serialCard->StartReading();
    QProcess proc;
    proc.start("xset s off");
    proc.waitForFinished();
    proc.start("xset -dpms");
    proc.waitForFinished();
    proc.start("xset s noblank");
    proc.waitForFinished();
#endif
}

void Rallo::Start()
{

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config);
        QObject::connect(networkSession, &QNetworkSession::opened, this, &Rallo::sessionOpened);

        // zavola to tiez sessionOpened() cez slot
        networkSession->open();
    } else {
        sessionOpened();
    }
    QObject::connect(tcpServer, &QTcpServer::newConnection, this, &Rallo::newConnectionHandler);
    qDebug() << "Started listening on TCP port: " << AppConfig->portNumber;
}

void Rallo::sessionOpened()
{
    // Save the used configuration
    if (networkSession) {
        QNetworkConfiguration netConfig = networkSession->configuration();
        QString id;
        if (netConfig.type() == QNetworkConfiguration::UserChoice)
            id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = netConfig.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }

    if (!tcpServer->listen(QHostAddress::Any, AppConfig->portNumber))
    {
        layout->ShowMessage(layout->UnableToStartServer + tcpServer->errorString());
    }
    else
    {
        layout->ShowMessage(layout->WaitingForServerToConnect);
    }
}


void Rallo::newConnectionHandler()
{
    layout->ShowMessage(layout->NewConnectionEstablished + tcpServer->errorString());
    if (clientConnection.size() != 0)
    {
        qWarning() << "New connection request, while I am connected to another server -> Closing current connection";
    }
    closeAllConnections();
    layout->StopTimers();
    inputData.clear();
    mutex.lock();
    clientConnection.push_back(tcpServer->nextPendingConnection());
    qDebug() << "Novy server sa pripojil s IP: " + clientConnection.back()->peerAddress().toString() + " name: " + clientConnection.back()->peerName();

    connect(clientConnection.back(), SIGNAL(readyRead()), this, SLOT(readFromServer()));
    connect(clientConnection.back(), SIGNAL(disconnected()), this, SLOT(clientDisconected()));
    mutex.unlock();
}

void Rallo::readFromServer()
{
    if (clientConnection.empty())
    {
        inputData = "";
        return;
    }
    while (clientConnection.back()->bytesAvailable())
    {
        inputData += QString::fromLocal8Bit(clientConnection.back()->readAll());
    }
    if (layout->DebugIOData)
        qDebug() << "Receiving data started";
    if (layout->DebugIOData)
        qDebug() << "Stored data + Received: " << inputData;

    while (1)
    {
        int startIndex = inputData.indexOf("<RLI");
        if (startIndex == -1)
        {
            inputData.clear();
            if (layout->DebugWarnings)
                qWarning() << "No START tag <RLI>";
            break;
        }
        else if (startIndex > 0)
        {
            if (layout->DebugWarnings)
                qWarning() << "Removed " << startIndex << "x first characters";
            inputData = inputData.remove(0,startIndex);
        }

        // ak sprava neobsahuje koncovy TAG /RLI
        int endIndex = inputData.indexOf("</RLI>");
        if (endIndex == -1)
            break;

        QXmlStreamReader *xml = new QXmlStreamReader(inputData.mid(0, endIndex));
        layout->ParseXmlData(xml);
        delete(xml);

        // orez priate data ktore boli spracovane a pokracuj so zvyskom spravy
        inputData = inputData.mid(endIndex+6);
        if (inputData.isEmpty())
            break;
    }
}

bool Rallo::SendMessageToServer(QString message)
{
    if (clientConnection.empty())
        return false;
    if (clientConnection.back()->write(message.toLocal8Bit()) == -1)
    {
        if (layout->DebugIOData)
            qDebug() << "SendResponse Local8Bit error: " << message;
        return false;
    }
    else
    {
        if (layout->DebugIOData)
            qDebug() << "SendResponse Local8Bit: " << message;
        return true;
    }
}


void Rallo::clientDisconected()
{
    layout->modalWindow->CloseWindow();
    layout->ShowMessage(layout->ServerWasDisconnected);
}
void Rallo::sendMessage(QString msg)
{
    SendMessageToServer("<RLO>" + msg + "</RLO>");
}
