#include "mymainwindow.h"


//removeAll -> removeOne  later

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyMainWindow window;
    window.show();
    window.newdocument();

    return app.exec();
}


