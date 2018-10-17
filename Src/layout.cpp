#include "layout.h"
#include <fstream>

Layout::~Layout()
{
    if (ui != nullptr)
        delete ui;
    if (CurrTimeTimer != nullptr)
        delete CurrTimeTimer;
    if (IPUpdateTimer != nullptr)
        delete IPUpdateTimer;
}

Layout::Layout(Config *appConfig, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Layout)
{
    AppConfig = appConfig;
    beep = new BeepSound("16");
    CurrTimeTimer = new QTimer(this);
    IPUpdateTimer = new QTimer(this);
    modalWindow = new ModalWindow();

    ui->setupUi(this);
    statusBar()->hide();
    ui->mainToolBar->hide();
    ui->centralWidget->setContentsMargins(0,0,0,0);

    if(appConfig->isEmulator.isNull())
    {
        // if I set geometry, app window doesn't have border with close button        
        if (appConfig->displayID < QGuiApplication::screens().count())
            setGeometry(QGuiApplication::screens()[appConfig->displayID]->geometry());
        else
            qWarning() << "displayID is greater than number of displays";
        showFullScreen();
        QApplication::setOverrideCursor(Qt::BlankCursor);
    }
    else
    {
        setWindowTitle("Rallo -p " + QString::number(appConfig->portNumber));
        show();
        QStringList parts = appConfig->isEmulator.split("x");
        setFixedSize(((QString)parts.at(0)).toInt(), ((QString)parts.at(1)).toInt());
        //setFixedSize(800, 480);
    }
}

void Layout::Init()
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("ISO-8859-15"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    connect(CurrTimeTimer, SIGNAL(timeout()), this, SLOT(UpdateTime()));
    connect(IPUpdateTimer, SIGNAL(timeout()), this, SLOT(UpdateIpAddressOnLayout()));

    // RFID proxy ctecka
    //serialCard = new SerialCardRead;

    // RFID kodovana MiFare ctecka
    //RFID_RC522 *rfid = new RFID_RC522();
}

void Layout::SetTextsOnLayout(QString message)
{
    EditQObjectAtribute("message", "text", message);
    EditQObjectAtribute("DNSname", "text", QHostInfo::localHostName());
    if (!AppConfig->isEmulator.isNull())
    {
        EditQObjectAtribute("PortNumber", "text", QString::number(AppConfig->portNumber));
    }
    else
    {
        EditQObjectAtribute("PortNumber", "text", " ");
        EditQObjectAtribute("PortNumberTitle", "text", " ");
    }
    EditQObjectAtribute("VersionNumber", "text", VerziaTitle + AppConfig->programVersion);
    EditQObjectAtribute("TimeLast", "text", QDateTime::currentDateTime().toString("hh:mm:ss"));
#if ForRaspberryPi
    if (QDateTime::currentDateTime() > QDateTime::fromString("2019-06", "yyyy-MM"))
        EditQObjectAtribute("gridLayout_2", "rowstretch", "1,1");
#endif
    EditQObjectAtribute("logo", "pixmap", ":/logo.png");
}

void Layout::SetTimersForTime()
{
    UpdateTime();
    UpdateIpAddressOnLayout();
    CurrTimeTimer->start(1000); // every second
    IPUpdateTimer->start(60*1000); // every minute
}

void Layout::ShowMessage(QString message)
{
    qDebug() << "INIT screen with message: " << message;
    ShowLayout(initLayoutName);
    SetTextsOnLayout(message);
    SetTimersForTime();
}

void Layout::StopTimers()
{
    CurrTimeTimer->stop();
    IPUpdateTimer->stop();
}

