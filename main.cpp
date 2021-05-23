#include "blockmatcher.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BlockMatcher w;
    w.show();
    return a.exec();
}
