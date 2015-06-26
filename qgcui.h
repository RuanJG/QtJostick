#ifndef QGCUI_H
#define QGCUI_H

#include <QObject>
#include <qgccore.h>

class QgcUi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString debugmsg READ getQgcDebugMsg WRITE setQgcDebugMsg NOTIFY hasQgcDebugMsg)
    Q_PROPERTY(QString seriolport READ getQgcSeriolport WRITE setQgcSeriolport)// NOTIFY qgcSeriolportChange)
    Q_PROPERTY(QString status NOTIFY qgcStatusChange)
    Q_PROPERTY(QString rc NOTIFY qgcRcChange)

public:
    explicit QgcUi(QObject *parent = 0);
    ~QgcUi();

    //Q_INVOKABLE void print(void);
    //for output debug message
    QString getQgcDebugMsg(){  return mdebugmsg;  }
    void setQgcDebugMsg(QString msg){  mdebugmsg=msg;  }

    QString getQgcSeriolport(){ return mseriolport; }
    void setQgcSeriolport(QString msg) { mseriolport = msg ; debug("set seriolport "+mseriolport); }



    int pitch;
    int roll;
    int yaw;
    int thr;
    int mLastKey;
    int rcSteup;
    int rcTrim;

    char rcChangeMask;


signals:
    void hasQgcDebugMsg();
    void qgcSeriolportListChange();
    void qgcStatusChange();
    void qgcRcChange();

