#ifndef QGCCORE_H
#define QGCCORE_H

#include <QObject>
#include <serialthread.h>
#include <common/mavlink.h>
#include <QDebug>
#include <QMutex>

#define QGCCORE_MAX_MESSAGE_COUNT 5
#define QGCCORE_MAVLINK_PACKAGE_HEAD_TAG 0xfe

class QgcCore : public QObject
{
    Q_OBJECT

public:
    explicit QgcCore(QObject *parent = 0);


    serialThread thread;
    struct qgcCoreMessageBuffer{ // use for sotre the raw messages frome serial
        int count;
        mavlink_message_t messages[QGCCORE_MAX_MESSAGE_COUNT];
    }messageBuffer;
    QMutex qgcMutex;
    uint8_t qgcSysid=255;
    uint8_t qgcCompid=MAV_COMP_ID_MISSIONPLANNER;


    void startArm()
    {
        debug("Core start arm");
    }

    bool startConnect(QString portname){
        debug("start thread in core");
        bool ret = true;
        if ( thread.spOpen(portname,10000) )
        {
            ret = thread.toListenMessage();
        }else{
            debug("Core: open port error");
            ret = false;
        }
        return ret;
    }
    void startDisConnect(){
        thread.spClose();
    }



private:

    void handleHeartbeatMessage(mavlink_heartbeat_t & heartbeat)
    {

        debug("custom_mode="+QString::number(heartbeat.custom_mode)+" , autopilot="+QString::number(heartbeat.autopilot)+", base_mode="+QString::number(heartbeat.base_mode)+", system_status="+QString::number(heartbeat.system_status));
        mavlink_message_t msg;
        mavlink_heartbeat_t heart_sp;

        heart_sp.type = MAV_TYPE_GCS;

        mavlink_msg_heartbeat_encode(qgcSysid, qgcCompid ,&msg, &heart_sp);

        thread.toSendMessage(msg);
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
            thread.toListenMessage();
            break;

        }
    }
    void handleMessage(const QByteArray &data)
    {
        qgcMutex.lock();

        decodeRawDataToMavlink(data);
        for( int i=0 ; i < messageBuffer.count; i++){
            processMessage(messageBuffer.messages[i]);
        }

        qgcMutex.unlock();

    }





signals:
    void debugmsg(QString msg);
public slots:

    void processResponse(QByteArray data)
    {
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
