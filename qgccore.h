#ifndef QGCCORE_H
#define QGCCORE_H

#include <QObject>
//#include <serialthread.h>
#include <common/mavlink.h>
#include <QDebug>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QSerialPort>
#include <QTime>



class QgcCore : public QThread
{
    Q_OBJECT

public:
    explicit QgcCore(QObject *parent = 0);

    //serialThread thread ;
    QMutex mMutex ;
    QWaitCondition mCond;
    uint8_t qgcSysid ;
    uint8_t qgcCompid ;
    int mSerialPortWriteWaitTimeMS ;
    int mSerialPortReadWaitTimeMS ;
    int mLoopWaitTimeMS ; //core rate xxhz
    QSerialPort serial;
    QByteArray mserialData;



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
        DOHEARTBEAT = 1,
        DOGETPARAM = 2
    };
    int mTask;


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
    struct CopterStatus mCopterStatus;


    struct copterParam{
        //char indexTag[488];
        int minIndex ;
        int maxIndex ;
        int nowIndex;
        QMap<QString,float> params;
        void init()
        {
            //memset(indexTag,0,488);
            minIndex = 67;//RC1_MIN
            maxIndex = 92;//RC5_FUNCTION
            nowIndex = 0;
            params.clear();
        }
    };
    struct copterParam mCopterParam;

    struct copterRawChannel{
        int draw;
        int dpitch;
        int droll;
        int dthr;
        mavlink_rc_channels_override_t rc;
        void init(){
            draw=0;
            dpitch=0;
            droll=0;
            dthr=0;
            memset(&rc,0,sizeof(mavlink_rc_channels_override_t));
        }
    };
    struct copterRawChannel mrcChannel;


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

            connect(&thread, SIGNAL(error(QString)),
                    this, SLOT(processError(QString)));
            connect(&thread, SIGNAL(timeout(QString)),
                    this, SLOT(processTimeout(QString)));
            connect(&thread, SIGNAL(debugMsg(QString)),
                    this, SLOT(debug(QString)));*/

            messageBuffer.init();
            mSerialPortWriteWaitTimeMS = 100 ; //ms 100hz
            mSerialPortReadWaitTimeMS = 100;
            mLoopWaitTimeMS = 100; //10hz

            mCopterStatus.init();
            emit copterStatusChanged();
            mTask = 0;
            qgcSysid=255;
            qgcCompid=MAV_COMP_ID_MISSIONPLANNER;
            mCopterParam.init();

            connect(&serial,SIGNAL(readyRead()),
                    this,SLOT(hasDataFromSerial()));

            debug("qgccore init");

    }


    void doSendCommandMessage()
    {

        if ( mTask & DOHEARTBEAT ){
            qDebug() << "CORE:  start do heardbeat task";
            sendHeartBeatMessage();
            mTask &= ~DOHEARTBEAT;
        }
        if( mTask & DOGETPARAM ){

        }


    }


    void startArm(bool arm)
    {
        mMutex.lock();
        if( mStatus != CONNECT ){
            debug("Core: starArm false  , first connect port");
            mMutex.unlock();
            return;
        }
        sendArmMessage(arm);
        mMutex.unlock();
    }
    void startSetMode(int mode)
    {
        mMutex.lock();
        if( mStatus != CONNECT ){
            debug("Core: set Mode false  , first connect port");
            mMutex.unlock();
            return;
        }
        if( mode <= BRAKE && mode >= STABILIZE )
            sendSetModeMessage(mode);
        mMutex.unlock();
    }

    bool startConnect(QString portname){
        debug("start connect in core");
        bool ret = true;
        mMutex.lock();
        if( mStatus == CONNECT || this->isRunning()){
            debug("Core : Warn !!   has connected ");
            ret = true;
            goto out;
        }
        debug("start open seriol");
        if ( openPort(portname) )
        {
            serial.clear();
            mStatus = CONNECT;
            mCopterParam.init();
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
            mCond.wakeOne();
            wait();
            closePort();
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

    void startGetParam()
    {
        mMutex.lock();
        if( mStatus != CONNECT ){
            mMutex.unlock();
            return;
        }


        //sendGetParamListMessage();

        mCopterParam.init();
        mCopterParam.nowIndex = mCopterParam.minIndex;
        sendGetParamMessage();

        mMutex.unlock();

    }

    void startSendRc()
    {

    }



    void writeMessage(mavlink_message_t &msg)
    {
        //bool res = true;
        int len,wlen;
        char data[300];

        len = mavlink_msg_to_send_buffer((uint8_t *)data,&msg);

        wlen = serial.write(data,len);
        if( len != wlen )
        {
            qDebug() << tr("write data not compile , message size=%1, writed size=%2, return %3")
                              .arg(QString::number(len)
                                   .arg(QString::number(wlen)
                                        .arg(QString::number(wlen))));
        }else
        if( wlen == -1 )
        {
            qDebug() << "write message false ";
        }else if( len != wlen ) {
            qDebug() << tr("write data not compile , message size=%1, writed size=%2, return %3")
                              .arg(QString::number(len)
                                   .arg(QString::number(wlen)
                                        .arg(QString::number(wlen))));
        }
        serial.flush();
/*
        if (serial.waitForBytesWritten(1000))
        {
            qDebug() << "write message "+QString::number(wlen)+"B ok ";

        }else{
            qDebug() << "write message time out ";
        }*/


    }
    bool readMessage(QByteArray &data)
    {
        bool res=true;

        //mutex.lock();
        if( !serial.isOpen() )
        {
            debug("Thread : open seriol port first ");
            res = false;
            goto out;
        }

        if (serial.waitForReadyRead(100)) {
            data.clear();
            data = serial.readAll();
            int size = data.size();
            debug("Core read msg size="+QString::number(size,10));
        }else{
            qDebug() << tr("Wait read response timeout %1").arg(QTime::currentTime().toString());

            res = false;
            goto out;
        }
  out:
        //mutex.unlock();
        return res;

    }

    bool openPort(QString portName)
    {
        //bool res ;
        if( serial.isOpen() )
        {
            debug("Core: open port false , this port has opened");
            return false;
        }
        serial.setPortName(portName);

        if (!serial.setDataBits(QSerialPort::Data8))
        {
            debug("set setDataBits false");
            return false;
        }
        if (!serial.setStopBits(QSerialPort::OneStop))
        {
            debug("set setStopBits false");
            return false;
        }
        if (!serial.setParity(QSerialPort::NoParity))
        {
            debug("set setParity false");
            return false;
        }
        if (!serial.setFlowControl(QSerialPort::NoFlowControl))
        {
            debug("set setFlowControl false");
            return false;
        }
        if (!serial.setBaudRate(QSerialPort::Baud115200))
        {
            debug("set Baudrate false");
            return false;
        }
        //serial.setReadBufferSize(sizeof(mavlink_message_t));

        if (!serial.open(QIODevice::ReadWrite)) {
            debug(tr("Can't open %1, error code %2")
                       .arg(portName).arg(serial.error()));
            return false;
        }
        debug("Core: open port "+portName+"ok");
        return true;
    }
    bool closePort()
    {
        if( serial.isOpen() )
        {
            serial.close();
            debug("CORE:  port has closed");
            return true;
        }
        debug("CORE: no port has opened");
        return false;
    }


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

    void sendGetParamListMessage()
    {
        mavlink_param_request_list_t psp;
        mavlink_message_t msg;

        psp.target_component = qgcCompid;
        psp.target_system = qgcSysid;
        mavlink_msg_param_request_list_encode(mCopterStatus.sysid,mCopterStatus.compid,&msg,&psp);

        writeMessage(msg);
        debug("send get param list message ok");

    }

    void sendGetParamMessage()
    {
        mavlink_param_request_read_t p_sp;
        mavlink_message_t msg;

        p_sp.target_component = qgcCompid;
        p_sp.target_system = qgcSysid;
        p_sp.param_index = mCopterParam.nowIndex;

        mavlink_msg_param_request_read_encode(mCopterStatus.sysid,mCopterStatus.compid,&msg,&p_sp);
        writeMessage(msg);
        debug("send get param message ok");

    }

    void sendSetModeMessage(int mode)
    {
        mavlink_set_mode_t mode_sp ;
        mavlink_message_t msg;


        mode_sp.base_mode= mCopterStatus.baseMode;
        mode_sp.custom_mode=  mode;
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




    void run()
    {

        bool res;
        int len;

        //mTask |= DOHEARTBEAT;

        while(true)
        {


            mMutex.lock();

            if( mStatus == DISCONNECT ){
                mMutex.unlock();
                    break ;
            }


            while( !mserialData.isEmpty() )
            {
                if( decodeRawDataToMessage(mserialData,&messageBuffer.messages[0])){
                    messageBuffer.count=1;
                    processMessage(messageBuffer.messages[0]);
                }
            }

            doSendCommandMessage();

            mCond.wait(&mMutex);
            mMutex.unlock();
            //qDebug()<<"I go to sleep";
            //mCond.wait(&mMutex,mLoopWaitTimeMS);
          //msleep(mLoopWaitTimeMS);
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

    void decodeRawDataToMavlink(QByteArray &data){

        int dataIndex = 0;
        int dataSize = data.size();
        int cnt = 0;
        int lestLen;
        char QGCCORE_MAVLINK_PACKAGE_HEAD_TAG = 0xfe;

        mavlink_message_t *msg;
        char * tmp_p;

        messageBuffer.init();

        cnt = 0;
        while(dataIndex < dataSize)
        {
                if( data[dataIndex] == QGCCORE_MAVLINK_PACKAGE_HEAD_TAG )
                {
                    //check the length of the package
                    lestLen = (dataSize-dataIndex) ;
                    if( lestLen < 4 || lestLen < data[dataIndex+1]+8 ){
                        debug("get a part of message");
                        break;
                    }
                    //start to alloc a message
                    msg = &messageBuffer.messages[cnt];
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

                    dataIndex = dataIndex+8+msg->len;
                    messageBuffer.count++;
                    if(messageBuffer.count >= messageBuffer.maxcnt)
                        break;
                    cnt++;
                }else{
                    dataIndex++;
                }
        }

        data.remove(0,dataIndex);
        debug("qgcCore :decode "+QString::number( messageBuffer.count)+" messages ");

    }

    bool decodeRawDataToMessage(QByteArray &data,mavlink_message_t *msg){

        int dataIndex = 0;
        int dataSize = data.size();
        int lestLen,msg_len;
        char QGCCORE_MAVLINK_PACKAGE_HEAD_TAG = 0xfe;

        char * tmp_p;
        char * msg_p;
        char * data_p;

        for( ; dataIndex < dataSize; dataIndex++)
        {
                if( data[dataIndex] == QGCCORE_MAVLINK_PACKAGE_HEAD_TAG )
                {
                    //check the length of the package
                    lestLen = (dataSize-dataIndex) ;
                    msg_len = data[dataIndex+1]+8;
                    if( lestLen < 4 || lestLen < msg_len ){
                        debug("get a part of message");
                        data.clear();
                        return false;
                    }
                    //start to alloc a message
                    data_p = data.data();
                    msg_p = (char *)&msg->magic;
                    memcpy(msg_p,data_p,msg_len);

                    dataIndex = dataIndex+msg_len;
                    break;
                }
        }

        data.remove(0,dataIndex);


    }

    void processMessage( mavlink_message_t &message)
    {
        debug("process message id="+QString::number(message.msgid)+", seq="+QString::number(message.seq));
        switch(message.msgid){
        case MAVLINK_MSG_ID_HEARTBEAT:
        {
            mavlink_heartbeat_t heartbeat;

            mCopterStatus.sysid = message.sysid;
            mCopterStatus.compid = message.compid;
            mavlink_msg_heartbeat_decode(&message,&heartbeat);
            handleHeartbeatMessage(heartbeat);
            break;
        }

        case MAVLINK_MSG_ID_COMMAND_ACK:
        {
            int ack_cmd = mavlink_msg_command_ack_get_command(&message);
            int ack_result = mavlink_msg_command_ack_get_result(&message);
            qDebug() << "get a ack back cmd="+QString::number(ack_cmd)+",res="+QString::number(ack_result);
            break;
        }
        case MAVLINK_MSG_ID_PARAM_VALUE:
        {
            mavlink_param_value_t p_sp;
            mavlink_msg_param_value_decode(&message , &p_sp);
            mCopterParam.params.insert(p_sp.param_id,p_sp.param_value);

            if( mCopterParam.nowIndex >= mCopterParam.minIndex && mCopterParam.nowIndex <= mCopterParam.maxIndex){
                if( p_sp.param_index ==  mCopterParam.nowIndex )
                    mCopterParam.nowIndex++;

                sendGetParamMessage();
            }
            debug("get a param value package,index="+QString::number(p_sp.param_index));
            break;
        }

        default:
        {
            debug("unspport message type");
            break;
        }

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

    void hasDataFromSerial()
    {

        if( this->isRunning() && mStatus == CONNECT )
        {
            mMutex.lock();
           QByteArray data = serial.readAll();
           mserialData.clear();
           mserialData.append(data);
           mCond.wakeOne();
           mMutex.unlock();
        }

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
