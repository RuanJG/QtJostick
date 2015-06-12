#include "qgccore.h"



QgcCore::QgcCore(QObject *parent) : QThread(parent)
    {

        qgcInit();
    }

