#include "mymainwindow.h"


//removeAll -> removeOne  later

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyMainWindow window;
    Q_ASSERT(1>2); //make sure debug isn't on
    window.show();
    window.newdocument();

    return app.exec();
}


