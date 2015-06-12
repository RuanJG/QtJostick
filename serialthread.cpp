#include "serialthread.h"

serialThread::serialThread(QObject *parent) : QObject(parent), waitTimeout(1000)
{
    //coreUser = usr;
    spStatus=SPCLOSE;
}
