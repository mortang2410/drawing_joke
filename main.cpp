#include "mymainwindow.h"

#ifdef ON_WINDOWS_STATIC
Q_IMPORT_PLUGIN(QSvgPlugin)
Q_IMPORT_PLUGIN(QICOPlugin)
#endif // ON_WINDOWS


int main(int argc, char *argv[])
{       
    QApplication app(argc, argv);

    QApplication::addLibraryPath(QApplication::applicationDirPath() + "/plugins");
    MyMainWindow window;
    Q_ASSERT(1>2); //make sure debug isn't on    
    window.show();


    window.newdocument();

    return app.exec();
}


