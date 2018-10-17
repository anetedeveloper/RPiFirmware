#ifndef MODALWINDOW_H
#define MODALWINDOW_H

#include <QPushButton>
#include <QMainWindow>
#include <QDebug>
#include <QLabel>
#include <QStyle>
#include <QApplication>
#include <QDesktopWidget>
#include <QStatusBar>
#include <QGridLayout>
#include <QDateTime>


class ModalWindow : public QObject
{
    Q_OBJECT

private:
    QMainWindow *modalWindow = nullptr;
    void SetWindowAsModal(QMainWindow *window);
    QPushButton *CreateButtonToModal(QString name, QString text, int width);
    QLabel *CreateTextToModal(QString text);
private slots:
    void modalBtnClicked();
signals:
    void MessageToSend(QString msg);
public:
    ModalWindow();
    bool ShowModal(QString message, QList<QPair<QString, QString>> buttonsNames);
    bool CloseWindow();
};

#endif // MODALWINDOW_H
