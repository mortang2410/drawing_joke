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
#PRECOMPILED_HEADER += qtmodules.h
HEADERS += qtmodules.h \
    renderarea.h \
    myshape.h \
    mymainwindow.h \
    mycentralwidget.h \   
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





#COMMENT OUT THESE LINES IF YOU DON'T WANT A STATIC BUILD.

win32{
TEMPLATE = app
CONFIG+=static
DEFINES += ON_WINDOWS_STATIC
 QTPLUGIN     +=  qico qsvg
}




