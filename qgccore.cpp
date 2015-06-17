#include "qgccore.h"



QgcCore::QgcCore(QObject *parent) : QThread(parent)
    {

        qgcInit();

        connect(&serial,SIGNAL(readyRead()),
                this,SLOT(hasDataFromSerial()));
    }

