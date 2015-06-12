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
//#include <qgccore.h>

#include <common/mavlink.h>

class serialThread : public QObject
{
    Q_OBJECT

private:
        QString portName;
        int waitTimeout;

        QMutex mutex;
        QMutex threadMutex;
        QWaitCondition cond;
        //QgcCore *coreUser;
        QSerialPort serial;
        QByteArray responseData;
        QByteArray resquestData;
        struct _spWriteData{
            char data[300];
            int len;
            int msgid;
            void clear()
            {
                memset(data,0,300);
                len = 0;
                msgid = 0;
            }
        }spWriteData;


        int spStatus;
        enum SPSTATUS{
            SPCLOSE=0,
            SPOPEN,
            SPRUN
        };




public:
    serialThread(QObject *parent = 0);
    ~serialThread()
    { 
        spClose();

    }

    bool spOpen(const QString &pn, int wt)
    {
        bool ret=true;

        mutex.lock();

        portName = pn;
        waitTimeout = wt;

        debug("thread: open seriol..."+portName);

        if( spStatus != SPCLOSE ){
            debug("the thread has running , stop it first");
            ret = false;
            goto out;
        }
        serial.setPortName(portName);
        serial.setBaudRate(QSerialPort::Baud115200);
        serial.setDataBits(QSerialPort::Data8);
        serial.setStopBits(QSerialPort::OneStop);
        serial.setParity(QSerialPort::NoParity);
        serial.setFlowControl(QSerialPort::NoFlowControl);

        serial.setReadBufferSize(sizeof(mavlink_message_t));

        if (!serial.open(QIODevice::ReadWrite)) {
            emit error(tr("Can't open %1, error code %2")
                       .arg(portName).arg(serial.error()));
            ret = false;
            goto out;
        }
        spStatus = SPOPEN;
        debug("OK");
out:
        mutex.unlock();
        return ret;
    }

    bool spClose()
    {
        debug("Thread : to close seriol port ");
        mutex.lock();
        if( spStatus != SPCLOSE ){
            spStatus = SPCLOSE;
            mutex.unlock();
            serial.close();
            debug("Thread : close seriol port ok ");
        }else
            mutex.unlock();

    }

    bool readMessages( QByteArray &data, int waiteTime )
    {
        bool res=true;

        mutex.lock();
        if( spStatus != SPOPEN || !serial.isOpen() )
        {
            debug("Thread : open seriol port first ");
            res = false;
            goto out;
        }

        if (serial.waitForReadyRead(waiteTime)) {
            data.clear();
            data = serial.readAll();
            int size = data.size();
            debug("Thread : readed in "+portName+" read msg size="+QString::number(size,10));
        }else{
            //emit timeout(tr("Wait read response timeout %1").arg(QTime::currentTime().toString()));

            res = false;
            goto out;
        }
  out:
        mutex.unlock();
        return res;
    }

    bool writeOneMessage(mavlink_message_t &msg, int waiteTime)
    {
        bool res = true;
        int len;

        mutex.lock();
        if( spStatus == SPCLOSE){
            debug( "Thread : open the serial first");
            res =  false;
            goto here;
        }
        spWriteData.clear();
        spWriteData.len = mavlink_msg_to_send_buffer((uint8_t *)spWriteData.data,&msg);
        spWriteData.msgid = msg.msgid;

        len = serial.write(spWriteData.data,spWriteData.len);
        if( len != spWriteData.len )
        {
            emit error(tr("write data false , message size=%1, writed size=%2, return %3")
                              .arg(QString::number(spWriteData.len)
                                   .arg(QString::number(len)
                                        .arg(QString::number(len)))));
        }
        serial.flush();
        if (serial.waitForBytesWritten(waiteTime))
        {
            //write ok
            //spWriteData.clear();
            res = true;
            goto here;

        }else{
            emit timeout(tr("Wait writen data timeout %1")
                            .arg(QTime::currentTime().toString()));
            res = false;
            goto here;
        }

    here:

        mutex.unlock();
        return res;
    }
/*
    bool toListenMessage()
    {
        bool ret = true;

        mutex.lock();
        if( spStatus == SPOPEN){
            this->start();
        }else if( spStatus == SPRUN ){
            cond.wakeOne();
        }else{
            debug( "Thread : open the serial first");
            ret = false;
        }
        mutex.unlock();
        return ret;
    }


    bool toSendMessage(mavlink_message_t &msg)
    {
        int len;
        bool ret = true;

        mutex.lock();

        if( spStatus == SPCLOSE){
            debug( "Thread : open the serial first");
            ret = false;
            goto out;
        }

        spWriteData.clear();
        spWriteData.len = mavlink_msg_to_send_buffer((uint8_t *)spWriteData.data,&msg);
        spWriteData.msgid = msg.msgid;

        if( spStatus == SPOPEN){
            this->start();
        }else if( spStatus == SPRUN ){
            cond.wakeOne();
        }else{
            debug( "Thread : the status is error ,check it");
            ret = false;
            goto out;
        }


out:
        mutex.unlock();
        return ret;
    }
*/

    void debug(QString msg)
    {
        emit debugMsg("serialThread: "+msg);
        //qDebug() << "serialThread: "+msg;
    }
/*
    void run()
    {
        bool res  ;
        mutex.lock();
        spStatus = SPRUN;
        mutex.unlock();


        while( spStatus == SPRUN )
        {
            mutex.lock();
            res = false;
            if( spWriteData.len != 0 )
            {
                int len = serial.write(spWriteData.data,spWriteData.len);
                if( len != spWriteData.len )
                {
                    emit error(tr("write data false , message size=%1, writed size=%2, return %3")
                                      .arg(QString::number(spWriteData.len)
                                           .arg(QString::number(len)
                                                .arg(QString::number(len)))));
                }
                serial.flush();
                if (serial.waitForBytesWritten(waitTimeout))
                {
                    //write ok
                    spWriteData.clear();
                    res = true;

                }else{
                    emit timeout(tr("Wait writen data timeout %1")
                                    .arg(QTime::currentTime().toString()));
                    res = false;
                }

                if( !res ) goto loop;
            }







            debug("waiting for readready in "+portName);
            if (serial.waitForReadyRead(waitTimeout)) {
                responseData.clear();
                responseData = serial.readAll();
                int size = responseData.size();
                debug("Thread : readed in "+portName+" read msg size="+QString::number(size,10));
                //coreUser->processResponse(responseData);
                //emit response(responseData);

            }else{
                emit timeout(tr("Wait read response timeout %1")
                             .arg(QTime::currentTime().toString()));

                res = false;
            }


loop:
            if( res ) //if no successfully ,don't sleep
            {
                debug("Thread : I go to sleep");
                cond.wait(&mutex);
            }
            debug("Thread : I work again");
            mutex.unlock();
            msleep(10);

        }
        debug("Thread : I fire the boss");

    }
*/

signals:
    void response(const QByteArray a);
    void error(const QString s);
    void timeout(const QString s);
    void debugMsg(QString msg);


};

#endif // SERIALTHREAD_H
