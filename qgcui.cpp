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
    mstatus = Disconnect;
    emit qgcStatusChange();

    connect(&mCore, SIGNAL(debugmsg(QString)),
            this, SLOT(debug(QString)));
    connect(&mCore, SIGNAL(copterStatusChanged()),
            this, SLOT(copterStatusChanged()));
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

    if(mstatus!= ARMED)
    {
        debug("armming ...");
        mCore.startArm(true);
    }else{
        debug("disarmming ...");
        mCore.startArm(false);
    }

}

void  QgcUi::qgcConnect()
{
    if( mstatus == Connect )
        return;
    if( mCore.startConnect(mseriolport) ){
        mstatus = Connect;
        emit qgcStatusChange();
    }

}

void  QgcUi::qgcDisconnect()
{
    debug("disconnect ...");

    if( mstatus == Disconnect )
        return;

    mCore.startDisConnect();

}

void  QgcUi::qgcFly()
{
    debug("start fly ...");


}

