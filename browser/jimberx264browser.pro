TEMPLATE = app
TARGET = browser

#QMAKE_CXXFLAGS += -Werror
QMAKE_CXXFLAGS -= -Wunused-parameter

CONFIG += debug c++11
QT      +=  webenginewidgets \
            websockets \
            network \
            multimedia

HEADERS =   src/jimberx264browser.h \
            src/socketcommunication.h \
            src/jimberx264webview.h \
            src/jimberx264webpage.h \
            src/jxfileupload.h \
            src/jxfiledownload.h \
            src/JCommandHandler.h \
            src/jpopupwindow.h \
            src/proto.h

SOURCES =   src/main.cpp \
            src/jimberx264browser.cpp \
            src/jimberx264webview.cpp \
            src/jimberx264webpage.cpp \
            src/jxfileupload.cpp \
            src/jxfiledownload.cpp \
            src/jpopupwindow.cpp \
            src/socketcommunication.cpp \
            src/jxwindowcontroller.cpp


INCLUDEPATH += $$(CUSTOM_WEB_ENGINE_DIR)
DESTDIR = build
OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.rcc
UI_DIR = $$DESTDIR/.ui
PCH_DIR = $$DESTDIR/.pch