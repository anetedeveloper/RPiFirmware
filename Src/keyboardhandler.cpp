#include "keyboardhandler.h"

void KeyboardHandler::SetStartKey(int startKey)
{
    StartKey = startKey;
}

bool KeyboardHandler::eventFilter(QObject* object,QEvent* event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (started)
        {
            if(keyEvent->key() == 16777220 || keyEvent->key() == 16777221) // cislo klavesy Enter a Enter numericky
            {
                qDebug() << "Card readed: " << CardNumber;
                emit CardReaded("<CardRead TimeStamp=\"" + QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss") + "\" Number=\"" + CardNumber + "\"/>");
                CardNumber = "";
                started = false;
            }
            else
            {
                CardNumber += keyEvent->text();
            }
        }
        else
        {
              if (keyEvent->key() == StartKey)
              {
                  qDebug() << "Card read started";
                  started = true;
              }
              else
              {
                  qDebug() << "Received key without start symbol" << keyEvent->text();
              }
        }
        return true;
    }
    else
    {
        return QObject::eventFilter(object,event);
    }
}
