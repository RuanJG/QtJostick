#include "qgccore.h"



QgcCore::QgcCore(QObject *parent) : QThread(parent)
    {
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

        debug("qgccore init");

    }

