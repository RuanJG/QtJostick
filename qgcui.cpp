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

    mLastKey = 0;
    rcSteup = 200;
    rcTrim = 10;
    rcChangeMask = 0x0;//8 channel is enable

    connect(&mCore, SIGNAL(debugmsg(QString)),
            this, SLOT(debug(QString)));
    connect(&mCore, SIGNAL(copterStatusChanged()),
            this, SLOT(copterStatusChanged()));
    connect(&mCore, SIGNAL(qgcRcChange()),
            this, SLOT(qgcCoreRcChange()));


    emit qgcStatusChange();
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

    if(! isArmed())
    {
        debug("armming ...");
        mCore.startArm(true);
    }

}
void QgcUi::qgcDisArm()
{
    if( isArmed() ){
            debug("disarmming ...");
            mCore.startArm(false);
        }
}

void  QgcUi::qgcConnect()
{
    if( isConnect() )
        return;
    if( mCore.startConnect(mseriolport,mserialBaud) ){
        debug("QgcUI: startConnect ok");
    }else
        debug("QgcUI: startConnect false");

}

void  QgcUi::qgcDisconnect()
{
    debug("disconnect ...");

    if( !isConnect() )
        return;

    qgcDisArm();
    mCore.startDisConnect();

}

void  QgcUi::qgcFly()
{


}

