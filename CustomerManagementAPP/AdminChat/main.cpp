#include "adminchat.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AdminChat w;
    w.show();
//    AdminChat w1;
//    w1.show();
    return a.exec();
}
