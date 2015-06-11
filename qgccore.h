#ifndef QGCCORE_H
#define QGCCORE_H

#include <QObject>
#include <serialthread.h>
#include <common/mavlink.h>
#include <QDebug>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#define QGCCORE_MAX_MESSAGE_COUNT 5
#define QGCCORE_MAVLINK_PACKAGE_HEAD_TAG 0xfe

class QgcCore : public QThread
{
    Q_OBJECT

public:
    explicit QgcCore(QObject *parent = 0);

    serialThread *thread ;
    QMutex mMutex ;
    QWaitCondition mCond;
    int mStatus;
    uint8_t qgcSysid=255;
    uint8_t qgcCompid=MAV_COMP_ID_MISSIONPLANNER;

    struct qgcCoreMessageBuffer{ // use for sotre the raw messages frome serial
        int count;
        mavlink_message_t messages[QGCCORE_MAX_MESSAGE_COUNT];
        void init(){
            count = 0;
        }
    }messageBuffer;

    enum QGCSTATUS {
        DISCONNECT = 0,
        CONNECT
    };


    void qgcInit()
    {
        thread = new serialThread(this);
        /*
            connect(thread, SIGNAL(response(QByteArray)),
                    this, SLOT(processResponse(QByteArray)));
                    */
            connect(thread, SIGNAL(error(QString)),
                    this, SLOT(processError(QString)));
            connect(thread, SIGNAL(timeout(QString)),
                    this, SLOT(processTimeout(QString)));
            connect(thread, SIGNAL(debugMsg(QString)),
                    this, SLOT(debug(QString)));

            messageBuffer.init();
            debug("qgccore init");
    }

    void startArm()
    {
        debug("Core start arm");
    }

    bool startConnect(QString portname){
        debug("start thread in core");
        bool ret = true;

        if( mStatus == CONNECT){
            debug("Core : Warn !!   has connected ");
            return false;
        }
        if ( thread->spOpen(portname,10000) )
        {
            mStatus = CONNECT;
            this->start();
            ret = thread->toListenMessage();
        }else{
            debug("Core: open port error");
            ret = false;
        }
        return ret;
    }
    void startDisConnect(){       
        if( mStatus == CONNECT )
        {
            thread->spClose();
            mStatus = DISCONNECT;
            mCond.wakeOne();
            wait();
            debug("Core:  I have Disconnected successfully");
        }else{
            debug("Core: Warnning I have Disconnect");
        }
    }

    void processResponse(QByteArray &data) // this is call by serial thread
    {
        //handleMessage(data);
        mMutex.lock();
        decodeRawDataToMavlink(data);
        mCond.wakeOne();
        mMutex.unlock();
    }

    void run()
    {
        while(mStatus == CONNECT)
        {
            handleMessage();
            mMutex.lock();
            mCond.wait(&mMutex);
            mMutex.unlock();
        }

    }


private:

    void handleHeartbeatMessage(mavlink_heartbeat_t & heartbeat)
    {

        debug("custom_mode="+QString::number(heartbeat.custom_mode)+" , autopilot="+QString::number(heartbeat.autopilot)+", base_mode="+QString::number(heartbeat.base_mode)+", system_status="+QString::number(heartbeat.system_status));
        mavlink_message_t msg;
        mavlink_heartbeat_t heart_sp;

        heart_sp.type = MAV_TYPE_GCS;

        mavlink_msg_heartbeat_encode(qgcSysid, qgcCompid ,&msg, &heart_sp);

        thread->toSendMessage(msg);
    }

    void decodeRawDataToMavlink(QByteArray data){

        int dataIndex = 0;
        int dataSize = data.size();
        int cnt = 0;

        mavlink_message_t *msg;
        char * tmp_p;

        messageBuffer.count = 0;

        for ( cnt = 0; cnt < QGCCORE_MAX_MESSAGE_COUNT; cnt ++)
        {
            msg = &messageBuffer.messages[cnt];
            while(dataIndex < dataSize)
            {
                if( data[dataIndex] == QGCCORE_MAVLINK_PACKAGE_HEAD_TAG )
                {
                    //start to alloc a message
                    memset(msg,0,sizeof(mavlink_message_t));
                    msg->len = data[dataIndex+1];
                    msg->seq = data[dataIndex+2];
                    msg->sysid = data[dataIndex+3];
                    msg->compid = data[dataIndex+4];
                    msg->msgid = data[dataIndex+5];
                    tmp_p =(char*) msg->payload64;
                    for( int i=0; i< msg->len; i++){
                        tmp_p[i]=data.at(dataIndex+6+i);
                    }

                    dataIndex = dataIndex+7+msg->len;
                    messageBuffer.count++;
                    break;
                }else{
                    dataIndex++;
                }
            }

        }

        debug("qgcCore :decode "+QString::number( messageBuffer.count)+" messages ");

    }
    void processMessage( mavlink_message_t message)
    {
        debug("process message id="+QString::number(message.msgid)+", seq="+QString::number(message.seq));
        switch(message.msgid){

        case MAVLINK_MSG_ID_HEARTBEAT:
            mavlink_heartbeat_t heartbeat;
            mavlink_msg_heartbeat_decode(&message,&heartbeat);
            handleHeartbeatMessage(heartbeat);
            break;

        default :
            debug("unspport message type");
            thread->toListenMessage();
            break;

        }
    }
    void handleMessage()
    {

        for( int i=0 ; i < messageBuffer.count; i++){
            processMessage(messageBuffer.messages[i]);
        }


    }





signals:
    void debugmsg(QString msg);
public slots:



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
