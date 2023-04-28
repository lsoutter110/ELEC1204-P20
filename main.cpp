#include "sendwindow.h"
#include "receivewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SendWindow s;
    s.show();

    return a.exec();
}
