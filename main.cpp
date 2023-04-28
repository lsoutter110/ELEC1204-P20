#include "sendwindow.h"
#include "receivewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SendWindow s;
    ReceiveWindow r(&s);
    s.show();
    r.show();

    return a.exec();
}
