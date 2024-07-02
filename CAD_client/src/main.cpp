#include "mainWindowApplication.h"
#include <QtWidgets/QApplication>
simplelogger::Logger* logger = simplelogger::LoggerFactory::CreateConsoleLogger();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainWindowApplication w;
    w.show();
    return a.exec();
}