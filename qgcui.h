#ifndef QGCUI_H
#define QGCUI_H

#include <QObject>

class QgcUi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString debugmsg READ getQgcDebugMsg WRITE setQgcDebugMsg NOTIFY hasQgcDebugMsg)
public:
    explicit QgcUi(QObject *parent = 0);
    ~QgcUi();
    //Q_INVOKABLE void print(void);
    QString getQgcDebugMsg(){  return mdebugmsg;  }
    void setQgcDebugMsg(QString msg){  mdebugmsg=msg;  }

signals:
    void hasQgcDebugMsg();
public slots:
    void debug(QString msg);


private:
    QString mdebugmsg;

};

#endif // QGCUI_H
