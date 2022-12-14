######################################################################
# Automatically generated by qmake (2.01a) ?? ??? 15 19:12:31 2013
######################################################################

TEMPLATE = app
TARGET = qnetstatview
DEPENDPATH += .
INCLUDEPATH += .

MOC_DIR = .build
OBJECTS_DIR = .build
UI_DIR = .build
RCC_DIR = .build

DESTDIR = Bin
QT += network widgets

greaterThan(QT_VERSION, 6){
    QT += core5compat
}

RESOURCES += qnetstatview.qrc

isEmpty(PATH_USERDATA){
    win32: PATH_USERDATA=.
    unix:  PATH_USERDATA=/usr/share/qnetstatview
}
message("Set PATH_USERDATA:" $$PATH_USERDATA)
DEFINES += PATH_USERDATA="\\\""$$PATH_USERDATA"\\\""


TRANSLATIONS = $$files(langs/qnetstatview_*.ts)


win32 {
    RC_FILE = qnetstatview.rc
    LIBS += -lpsapi
}

unix {

}

QMAKE_CXXFLAGS += -pedantic -pedantic-errors
QMAKE_CXXFLAGS += -Wall -Wextra -Wformat -Wformat-security -Wno-unused-variable -Wno-unused-parameter


### install ###

isEmpty(QMAKE_LRELEASE) {
    win32|os2:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    unix {
        !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease-qt5 }
        !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease-qt6 }
    } else {
        !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease }
    }
}

!win32 {
  system($${QMAKE_LRELEASE} -silent $${_PRO_FILE_} 2> /dev/null)
}
win32 {
  system($${QMAKE_LRELEASE} $${_PRO_FILE_})
}

updateqm.input = TRANSLATIONS
updateqm.output = langs/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE -silent ${QMAKE_FILE_IN} -qm langs/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm

data_bin.path = /usr/bin/
data_bin.files = Bin/qnetstatview
INSTALLS += data_bin

data_app.path = /usr/share/applications/
data_app.files = pkg/qnetstatview.desktop
INSTALLS += data_app

data_pixmaps.path = /usr/share/pixmaps/
data_pixmaps.files = images/qnetstatview.png
INSTALLS += data_pixmaps

polkit.path = /usr/share/polkit-1/actions/
polkit.files = pkg/org.pkexec.qnetstatview.policy
INSTALLS += polkit

data_langs.path = $$PATH_USERDATA/langs/
data_langs.files = langs/*.qm
INSTALLS += data_langs

data_images.path = $$PATH_USERDATA/images
data_images.files = images/*
INSTALLS += data_images

data_icons.path = /usr/share/icons/hicolor/
data_icons.files = pkg/icons/*
INSTALLS += data_icons


# Input
SOURCES += main.cpp \
    mainwindow.cpp \
    cNetStat.cpp \
    ProcessListWindows.cpp \
    ProcessListLinux.cpp \
    setupUtiliteKill.cpp \
    ProcessList.cpp \
    FormAbout.cpp \
    DialogPasswordPrompt.cpp \
    ProcessExplorer.cpp

HEADERS += \
    mainwindow.h \
    cNetStat.h \
    ProcessListWindows.h \
    ProcessListLinux.h \
    setupUtiliteKill.h \
    ProcessList.h \
    FormAbout.h \
    config_qnetstatview.h \
    DialogPasswordPrompt.h \
    ProcessExplorer.h

FORMS += \
    mainwindow.ui \
    setupUtiliteKill.ui \
    FormAbout.ui \
    DialogPasswordPrompt.ui \
    ProcessExplorer.ui
