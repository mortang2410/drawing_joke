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
PRECOMPILED_HEADER += qtmodules.h
HEADERS += \
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
INCLUDEPATH += "D:/qtbuild/testing2"


#FOR RELEASE MODE ONLY
DEFINES += QT_NO_DEBUG QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT QT_NO_DEBUG_STREAM
CONFIG += release
