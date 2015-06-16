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

public slots:
    void debug(QString msg);
    QList<QString> getSeriolPortList();
    void qgcArm();
    void qgcDisArm();
    void qgcConnect();
    void qgcDisconnect();
    void qgcFly();
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


private:
    QString mdebugmsg;
    QString mseriolport;
    int mstatus;
    QgcCore mCore;



};

#endif // QGCUI_H
