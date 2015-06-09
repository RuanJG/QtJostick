#include "qgcui.h"

#if defined (Q_OS_WIN)
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#elif defined (Q_OS_ANDROID)
#endif
#include <QDebug>


QgcUi::QgcUi(QObject *parent) : QObject(parent)
{
    debug("people init");
}

QgcUi::~QgcUi()
{
    debug("people deinit");
}

void QgcUi::debug(QString msg)
{
    qDebug() << msg;
    mdebugmsg = msg;
    emit hasQgcDebugMsg();
}
