
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QT_COMPILER

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = ../

FORMS += \
    main_widget.ui

LIBS += -L$$DESTDIR/libs \
        -ltest \
        -llibui \
        -lfreetype \
        -lharfbuzz \
        -licu \
        -llibpng \
        -lzlib \
        -llibjpeg \
        -lqrcodegen \


TRANSLATIONS += helloqt_zh_CN.ts

# Default rules for deployment.
qnx: target.path =/tmp/$${TARGET}/bin
else: unix:!android: target.path =/opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SOURCES += \
    ../drivers/display/gui_thread.cpp \
    ../drivers/display/monitor.cpp \
    ../drivers/display/task_thread.cpp \
    ../drivers/indev/mouse_input.cpp \
    ../drivers/indev/mousewheel_input.cpp \
    main.cpp \
    main_widget.cpp

HEADERS += \
    ../drivers/config.h \
    ../drivers/display/gui_thread.h \
    ../drivers/display/task_thread.h \
    ../drivers/display/monitor.h \
    ../drivers/indev/mouse_input.h \
    ../drivers/indev/mousewheel_input.h \
    main_widget.h

INCLUDEPATH += \
    ../../../../frameworks \
    ../../../../../utils/interfaces/innerkits \
    ../../../../../utils/interfaces/kits \
    ../../../../../ui/interfaces/innerkits \
    ../../../../../ui/interfaces/kits \
    ../../../../../../../third_party/freetype/include \
    ../drivers/display \
    ../drivers/indev \
    ../drivers
