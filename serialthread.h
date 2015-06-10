#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTime>
#include <QDebug>

class serialThread : public QThread
{
    Q_OBJECT

public:
    serialThread(QObject *parent = 0);
    ~serialThread()
    {
        quit = true;
        wait();
    }

    void debug(QString msg)
    {
        emit debugMsg("serialThread: "+msg);
        qDebug() << "serialThread: "+msg;
    }

    bool transaction(const QString &pn, int wt)
    {
        portName = pn;
        waitTimeout = wt;

        debug("start thread running in "+portName);

        if( this->isRunning() ){
            debug("the thread has running , stop it");
            return false;
        }
        serial.setPortName(portName);
        serial.setBaudRate(QSerialPort::Baud115200);
        serial.setDataBits(QSerialPort::Data8);
        serial.setStopBits(QSerialPort::OneStop);
        serial.setParity(QSerialPort::NoParity);
        serial.setFlowControl(QSerialPort::NoFlowControl);

        if (!serial.open(QIODevice::ReadWrite)) {
            emit error(tr("Can't open %1, error code %2")
                       .arg(portName).arg(serial.error()));
            return false;
        }

        quit = false;
        this->start();
        debug("has started thread running in "+portName);
        return true;
    }

    void setquit()
    {
        debug("stop thread running in "+portName);
        quit = true;
        wait();
        debug("stoped thread running in "+portName);

        serial.close();
        debug("close "+portName);
    }

    void run()
    {
        while( ! quit ){
            //serial.write(requestData);
            //if (serial.waitForBytesWritten(waitTimeout))
            debug("waiting for readready in "+portName);
            if (serial.waitForReadyRead(waitTimeout)) {
                QByteArray responseData = serial.readAll();
                debug("readed in "+portName);
                emit response(responseData);
            }else{
                emit timeout(tr("Wait read response timeout %1")
                             .arg(QTime::currentTime().toString()));
            }

        }
    }

signals:
    void response(const QByteArray s);
    void error(const QString s);
    void timeout(const QString s);
    void debugMsg(QString msg);

private:
    QString portName;
    QString request;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
    QSerialPort serial;
};

#endif // SERIALTHREAD_H
