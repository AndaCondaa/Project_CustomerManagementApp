#include "client.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client *w = new Client(0, "101632", "sadf");
    w->show();
    Client *w2 = new Client(0, "104842", "123");
    w2->show();
//    Client *w3 = new Client(0, "105832", "asdf");
//    w3->show();
    return a.exec();
}
