TARGET = JimberQPA

CONFIG += debug exceptions

QT += \
    websockets \
    network \
    websockets \
    gui-private \
    eventdispatcher_support-private \
    fontdatabase_support-private \
    theme_support-private \

SOURCES += \
    src/main.cpp \
    src/jxintegration.cpp \
    src/jxbackingstore.cpp \
    src/jxwsserver.cpp \
    src/jxwindow.cpp \
    src/jxcursor.cpp \
    src/jxinputcontext.cpp \
    src/videostreaming/JStreamer.cpp \
    src/videostreaming/Encoder.cpp \
    src/jxservices.cpp

HEADERS += \
    src/jxintegration.h \
    src/jxbackingstore.h \
    src/jxwsserver.h \
    src/jxwindow.h \
    src/jxcursor.h \
    src/jxclipboard.h \
    src/jxcommands.h \
    src/jxscreen.h \
    src/jxinputcontext.h \
    src/touchobject.h \
    src/videostreaming/JStreamer.h \
    src/videostreaming/closeeventfilter.h \
    src/videostreaming/Encoder.h
    src/audiostreaming/Encoder.h


DISTFILES += jimberx264.json

LIBS += -lavformat -lavcodec -lavutil -lswscale

PLUGIN_TYPE = platforms
PLUGIN_CLASS_NAME = QJxIntegrationPlugin
!equals(TARGET, $$QT_DEFAULT_QPA_PLUGIN): PLUGIN_EXTENDS = -
load(qt_plugin)


DESTDIR = build
OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.rcc
UI_DIR = $$DESTDIR/.ui
PCH_DIR = $$DESTDIR/.pch
target.path += $$DESTDIR/platforms

QMAKE_CXXFLAGS += -Werror