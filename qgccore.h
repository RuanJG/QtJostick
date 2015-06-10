#ifndef QGCCORE_H
#define QGCCORE_H

#include <QObject>

#include <serialthread.h>
#include <common/mavlink.h>
#include <QDebug>

class QgcCore : public QObject
{
    Q_OBJECT

public:
    explicit QgcCore(QObject *parent = 0);


    serialThread thread;


    void startArm()
    {
        debug("Core start arm");
    }

    bool startConnect(QString portname){
        debug("start thread in core");
        bool res = thread.transaction(portname,10000);
        return res;
    }
    void startDisConnect(){
        thread.setquit();
    }


    void handleMessage(QByteArray data)
    {
        mavlink_heartbeat_t heartbeat;
        heartbeat.base_mode=0;

        mavlink_message_t *message;

        char * rawdata = data.data();
        message = (mavlink_message_t) rawdata;
        mavlink_msg_heartbeat_decode(message,&heartbeat);

    }

signals:
    void debugmsg(QString msg);
public slots:

    void processResponse(QByteArray data)
    {
        debug("get a package ");
        handleMessage(data);
    }

    void processError(QString msg)
    {
        debug(msg);
    }

    void processTimeout(QString msg)
    {
        debug(msg);
    }
    void debug(QString msg){
        emit debugmsg(msg);
    }



};

#endif // QGCCORE_H
