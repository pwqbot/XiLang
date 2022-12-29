
#include "lpf.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    lpf          w;
    w.show();
    return a.exec();
}
