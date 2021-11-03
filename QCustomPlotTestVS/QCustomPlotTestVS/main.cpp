#include "QCustomPlotTestVS.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCustomPlotTestVS w;
    w.show();
    return a.exec();
}
