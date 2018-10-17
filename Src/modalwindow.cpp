#include "modalwindow.h"
ModalWindow::ModalWindow()
{
}

void ModalWindow::SetWindowAsModal(QMainWindow *window)
{
    window->setFixedSize(500, 300);
    window->setStyleSheet("QMainWindow {background:white; border: 2px solid black;}");
    window->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            window->size(),
            qApp->desktop()->availableGeometry(window)
        )
    );
#ifdef new181
    window->setWindowFlags(Qt::FramelessWindowHint);
    window->statusBar()->setSizeGripEnabled(false);
    window->setWindowModality(Qt::ApplicationModal);
#else
    window->setWindowFlags(Qt::Dialog);
#endif
}


void ModalWindow::modalBtnClicked()
{
    //emit MessageToSend(QObject::sender()->objectName());
    emit MessageToSend("<ButtonClick TimeStamp=\"" + QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss") + "\" Id=\"" + QObject::sender()->objectName() + "\"/>");
}

QPushButton *ModalWindow::CreateButtonToModal(QString name, QString text, int width)
{
    QPushButton *btn = new QPushButton;
  //btn->setStyleSheet("font:bold 45px; padding-bottom: 15; padding-top: 15; background: rgb(61, 52, 116); color: white");
    btn->setStyleSheet("font:bold 45px; padding-bottom: 15; padding-top: 15; background: black; color: white;");
    btn->setMinimumWidth(width);
    btn->setMaximumWidth(width);
    btn->setText(text);
    btn->setObjectName(name);
    QObject::connect(btn, SIGNAL( clicked() ), this, SLOT(modalBtnClicked()) );
    return btn;
}

QLabel *ModalWindow::CreateTextToModal(QString text)
{
    QLabel *label = new QLabel;
    label->setStyleSheet("font:bold 25px; color: black; qproperty-alignment: AlignCenter; qproperty-wordWrap:true;");
    label->setText(text);
    return label;
}

bool ModalWindow::CloseWindow()
{
    if(modalWindow == nullptr)
        return false;
    modalWindow->close();
    modalWindow = nullptr;
    return true;
}

bool ModalWindow::ShowModal(QString message, QList<QPair<QString, QString>> buttonsNames)
{
    if (modalWindow != nullptr)
        return false;
    modalWindow = new QMainWindow();
    SetWindowAsModal(modalWindow);

    QList<QPushButton*> buttons;
    int width = modalWindow->width()*0.8/buttonsNames.size();
    foreach (auto btnName, buttonsNames) {
        buttons.append(CreateButtonToModal(btnName.first, btnName.second, width));
    }
    QLabel *label = CreateTextToModal(message);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(label,0,0,1,buttons.size(),Qt::AlignCenter);
    int i = 0;
    foreach (QPushButton* btn, buttons) {
        gridLayout->addWidget(btn, 1, i++, Qt::AlignCenter);
    }

    QWidget *w = new QWidget;
    w->setLayout(gridLayout);
    modalWindow->setCentralWidget(w);
    modalWindow->show();
    return true;
}
