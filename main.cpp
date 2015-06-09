#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <qgcui.h>



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    qmlRegisterType<QgcUi>("QgcUi",1,0,"QgcUi");


    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
