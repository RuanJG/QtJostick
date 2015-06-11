#include "serialthread.h"

serialThread::serialThread(QgcCore *usr,QObject *parent) : QThread(parent), waitTimeout(1000)
{
    coreUser = usr;
    spStatus=SPCLOSE;
}