public slots:
    void debug(QString msg);
    QList<QString> getSeriolPortList();
    void qgcArm();
    void qgcDisArm();
    void qgcConnect();
    void qgcDisconnect();
    void qgcFly();
    void qgcCoreRcChange()
    {
        emit qgcRcChange();
    }

    void qgcSetMode(int mode)
    {
        if( getCopterMode() == mode)
            return;
        if ( mode >= mCore.STABILIZE && mode <= mCore.BRAKE )
            mCore.startSetMode(mode);
    }

    //slot for qgcCore
    void copterStatusChanged()
    {
        debug ("QGCUI:　get status change");
        debug("custom_mode="+QString::number(mCore.mCopterStatus.customMode)+
              " , autopilot="+QString::number(mCore.mCopterStatus.boardType)+
              ", base_mode="+QString::number(mCore.mCopterStatus.baseMode)+
              ", system_status="+QString::number(mCore.mCopterStatus.systemStatus));

        emit qgcStatusChange();
    }
    bool isConnect()
    {
        if( mCore.mCopterStatus.systemStatus == mCore.mCopterStatus.BADVALUE){
            return false; //Disconnect;
        }else{
            return true;
        }
    }
    bool isArmed()
    {
        if( (mCore.mCopterStatus.baseMode & MAV_MODE_FLAG_SAFETY_ARMED) != 0 &&  isConnect() )
            return true;
        else
            return false;
    }
    bool isSendingRc()
    {
        return mCore.mCopterRc.sending;
    }

    void qgcSendRc()
    {
        if( isSendingRc() ){
            debug("stop sending rc  ...");
            mCore.stopSendRc();
        }else{
            debug("start sending rc ...");
            mCore.startSendRc();
        }
    }

    int getCopterMode()
    {
        if( ! isConnect() )
            return mCore.STABILIZE;
        return mCore.mCopterStatus.customMode;
    }
    void updateParam()
    {
        if( isConnect()){
            mCore.startGetParam();
        }
    }

    void qgcGetKey(int key)
    {
        int val;

        if(! mCore.mCopterRc.enable )
            return;

        if( key == mLastKey )
            return;

        switch(key)
        {
//roll sub
        case Qt::Key_J:
            val = getRoll()- rcTrim;
            if( val >= getRollMin() && val <= getRollMax() )
                mCore.startChangeRcByValue(val,getPitch(),getThr(),getYaw());
            break;
        case Qt::Key_1:
            roll = getRoll();
            val = roll - rcSteup;
            if( val >= getRollMin() && val <= getRollMax() )
                mCore.startChangeRcByValue(val,getPitch(),getThr(),getYaw());
            //mCore.startChangeRcBySteup(-1,0,0,0);
            break;
//roll add
        case Qt::Key_L:
            val = getRoll() + rcTrim;
            if( val >= getRollMin() && val <= getRollMax() )
                mCore.startChangeRcByValue(val,getPitch(),getThr(),getYaw());
            break;
        case Qt::Key_3:
            roll = getRoll();
            val = roll + rcSteup;
            if( val >= getRollMin() && val <= getRollMax() )
                mCore.startChangeRcByValue(val,getPitch(),getThr(),getYaw());

            //mCore.startChangeRcBySteup(1,0,0,0);
            break;

//pitch add
        case Qt::Key_I:
            val = getPitch() + rcTrim;
            if( val >= getPitchMin() && val <= getPitchMax())
                mCore.startChangeRcByValue(getRoll(),val,getThr(),getYaw());
            break;
        case Qt::Key_4:
            pitch = getPitch();
            val = pitch + rcSteup;
            if( val >= getPitchMin() && val <= getPitchMax())
                mCore.startChangeRcByValue(getRoll(),val,getThr(),getYaw());
            //mCore.startChangeRcBySteup(0,1,0,0);
            break;
//pitch sub
        case Qt::Key_K:
            val = getPitch() - rcTrim;
            if( val >= getPitchMin() && val <= getPitchMax())
                mCore.startChangeRcByValue(getRoll(),val,getThr(),getYaw());
            break;
        case Qt::Key_2:
            pitch = getPitch();
            val = pitch - rcSteup;
            if( val >= getPitchMin() && val <= getPitchMax())
                mCore.startChangeRcByValue(getRoll(),val,getThr(),getYaw());
            //mCore.startChangeRcBySteup(0,-1,0,0);
            break;


//yaw sub
        case Qt::Key_A:
            yaw = getYaw();
            val = yaw - rcSteup;
            if( val >= getYawMin() && val <= getYawMax())
                mCore.startChangeRcByValue(getRoll(),getPitch(),getThr(),val);
            break;
        case Qt::Key_Left:
            //mCore.startChangeRcBySteup(0,0,0,-1);
            val = getYaw() - rcTrim;
            if( val >= getYawMin() && val <= getYawMax())
                mCore.startChangeRcByValue(getRoll(),getPitch(),getThr(),val);
            break;

//yaw add
        case Qt::Key_D:
            yaw = getYaw();
            val = yaw + rcSteup;
            if( val >= getYawMin() && val <= getYawMax())
                mCore.startChangeRcByValue(getRoll(),getPitch(),getThr(),val);
            break;
        case Qt::Key_Right:
            val = getYaw() + rcTrim;
            if( val >= getYawMin() && val <= getYawMax())
                mCore.startChangeRcByValue(getRoll(),getPitch(),getThr(),val);
            break;
            //mCore.startChangeRcBySteup(0,0,0,1);
            break;
//thr add
        case Qt::Key_Up:
        case Qt::Key_W:
            val = getThr() + rcTrim;
            if( val >= getThrMin() && val <= getThrMax())
                mCore.startChangeRcByValue(getRoll(),getPitch(),val,getYaw());
            //mCore.startChangeRcBySteup(0,0,1,0);
            break;
//thr sub
        case Qt::Key_S:
        case Qt::Key_Down:
            val = getThr() - rcTrim;
            if( val >= getThrMin() && val <= getThrMax())
                mCore.startChangeRcByValue(getRoll(),getPitch(),val,getYaw());
            //mCore.startChangeRcBySteup(0,0,-1,0);
            break;

//other key
        case Qt::Key_5:
            //arm and disarm
            thr = getThr();
            yaw = getYaw();
            if( isArmed() )
                mCore.startChangeRcByValue(getRoll(),getPitch(),getThrMin(),getYawMin());
            else
                mCore.startChangeRcByValue(getRoll(),getPitch(),getThrMin(),getYawMax());
            break;
        default:
            break;
        }
        mLastKey = key;
    }
    void qgcReleaseKey(int key)
    {
        switch(key){
        //roll
        case Qt::Key_1:
        case Qt::Key_3:
            mCore.startChangeRcByValue(roll,getPitch(),getThr(),getYaw());
            //mCore.startChangeRcBySteup(-1,0,0,0);
            break;
        //pitch
        case Qt::Key_2:
        case Qt::Key_4:
            mCore.startChangeRcByValue(getRoll(),pitch,getThr(),getYaw());
            break;
//yaw
        case Qt::Key_A:
        case Qt::Key_D:
            mCore.startChangeRcByValue(getRoll(),getPitch(),getThr(),yaw);
            break;


        case Qt::Key_5:
            mCore.startChangeRcByValue(getRoll(),getPitch(),thr,yaw);
            break;

        }
        mLastKey = 0;
    }

    int getYaw(){
        if( mCore.mCopterRc.enable ){
            return mCore.mCopterRc.rc[3];
        }else
            return 0;
    }
    int getYawMin() { return mCore.mCopterRc.enable ? mCore.mCopterRc.yawGroup[0]:0 ;}
    int getYawMax() { return mCore.mCopterRc.enable ? mCore.mCopterRc.yawGroup[2]:0 ;}

    int getPitch()
    {
        if( mCore.mCopterRc.enable ){
            return mCore.mCopterRc.rc[1];
        }else
            return 0;
    }
    int getPitchMin() { return  mCore.mCopterRc.enable ? mCore.mCopterRc.pitchGroup[0]:0 ; }
    int getPitchMax() { return mCore.mCopterRc.enable ? mCore.mCopterRc.pitchGroup[2]:0 ; }

    int getThr()
    {
        if( mCore.mCopterRc.enable ){
            return mCore.mCopterRc.rc[2];
        }else
            return 0;
    }
    int getThrMin() { return mCore.mCopterRc.enable ? mCore.mCopterRc.thrGroup[0]:0 ; }
    int getThrMax() { return mCore.mCopterRc.enable ? mCore.mCopterRc.thrGroup[2]:0 ; }

    int getRoll()
    {
        if( mCore.mCopterRc.enable ){
            return mCore.mCopterRc.rc[0];
        }else
            return 0;
    }
    int getRollMin() { return mCore.mCopterRc.enable ? mCore.mCopterRc.rollGroup[0]:0 ; }
    int getRollMax() { return mCore.mCopterRc.enable ? mCore.mCopterRc.rollGroup[2]:0 ; }

    int getRcSteup()
    {
        return rcSteup;
    }
    int getRcTrim()
    {
        return rcTrim;
    }
    bool updateRcParam(int sp,int tp)
    {
        debug("rc param "+QString::number(sp)+"and "+QString::number(tp));
        if(sp <=800 && sp >0) rcSteup = sp;
        else return false;
        if( tp <= 100 && tp >0 ) rcTrim = tp;
        else return false;

        return true;
    }

    void setBaud(int baud)
    {
        mserialBaud = baud;
    }

    void setRcValue ( int rc, int val){
        if( rc >= 1 && rc<= 8)
            if( val >= 1000 && val <= 2500)
                mCore.setRcValue(rc,val);
    }
    int getRcValue ( int rc ){
        if( rc >= 1 && rc<= 8)
            return mCore.getRcValue(rc);
        else
            return 0;
    }
    int getRcParamMaxValue (int rc ){
        if( mCore.mCopterParam.enable && rc >= 1 && rc<= 8){
            QString name = "RC"+QString::number(rc)+"_MAX";
            debug(name+"="+QString::number(mCore.mCopterParam.params[name]));
            return mCore.mCopterParam.params[name];
        }else
            return 0;
    }
    int getRcParamMinValue (int rc ){
        if( mCore.mCopterParam.enable && rc >= 1 && rc<= 8){
            QString name = "RC"+QString::number(rc)+"_MIN";
            debug(name+"="+QString::number(mCore.mCopterParam.params[name]));
            return mCore.mCopterParam.params[name];
        }else
            return 0;
    }
    void setRcMask(int rc, int en)
    {
        if( rc <=8 && rc >= 1){
            if( en )
                rcChangeMask |= 1<<(rc-1);
            else
                rcChangeMask &= ~(1<<(rc-1));

        }
        mCore.setRcChannelMask(rcChangeMask);
    }
    void clearRcMask()
    {
        rcChangeMask = 0x0;
    }

private:
    QString mdebugmsg;
    QString mseriolport;
    int mserialBaud;
    int mstatus;
    QgcCore mCore;



};

#endif // QGCUI_H
