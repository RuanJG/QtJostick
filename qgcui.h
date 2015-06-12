#ifndef QGCUI_H
#define QGCUI_H

#include <QObject>
#include <qgccore.h>

class QgcUi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString debugmsg READ getQgcDebugMsg WRITE setQgcDebugMsg NOTIFY hasQgcDebugMsg)
    Q_PROPERTY(QString seriolport READ getQgcSeriolport WRITE setQgcSeriolport)// NOTIFY qgcSeriolportChange)
    Q_PROPERTY(QString status READ getQgcStatus NOTIFY qgcStatusChange)

public:
    explicit QgcUi(QObject *parent = 0);
    ~QgcUi();

    //Q_INVOKABLE void print(void);
    //for output debug message
    QString getQgcDebugMsg(){  return mdebugmsg;  }
    void setQgcDebugMsg(QString msg){  mdebugmsg=msg;  }

    QString getQgcSeriolport(){ return mseriolport; }
    void setQgcSeriolport(QString msg) { mseriolport = msg ; debug("set seriolport "+mseriolport); }

    QString getQgcStatus()
    {
        QString status;

        switch(mstatus){
            case Disconnect:
                status="DisConnect";
                break;
            case Connect:
            case DISARM:
            case TAKEOFF:
                status="Connect";
                break;
            case ARMED:
                status="Armed";
            case FLYED:
                status="Fly";
        }

        return status;
    }
    enum QGCSTATUS {
        Disconnect=0,
        Connect,
        ARMED,
        DISARM,
        FLYED,
        TAKEOFF
    };




signals:
    void hasQgcDebugMsg();
    void qgcSeriolportListChange();
    void qgcStatusChange();

public slots:
    void debug(QString msg);
    QList<QString> getSeriolPortList();
    void qgcArm();
    void qgcConnect();
    void qgcDisconnect();
    void qgcFly();

    //slot for qgcCore
    void copterStatusChanged()
    {
        debug ("QGCUI:　get status change");
        debug("custom_mode="+QString::number(mCore.mCopterStatus.customMode)+
              " , autopilot="+QString::number(mCore.mCopterStatus.boardType)+
              ", base_mode="+QString::number(mCore.mCopterStatus.baseMode)+
              ", system_status="+QString::number(mCore.mCopterStatus.systemStatus));

        if( mCore.mCopterStatus.systemStatus == mCore.mCopterStatus.BADVALUE){
            mstatus = Disconnect;
        }else{
            mstatus = Connect;
        }

        emit qgcStatusChange();
    }


private:
    QString mdebugmsg;
    QString mseriolport;
    int mstatus;
    QgcCore mCore;



};

#endif // QGCUI_H
