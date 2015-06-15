#ifndef QGCCORE_H
#define QGCCORE_H

#include <QObject>
#include <serialthread.h>
#include <common/mavlink.h>
#include <QDebug>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>


struct CopterStatus {
    int customMode; //_autopilot_modes ->above
    int baseMode; //MAV_MODE_FLAG arm / disarm ...
    int targetType; //MAV_TYPE {MAV_TYPE_GCS,MAV_TYPE_QUADROTOR...}
    int boardType; //MAV_AUTOPILOT
    int systemStatus; //MAV_STATE init/boot/standby ...
    int mavlinkVersion;
    int sysid;
    int compid;
    int BADVALUE;
    void init()
    {
        BADVALUE = -1;
        customMode = BADVALUE;
        targetType = BADVALUE;
        boardType = BADVALUE;
        systemStatus = BADVALUE;
        mavlinkVersion = BADVALUE;
        sysid = BADVALUE;
        compid = BADVALUE;
        baseMode = BADVALUE;
    }
};

class QgcCore : public QThread
{
    Q_OBJECT

public:
    explicit QgcCore(QObject *parent = 0);

    serialThread thread ;
    QMutex mMutex ;
    QWaitCondition mCond;
    uint8_t qgcSysid ;
    uint8_t qgcCompid ;
    int mSerialPortWriteWaitTimeMS ;
    int mSerialPortReadWaitTimeMS ;
    int mLoopWaitTimeMS ; //core rate xxhz



    struct qgcCoreMessageBuffer{ // use for sotre the raw messages frome serial
        int count;
        int maxcnt;
        mavlink_message_t messages[5];
        void init(){
            count = 0;
            maxcnt=5;
        }
    }messageBuffer;

    //qgcCore status
    int mStatus;
    enum QGCSTATUS {
        DISCONNECT = 0,
        CONNECT
    };

    //copter fly mode
    enum autopilotModes {
        STABILIZE =     0,  // manual airframe angle with manual throttle
        ACRO =          1,  // manual body-frame angular rate with manual throttle
        ALT_HOLD =      2,  // manual airframe angle with automatic throttle
        AUTO =          3,  // fully automatic waypoint control using mission commands
        GUIDED =        4,  // fully automatic fly to coordinate or fly at velocity/direction using GCS immediate commands
        LOITER =        5,  // automatic horizontal acceleration with automatic throttle
        RTL =           6,  // automatic return to launching point
        CIRCLE =        7,  // automatic circular flight with automatic throttle
        LAND =          9,  // automatic landing with horizontal position control
        OF_LOITER =    10,  // deprecated
        DRIFT =        11,  // semi-automous position, yaw and throttle control
        SPORT =        13,  // manual earth-frame angular rate control with manual throttle
        FLIP =         14,  // automatically flip the vehicle on the roll axis
        AUTOTUNE =     15,  // automatically tune the vehicle's roll and pitch gains
        POSHOLD =      16,  // automatic position hold with manual override, with automatic throttle
        BRAKE =        17   // full-brake using inertial/GPS system, no pilot input
    };

    enum QgcTaskType{
        DOARM = 1,
        DODISARM = 2,
        DOSETMODE = 4,
        DOHEARTBEAT = 8
    };
    int mTask;
    int mcopterMode;

    struct CopterStatus mCopterStatus;


    ~QgcCore()
    {
        debug("QGC deinit");

    }

    void qgcInit()
    {
        //thread = new serialThread(this);
        /*
            connect(&thread, SIGNAL(response(QByteArray)),
                    this, SLOT(processResponse(QByteArray)));
                    */
            connect(&thread, SIGNAL(error(QString)),
                    this, SLOT(processError(QString)));
            connect(&thread, SIGNAL(timeout(QString)),
                    this, SLOT(processTimeout(QString)));
            connect(&thread, SIGNAL(debugMsg(QString)),
                    this, SLOT(debug(QString)));

            messageBuffer.init();
            mSerialPortWriteWaitTimeMS = 100 ; //ms 100hz
            mSerialPortReadWaitTimeMS = 100;
            mLoopWaitTimeMS = 100; //10hz

            mCopterStatus.init();
            mTask = 0;
            mcopterMode = STABILIZE;
            qgcSysid=255;
            qgcCompid=MAV_COMP_ID_MISSIONPLANNER;

            debug("qgccore init");

    }


    void doSendCommandMessage()
    {
        if( mTask & DOARM ){
            qDebug() << "CORE:  start do arm task";
            sendArmMessage(true);
            mTask &= ~DOARM;
            //mcopterMode = ALT_HOLD;
            //sendSetModeMessage();
        }else if (mTask & DODISARM){
            qDebug() << "CORE:  start do disarm task";
            sendArmMessage(false);
            mTask &= ~DODISARM;
        }else if ( mTask & DOHEARTBEAT ){
            qDebug() << "CORE:  start do heardbeat task";
            sendHeartBeatMessage();
            mTask &= ~DOHEARTBEAT;
        }


    }


    void startArm(bool arm)
    {
        mMutex.lock();
        if ( arm )
            mTask |= DOARM;
        else
            mTask |= DODISARM;
        mMutex.unlock();
    }
    void startSetMode(enum autopilotModes mode)
    {
        mcopterMode = mode;
        mTask |= DOSETMODE;
    }

#if 0
    void writeMessage(mavlink_message_t &msg)
    {
        bool res = true;
        int len,wlen;
        char data[300];

        len = mavlink_msg_to_send_buffer((uint8_t *)data,&msg);

        wlen = thread.serial.writeData(data,len);
        if( len != wlen )
        {
            qDebug() << tr("write data false , message size=%1, writed size=%2, return %3")
                              .arg(QString::number(len)
                                   .arg(QString::number(wlen)
                                        .arg(QString::number(wlen))));
        }
        thread.serial.flush();
        if (thread.serial.waitForBytesWritten(100))
        {
            qDebug() << "write message ok ";

        }else{
            qDebug() << "write message time out ";
        }

    }
#else

