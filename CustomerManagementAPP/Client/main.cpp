#include "client.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client *w = new Client(0, "100732", "OsDental");
    w->show();
    Client *w2 = new Client(0, "101640", "Clinic");
    w2->show();
    Client *w3 = new Client(0, "102232", "Osstem");
    w3->show();
    return a.exec();
}
