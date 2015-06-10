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

    QString getQgcStatus(){ return mstatus; }
    enum QGCSTATUS {
        Disconnect,
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


private:
    QString mdebugmsg;
    QString mseriolport;
    QString mstatus;
    QgcCore mCore;



};

#endif // QGCUI_H