    bool writeMessage(mavlink_message_t &msg)
    {
        int retry=10;

        for ( retry = 10; retry > 1; retry--){
            if( thread.writeOneMessage(msg,mSerialPortWriteWaitTimeMS) )
                break;
            debug("send change mode msg retry="+QString::number(retry));
        }
        if( retry <= 1 ){
            debug("send change mode msg false");
            return false;
        }else{
            debug("send change mode msg ok");
            return true;
        }

    }
#endif

    void sendArmMessage(bool arm)
    {
        mavlink_command_long_t sp;
        mavlink_message_t msg;

        qDebug() << "sendArmMesage";
        sp.command = MAV_CMD_COMPONENT_ARM_DISARM;
        sp.target_system = qgcSysid;//control_data.system_id;
        sp.target_component = MAV_COMP_ID_MISSIONPLANNER;
        if( arm ){
            sp.param1=1;
        }else{
            sp.param1=0;
        }
        mavlink_msg_command_long_encode(mCopterStatus.sysid ,mCopterStatus.compid,&msg,&sp);


        writeMessage(msg);


    }

    void sendSetModeMessage()
    {
        mavlink_set_mode_t mode_sp ;
        mavlink_message_t msg;


        mode_sp.base_mode= mCopterStatus.baseMode;
        mode_sp.custom_mode=  mcopterMode;
        mavlink_msg_set_mode_encode(mCopterStatus.sysid ,mCopterStatus.compid,&msg,&mode_sp);

        writeMessage(msg);

    }

    void sendHeartBeatMessage()
    {
        mavlink_message_t msg;
        mavlink_heartbeat_t heart_sp;

        heart_sp.type = MAV_TYPE_GCS;
        mavlink_msg_heartbeat_encode(mCopterStatus.sysid ,mCopterStatus.compid ,&msg, &heart_sp);
        writeMessage(msg);
    }





    bool startConnect(QString portname){
        debug("start thread in core");
        bool ret = true;
        mMutex.lock();
        if( mStatus == CONNECT || this->isRunning()){
            debug("Core : Warn !!   has connected ");
            ret = true;
            goto out;
        }
        debug("start open seriol");
        if ( thread.spOpen(portname,mSerialPortReadWaitTimeMS) )
        {
            mStatus = CONNECT;
            this->start();
            debug("Core : start my thread ");
        }else{
            debug("Core: open port error");
            ret = false;
        }
      out:
        mMutex.unlock();
        return ret;
    }
    bool startDisConnect(){
        mMutex.lock();
        if( mStatus == CONNECT )
        {
            mStatus = DISCONNECT;
            mMutex.unlock();
            wait();
            thread.spClose();
            mCopterStatus.init();
            emit copterStatusChanged();
            debug("Core:  I have Disconnected successfully");
            return true;
        }else{
            mMutex.unlock();
            debug("Core: Warnning I have Disconnect");
            return false;
        }
    }


    void run()
    {
        QByteArray serialData;
        bool res;

        //mTask |= DOHEARTBEAT;

        while(mStatus)
        {

            mMutex.lock();

            doSendCommandMessage();

            res=thread.readMessages(serialData,mSerialPortReadWaitTimeMS);
            if( res ){
                decodeRawDataToMavlink(serialData);
                handleMessage();
            }

            mMutex.unlock();
            //qDebug()<<"I go to sleep";
            //mCond.wait(&mMutex,mLoopWaitTimeMS);
            msleep(mLoopWaitTimeMS);
            //qDebug()<<"I go to work";



        }
        debug("Core: I fired the boss");

    }


private:

    void handleHeartbeatMessage(mavlink_heartbeat_t & heartbeat)
    {

        //debug("custom_mode="+QString::number(heartbeat.custom_mode)+" , autopilot="+QString::number(heartbeat.autopilot)+", base_mode="+QString::number(heartbeat.base_mode)+", system_status="+QString::number(heartbeat.system_status));
        //mavlink_message_t msg;
        //mavlink_heartbeat_t heart_sp;

        mCopterStatus.baseMode = heartbeat.base_mode;
        mCopterStatus.boardType = heartbeat.autopilot;
        mCopterStatus.customMode = heartbeat.custom_mode;
        mCopterStatus.systemStatus = heartbeat.system_status;
        mCopterStatus.targetType = heartbeat.type;
        mCopterStatus.mavlinkVersion = heartbeat.mavlink_version;
        emit copterStatusChanged();

        //heart_sp.type = MAV_TYPE_GCS;
        //mavlink_msg_heartbeat_encode(qgcSysid, qgcCompid ,&msg, &heart_sp);
        //thread.writeOneMessage(msg,mSerialPortWriteWaitTimeMS);

        mTask |= DOHEARTBEAT;
    }

    void decodeRawDataToMavlink(QByteArray data){

        int dataIndex = 0;
        int dataSize = data.size();
        int cnt = 0;
        char QGCCORE_MAVLINK_PACKAGE_HEAD_TAG = 0xfe;

        mavlink_message_t *msg;
        char * tmp_p;

        messageBuffer.init();

        for ( cnt = 0; cnt < messageBuffer.maxcnt; cnt ++)
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

            mCopterStatus.sysid = message.sysid;
            mavlink_msg_heartbeat_decode(&message,&heartbeat);
            handleHeartbeatMessage(heartbeat);
            break;

        default :
            debug("unspport message type");
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
    void copterStatusChanged();
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
