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
    mstatus = "Disconnect";
    emit qgcStatusChange();

    connect(&mCore, SIGNAL(debugmsg(QString)),
            this, SLOT(debug(QString)));

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

QList<QString> QgcUi::getSeriolPortList()
{
    QList<QString> list;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        //portlist->addItem(info.portName());
        list.append(info.portName());
        debug(info.portName());
    }
    return list;

}

void QgcUi::qgcArm()
{
    debug("armming ...");
    mCore.startArm();
    mstatus = "Armed";
    emit qgcStatusChange();
}

void  QgcUi::qgcConnect()
{

        debug("qgcui: start mcore connect");
        if( mCore.startConnect(mseriolport) )
        {
          mstatus = "Connected";
           emit qgcStatusChange();
        }else{
            debug("qgccore connect false");
            mstatus = "Disconnect";
            emit qgcStatusChange();
        }

}

void  QgcUi::qgcDisconnect()
{
    debug("disconnect ...");
    mCore.startDisConnect();

    mstatus = "Disconnect";
    emit qgcStatusChange();
}

void  QgcUi::qgcFly()
{
    debug("start fly ...");
    mstatus = "Flying";
    emit qgcStatusChange();

}

