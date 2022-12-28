
#include "lpf.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    lpf w;
    w.show();
    return a.exec();
}
