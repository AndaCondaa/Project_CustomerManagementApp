#include "client.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client *w = new Client;
    w->show();
    Client *w2 = new Client;
    w2->show();
    Client *w3 = new Client;
    w3->show();
    return a.exec();
}
