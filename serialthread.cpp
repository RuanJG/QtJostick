#include "serialthread.h"

serialThread::serialThread(QObject *parent) : QThread(parent), waitTimeout(0), quit(false)
{

}
