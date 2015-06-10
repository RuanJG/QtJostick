#ifndef QGCCORE_H
#define QGCCORE_H

#include <QObject>

#include <serialthread.h>

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

    void handleMessage()
    {

    }

signals:
    void debugmsg(QString msg);
public slots:

    void processResponse(QByteArray data)
    {
        debug("get a package ");
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