void Layout::UpdateTime()
{
    EditQObjectAtribute("TimeCurrent", "text", QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void Layout::UpdateIpAddressOnLayout()
{
    QString IPAddresses;
    bool addNewLine = false;
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
        {
            if(addNewLine)
                IPAddresses.append("\n");
            else
                addNewLine = true;
            IPAddresses += address.toString();
        }
    }
    if (IPAddresses.isEmpty())
        IPAddresses = "---.---.---.---";
    EditQObjectAtribute("IPNumber", "text", IPAddresses);
}

void Layout::xmlInitFunction(QXmlStreamReader *xml)
{
    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
    {
        if (storeAttr.name() == "StartKey")
        {
            if (Debug)
                qDebug() << "StartKey - set to: " << storeAttr.value();
            // ToDo KeyboardCardRead->SetStartKey(storeAttr.value().toInt());
        }
        else if (storeAttr.name() == "CurrentTime")
        {
#ifdef ForRaspberryPi
            char c[33];
            strcpy(c,storeAttr.value().toString().insert(0, "sudo date -u ").toLatin1());
            system(c);
            system("sudo cp /usr/share/zoneinfo/Europe/Prague /etc/localtime");
#endif
        }
        else
        {
            qWarning() << "Init - Unknow attribute: " << storeAttr.name();
        }
    }
    Layouts.clear();
    References.clear();
    CurrentLayout.clear();
    qDeleteAll(ui->centralWidget->children());


    while (xml->readNextStartElement())
    {
        if (xml->name() == "StyleSheet")
        {
            setStyleSheet(xml->readElementText());
        }
        else
        {
            if (DebugWarnings)
                qWarning() << "Init - unknown Element: " << xml->name();
        }

    }
    ShowMessage(InitInProgress);
    StopTimers();
}

void Layout::xmlMessageFunction(QXmlStreamReader *xml)
{
    QString title, msg;
    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
    {
        if (storeAttr.name() == "Title")
        {
            title = storeAttr.value().toString();
        }
        else if (storeAttr.name() == "Message")
        {
            msg = storeAttr.value().toString();
        }
        else
        {
            qWarning() << "Message - Unknow attribute: " << storeAttr.name();
        }
    }
    if (title.isEmpty() || title.isNull())
    {
        if (DebugWarnings)
        {
            qWarning() << "Message - Title atribute is missing";
        }
        emitError("Message - Title atribute is missing");
    }
    else if (msg.isEmpty() || msg.isNull())
    {
        if (DebugWarnings)
        {
            qWarning() << "Message - Message atribute is missing";
        }
        emitError("Message - Message atribute is missing");
    }
    else
    {
        if (DebugFunction)
            qDebug() << "Message - title: " << title << " msg: " << msg;
        QMessageBox *msgBox = new QMessageBox( QMessageBox::NoIcon,
                                               title,
                                               msg,
                                               QMessageBox::Ok,
                                               this,
                                               Qt::FramelessWindowHint | Qt::Dialog );
        msgBox->setAttribute( Qt::WA_DeleteOnClose ); //makes sure the msgbox is deleted automatically when closed
        msgBox->setStyleSheet("QMessageBox {background-color: #000000;}"
                          "QMessageBox QLabel {color: #FFFFFF; font-size:20px;}"
                          "QMessageBox QPushButton {border: 2px solid #0000FF; border-radius:5px; font-size:40px; color:#FF0000; background-color: #FF00FF;}");
        msgBox->open();
    }
}

void Layout::xmlDisplayFunction(QXmlStreamReader *xml)
{
    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
    {
        if (storeAttr.name() == "IsEnable")
        {
            if (storeAttr.value().toInt() == 0)
            {
#ifdef ForRaspberryPi
                if (DebugFunction)
                    qDebug() << "Display - Vypnut";
                QProcess proc;
                proc.start("xset dpms force off");
                proc.waitForFinished();
#else
                qWarning() << "Display turn off NOT supported";
#endif
            }
            else
            {
#ifdef ForRaspberryPi
                if (DebugFunction)
                    qDebug() << "Display - Zapnut";
                QProcess proc;
                proc.start("xset dpms force on");
                proc.waitForFinished();
#else
                qWarning() << "Display turn on NOT supported";
#endif
            }
        }
        else
        {
            if (DebugWarnings)
                qWarning() << "Display - Unknow attribute: " << storeAttr.name();
        }
    }
}

void Layout::xmlBeepFunction(QXmlStreamReader *xml)
{
    // default values
    double length = 200, delay = 0; int count = 1;
    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
    {
        if (storeAttr.name() == "Length")
        {
            length = storeAttr.value().toDouble();
        }
        else if (storeAttr.name() == "Delay")
        {
            delay = storeAttr.value().toDouble();
        }
        else if (storeAttr.name() == "Count")
        {
            count = storeAttr.value().toInt();
        }
        else
        {
            if (DebugWarnings)
                qWarning() << "Beep - Unknow attribute: " << storeAttr.name();
        }
    }
    if (DebugFunction)
        qDebug() << "Beep - Length: " << length << " Delay: " << delay << " Count: " << count;
    beep->BeepGPIO(length, delay, count);
}

void Layout::xmlStoreRefFunction()
{
    if (DebugWarnings)
        qWarning("Store Ref - NOT IMPLEMENTED ! ! !!!!!!! !");
    emitError("Store ref Not implemented");
    if (DebugFunction)
        qDebug() << "StoreRef - ukladam referenciu: NOT IMPLEMENTED";
    /*
    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
    {
        if (storeAttr.name() == "Id")
        {
            References.insert(storeAttr.value().toString(), xml->readElementText());
        }
        else
        {
            qDebug() << "Unknow attribute in StoreRef: " << storeAttr.name();
        }
    }

    if (Debug)
    {
        qDebug() << "Data v References: ";
        QMap<QString, QString>::iterator i;
        for (i = References.begin(); i != References.end(); ++i)
            qDebug() << i.key() << ": " << i.value();
    }
    */
}

void Layout::xmlStoreLayoutFunction(QXmlStreamReader *xml)
{
    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
    {
        if (storeAttr.name() == "Id")
        {
            QString layoutData = xml->readElementText();

            /*
            QRegExp regExp = QRegExp("<LoadRef[^>]*Id=\"([^>]*)\"[^>]*>"); // <LoadRef[^>]*?Id="([^>]*?)"[^>]*?>
            int ind = 0;
            while ((ind = regExp.indexIn(layoutData, ind)) != -1)
            {
                if (References.contains(regExp.capturedTexts().at(1)))
                    layoutData.replace(ind, ((QString) regExp.capturedTexts().at(0)).length(), References[regExp.capturedTexts().at(1)]);
                else
                {
                    emitError("need reference for: \"" + regExp.capturedTexts().at(1) + "\" (must set it with <StoreRef> TAG)");
                }
                ind += ((QString) regExp.capturedTexts().at(0)).length();
            }
            */
            Layouts.insert(storeAttr.value().toString(), layoutData);
            if (DebugFunction)
                qDebug() << "StoreLayout - ukladam layout: " << storeAttr.value();
        }
        else
        {
            if (DebugWarnings)
                qWarning() << "StoreLayout - Unknow attribute: " << storeAttr.name();
        }
    }

    if (DebugStoredData)
    {
        qDebug() << "Data v Layouts: ";
        QMap<QString, QString>::iterator i;
        for (i = Layouts.begin(); i != Layouts.end(); ++i)
            qDebug() << i.key() << ": " << i.value();
    }
}

void Layout::xmlStoreDataFunction(QXmlStreamReader *xml)
{
    if (DebugWarnings)
        qWarning() << "StoreData - function    NOT IMPLEMENTED!!!!!";
    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
    {

        if (DebugFunction)
            qDebug() << "StoreData - ukladam id: NOT IMPLEMENTED";
        if (storeAttr.name() == "Name")
        {
            /* ToK ToDo
            QFile file;
                    xml->readElementText()
                    */
        }
    }
}

void Layout::ShowLayout(QString layoutName)
{
    QUiLoader loader;
    QBuffer buf;
    if (Layouts.contains(layoutName))
    {
        buf.setData(Layouts[layoutName].toUtf8());
    }
    else
    {
        if (layoutName == initLayoutName)
        {
            QFile file(":/" + layoutName);
            file.open(QFile::ReadOnly | QFile::Text);
            buf.setData(file.readAll());
            file.close();
            Layouts.insert(layoutName, buf.data());
            // potrebne ju ulozit, inak zaznam o layoutName vytvori pri pocessbutton
            // a pri dalsom vykresleni layoutName rozpozna ze v Layouts je, avsak by bolo prazdne
        }
    }
    QWidget *myWidget = loader.load(&buf, this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(myWidget);

    qDeleteAll(ui->centralWidget->children());

    layout->setContentsMargins(0,0,0,0);

    ui->centralWidget->setLayout(layout);
    ProcessLayoutButtons(Layouts[layoutName]);
    CurrentLayout = layoutName;
}

void Layout::xmlModalFunction(QXmlStreamReader *xml)
{
    QString message;
    QList<QPair<QString, QString>> buttons;
    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
    {
        if (storeAttr.name() == "Text")
        {
            message = storeAttr.value().toString();
            while (xml->readNextStartElement())
            {
                QPair<QString, QString> tmp;
                if (xml->name() == "Button")
                {
                    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
                    {
                        if (storeAttr.name() == "Id")
                        {
                            tmp.first = storeAttr.value().toString();
                        } else if (storeAttr.name() == "Text")
                        {
                            tmp.second = storeAttr.value().toString();
                        } else {
                            if (DebugWarnings)
                                qWarning() << "Invalid attribute in Modal->Button: " << storeAttr.value();
                        }
                    }
                    buttons.append(tmp);
                    xml->skipCurrentElement();
                }
            }
        }
        else
        {
            emitError("Unknow attribute in ModalFunction: " + storeAttr.name().toString());
        }
    }
    modalWindow->ShowModal(message, buttons);
}

void Layout::xmlShowFunction(QXmlStreamReader *xml)
{
    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
    {
        if (storeAttr.name() == "Id")
        {
            QString layoutName = storeAttr.value().toString();
            if (layoutName == CurrentLayout)
            {
                if (DebugFunction)
                    qDebug() << "Show - " + CurrentLayout + " layout allready shown";
                ProcessXmlBodyOfShowLayout(xml);
            }
            else
            {
                if (DebugFunction)
                    qDebug() << "Show - rebuild layout to " + layoutName;
                if (!Layouts.contains(layoutName) && layoutName != initLayoutName)
                {
                    emitError("Did not find layout with Id: " + layoutName);
                }
                else
                {
                    modalWindow->CloseWindow();
                    ShowLayout(layoutName);
                    ProcessXmlBodyOfShowLayout(xml);
                }
            }
        }
        else
        {
            emitError("Unknow attribute in StoreLayout: " + storeAttr.name().toString());
        }
    }
}

void Layout::ParseXmlData(QXmlStreamReader *xml)
{
    while (xml->readNextStartElement())
    {
        if (xml->name() == "RLI")
        {
                        while (xml->readNextStartElement())
                        {
                            if (xml->name() == "Init")
                            {
                                xmlInitFunction(xml);
                                xml->skipCurrentElement();
                            }
                            else if (xml->name() == "Message")
                            {
                                xmlMessageFunction(xml);
                                xml->skipCurrentElement();
                            }
                            else if (xml->name() == "Display")
                            {
                                xmlDisplayFunction(xml);
                                xml->skipCurrentElement();
                            }
                            else if (xml->name() == "Beep")
                            {
                                xmlBeepFunction(xml);
                                xml->skipCurrentElement();
                            }
                            else if (xml->name() == "StoreRef")
                            {
                                xmlStoreRefFunction();
                                xml->skipCurrentElement();
                            }
                            else if (xml->name() == "StoreLayout")
                            {
                                xmlStoreLayoutFunction(xml);
                                xml->skipCurrentElement();
                            }
                            else if (xml->name() == "StoreData")
                            {
                                xmlStoreDataFunction(xml);
                                xml->skipCurrentElement();
                            }
                            else if (xml->name() == "Show")
                            {
                                xmlShowFunction(xml);
                                xml->skipCurrentElement();
                            }
                            else if (xml->name() == "Modal")
                            {
                                qDebug() << "Received modal"; //aaa
                                xmlModalFunction(xml);
                                xml->skipCurrentElement();
                            }
                            else if (xml->name() == "ModalClose")
                            {
                                modalWindow->CloseWindow();
                                xml->skipCurrentElement();
                            }
                            else if (xml->name() == "StorePicture")
                            {
                                qDebug() << "Saving picture";

                                QString picturePath = "";
                                QString pictureName = "";
                                foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
                                {
                                    if (storeAttr.name() == "Path")
                                    {
                                        picturePath = storeAttr.value().toString();
                                    }
                                    else if (storeAttr.name() == "Name")
                                    {
                                        pictureName= storeAttr.value().toString();
                                    }
                                    else
                                    {
                                        if (DebugWarnings)
                                            qWarning() << "SavePicture - Unknow attribute: " << storeAttr.name();
                                    }
                                }
                                if (pictureName.isEmpty())
                                {
                                    if (DebugWarnings)
                                        qWarning() << "SavePicture - picture name is missing";
                                }
                                else
                                {
                                    QString pictureData = xml->readElementText();
                                    qDebug() << "SavePicture - path: " << picturePath << " name: " << pictureName;
                                    QDir dir(picturePath);
                                    if (!dir.exists()){
                                        qDebug() << "Creating folder path: " << picturePath;
                                        dir.mkpath(".");
                                    }
                                    QFile file(picturePath + "/" + pictureName.toUtf8().constData());
                                    file.open(QIODevice::WriteOnly);
                                    file.write(QByteArray::fromHex(pictureData.toLatin1()));
                                    file.close();
                                    qDebug() << "SavePicture - Successfull";
                                }
                                xml->skipCurrentElement();
                            }
                            else
                            {
                                if (DebugWarnings)
                                    qWarning() << "Unknow ELEMENT name: " + xml->name().toString();
                                emitError("Unknow ELEMENT name: " + xml->name().toString());
                                xml->skipCurrentElement();
                            }
                        } // end while loop
        }
        else
        {
            if (DebugWarnings)
                qWarning() << "RLI start tag is missing, founded: " + xml->name().toString();
            emitError("RLI start tag is missing, founded: " + xml->name().toString());
        }
    }
}

void Layout::ProcessXmlBodyOfShowLayout(QXmlStreamReader *xml)
{
    QString modifyList;
    while (xml->readNextStartElement())
    {
        if (xml->name() == "ModifyAttribute")
        {
            modifyList.append(ModifyAttributeFromXML(xml));
            modifyList.append(", ");
        }
        else
        {
            if (DebugWarnings)
                qWarning() << "Unknow element name " << xml->name() << " in Show";
        }
        xml->skipCurrentElement();
    }
    if(DebugFunction && !modifyList.isEmpty())
        qDebug() << "ModifyAttribute - vykonane: " << modifyList;
}

// vracia "title -> value" aby sa mohlo zalogovat do jedneho riadku
QString Layout::ModifyAttributeFromXML(QXmlStreamReader *xml)
{
    QString id, attribute, value;
    foreach (const QXmlStreamAttribute &storeAttr, xml->attributes())
    {
        if (storeAttr.name() == "Id")
        {
            id = storeAttr.value().toString();
        }
        else if (storeAttr.name() == "Attribute")
        {
            attribute = storeAttr.value().toString();
        }
        else if (storeAttr.name() == "Value")
        {
            value = storeAttr.value().toString();
        }
        else if (DebugWarnings)
            qWarning() << "Unknow attribute " << storeAttr.name() << " in ModifyAttribute";
    }
    if (!(id.isEmpty() || attribute.isEmpty() || value.isEmpty()))
    {
        if (!EditQObjectAtribute(id, attribute, value))
        {
            qWarning() << "QObjekt " << id << " was not find in layout: " << CurrentLayout;
            emitError("QObjekt " + id + " was not find in layout: " + xml->name().toString());
        }
        return id + " -> " + value;
    }
    else
    {
        if (DebugWarnings)
            qWarning() << "ModifyAttribute Not all attributes received (Id, Attribute, Value)";
        return "";
    }
}


QString Layout::ProcessLayoutButtons(QString inputLayout)
{
    QRegExp rx = QRegExp("<widget\\s*class=\"QPushButton\"\\s*name=\"([A-Za-z0-9_]+)\"");

    int i = 0;
    while(1)
    {
        i = rx.indexIn(inputLayout, i);
        if (rx.capturedTexts().at(0) == "")
            break;
        i++;
        if (DebugFindButton)
            qDebug() << "Found QPushButton on layout with name: " << rx.capturedTexts().at(1);

        QPushButton *pB = ui->centralWidget->findChild<QPushButton*>(rx.capturedTexts().at(1));
        if (pB != nullptr)
        {
            if (rx.capturedTexts().at(1) == "testPlayAccpetBtn123")
            {
                connect(pB, &QPushButton::clicked, [=](){ qDebug() << "Beep accept"; beep->BeepGPIO(500,0,1); });
            }
            else if (rx.capturedTexts().at(1) == "testPlayErrorBtn123")
            {
                connect(pB, &QPushButton::clicked, [=](){ qDebug() << "Beep error"; beep->BeepGPIO(300,300,3); });
            }
            else
            {
                connect(pB, &QPushButton::clicked, [=](){
                    emit MessageToSend("<ButtonClick TimeStamp=\"" + QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss") + "\" Id=\"" + rx.capturedTexts().at(1) + "\"/>");
                });
            }
        }
        else
        {
            if (DebugWarnings)
                qWarning() << "Did not find button: "<< rx.capturedTexts().at(1) << " in current layout";
        }
    }
    return inputLayout;
}

bool Layout::EditQObjectAtribute(QString id, QString attribute, QString newValue)
{
    QObject *object = ui->centralWidget->findChild<QObject*>(id);
    if (object == nullptr)
    {
        qWarning() << "object " << id << " was not find";
        return false;
    }
/*
 * Zmena triedy pre nastavenie dizajnu sa nepouziva
    if (attribute == "class")
    {
        //if (typeid(*object) == typeid(QWidget))
        //{
            object->setProperty("class", newValue);
        //}
        //else
        //{
        //    qWarning() << "cant set class property to " << id << ", is not QWidget type";
        //}
    }
*/
    if(typeid(*object) == typeid(QLabel))
    {
        if (attribute == "pixmap")
        {
            QPixmap p = QPixmap(newValue);
            ((QLabel*)object)->setProperty("pixmap", p.scaled(250, 1000, Qt::KeepAspectRatio));
        }
        else if (attribute == "visible")
        {
            ((QLabel*)object)->setVisible(QString::compare(newValue, "true",Qt::CaseInsensitive));
        }
        else
        {
            ((QLabel*)object)->setProperty(attribute.toStdString().c_str(), newValue);
        }
    }
    else if(typeid(*object) == typeid(QPushButton))
    {if (attribute == "visible")
        {
            ((QPushButton*)object)->setVisible(QString::compare(newValue, "true",Qt::CaseInsensitive));
        }
        else
            ((QPushButton*)object)->setProperty(attribute.toStdString().c_str(), newValue);
    }
    else if(typeid(*object) == typeid(QToolButton))
    {
        ((QToolButton*)object)->setProperty(attribute.toStdString().c_str(), newValue);
    }
    else if(typeid(*object) == typeid(QHBoxLayout) ||
            typeid(*object) == typeid(QVBoxLayout) ||
            typeid(*object) == typeid(QGridLayout))
    {
        if (attribute == "stretch")
        {
            QStringList layout = newValue.split(",");
            int length = layout.length();
            for (int i = 0; i < length; i++)
            {
                ((QHBoxLayout *)object)->setStretch(i, layout[i].toInt());
            }
        }
        if (attribute == "rowstretch")
        {
            QStringList layout = newValue.split(",");
            int length = layout.length();
            for (int i = 0; i < length; i++)
            {
                ((QGridLayout *)object)->setRowStretch(i, layout[i].toInt());
            }
        }
        if (attribute == "columnstretch")
        {
            QStringList layout = newValue.split(",");
            int length = layout.length();
            for (int i = 0; i < length; i++)
            {
                ((QGridLayout *)object)->setColumnStretch(i, layout[i].toInt());
            }
        }
    }
    else if(typeid(*object) == typeid(QWidget))
    {
        ((QWidget*)object)->setProperty(attribute.toStdString().c_str(), newValue);
    }
    else
    {
        qWarning() << "Unknow type of " << id;
        // neznamy typ a funkcia, tak aspon sa pokusim vykonat pozadovany prikaz
        object->setProperty(attribute.toStdString().c_str(), newValue);
    }
    return true;
}

void Layout::emitError(QString error)
{
    emit MessageToSend("<Error TimeStamp=\"" + QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss") + "\" Message=\"" + error + "\"/>");
}
