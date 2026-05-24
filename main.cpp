#include "EftDemo.h"
#include "stdafx.h"
#include <QtWidgets/QApplication>
#include "testtwocontroller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EftDemo w;
    w.show();
    // TestTwoController ttc;
    // ttc.main();

    return a.exec();
}
