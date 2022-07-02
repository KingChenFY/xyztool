#include "frmmain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    frmMain w;
    w.setWindowTitle("XyzTraceShow");
    w.resize(1000, 650);
    w.show();
    return a.exec();
}
