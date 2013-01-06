QT += svg

SOURCES += \
    main.cpp \
    renderarea.cpp \
    myshape.cpp \
    mymainwindow.cpp \
    mycentralwidget.cpp \
    misc.cpp \
    commands.cpp \
    mydialogs.cpp \
    smallfunctions.cpp \
    mydockwidgets.cpp \
    saveLoad.cpp \
    mydockwidgets2.cpp
HEADERS += \
    renderarea.h \
    myshape.h \
    mymainwindow.h \
    mycentralwidget.h \
    qtmodules.h \
    misc.h \
    commands.h \
    mydialogs.h \
    smallfunctions.h \
    mydockwidgets.h \
    saveLoad.h \
    mydockwidgets2.h

FORMS += \
    ModifyDialog.ui \
    mytransformdockwidget.ui \
    exportdialog.ui \
    shapeDock.ui \
    preference.ui

RESOURCES += \
    resource.qrc
QT += widgets
INCLUDEPATH += "D:/qtbuild/testing2"
