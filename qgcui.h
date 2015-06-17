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
        switch(key)
        {
        case Qt::Key_A:
            //roll sub
            mCore.startChangeRcBySteup(-1,0,0,0);
            break;
        case Qt::Key_D:
            //roll add
            mCore.startChangeRcBySteup(1,0,0,0);
            break;
        case Qt::Key_W:
            //pitch add
            mCore.startChangeRcBySteup(0,1,0,0);
            break;
        case Qt::Key_S:
            //pitch sub
            mCore.startChangeRcBySteup(0,-1,0,0);
            break;
        case Qt::Key_J:
            //yaw sub
            mCore.startChangeRcBySteup(0,0,0,-1);
            break;
        case Qt::Key_L:
            //yaw add
            mCore.startChangeRcBySteup(0,0,0,1);
            break;
        case Qt::Key_I:
            //thr add
            mCore.startChangeRcBySteup(0,0,1,0);
            break;
        case Qt::Key_K:
            //thr sub
            mCore.startChangeRcBySteup(0,0,-1,0);
            break;
        default:
            break;
        }
    }

    int getYaw(){
        if( mCore.mCopterRc.enable ){
            return mCore.mCopterRc.yaw;
        }else
            return 0;
    }
    int getYawMin() { return mCore.mCopterRc.enable ? mCore.mCopterRc.yawGroup[0]:0 ;}
    int getYawMax() { return mCore.mCopterRc.enable ? mCore.mCopterRc.yawGroup[2]:0 ;}

    int getPitch()
    {
        if( mCore.mCopterRc.enable ){
            return mCore.mCopterRc.pitch;
        }else
            return 0;
    }
    int getPitchMin() { return  mCore.mCopterRc.enable ? mCore.mCopterRc.pitchGroup[0]:0 ; }
    int getPitchMax() { return mCore.mCopterRc.enable ? mCore.mCopterRc.pitchGroup[2]:0 ; }

    int getThr()
    {
        if( mCore.mCopterRc.enable ){
            return mCore.mCopterRc.thr;
        }else
            return 0;
    }
    int getThrMin() { return mCore.mCopterRc.enable ? mCore.mCopterRc.thrGroup[0]:0 ; }
    int getThrMax() { return mCore.mCopterRc.enable ? mCore.mCopterRc.thrGroup[2]:0 ; }

    int getRoll()
    {
        if( mCore.mCopterRc.enable ){
            return mCore.mCopterRc.roll;
        }else
            return 0;
    }
    int getRollMin() { return mCore.mCopterRc.enable ? mCore.mCopterRc.rollGroup[0]:0 ; }
    int getRollMax() { return mCore.mCopterRc.enable ? mCore.mCopterRc.rollGroup[2]:0 ; }


private:
    QString mdebugmsg;
    QString mseriolport;
    int mstatus;
    QgcCore mCore;

    int rcSteup[8];



};

#endif // QGCUI_H
