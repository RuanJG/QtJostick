#include "serialthread.h"

serialThread::serialThread(QObject *parent) : QThread(parent), waitTimeout(1000)
{
    spStatus=SPCLOSE;
}
