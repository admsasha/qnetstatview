#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QLibraryInfo>

#include "mainwindow.h"

#if defined(Q_OS_WIN)
bool SetDebugPrivileges(){
    TOKEN_PRIVILEGES tp;
    LUID luid;
    HANDLE hToken;

    if(!OpenProcessToken(GetCurrentProcess(),TOKEN_READ|TOKEN_ADJUST_PRIVILEGES,&hToken))
        {return false;}

    if(!LookupPrivilegeValueA(NULL,"SeDebugPrivilege",&luid))
        {CloseHandle(hToken);return false;}

    tp.PrivilegeCount=1;
    tp.Privileges[0].Luid=luid;
    tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;

    if(!AdjustTokenPrivileges(hToken,FALSE,&tp,0,NULL,NULL))
        {CloseHandle(hToken);return false;}

    CloseHandle(hToken);
    return true;
}
#endif

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
     
#ifndef HAVE_QT5
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
#endif

    QTranslator translator;
    QString locale = QLocale::system().name();

    if (translator.load(QString("qnetstatview_") + locale)==false){
        translator.load(QString("/usr/share/qnetstatview/qnetstatview_") + locale);
    }
    app.installTranslator(&translator);

    //set translator for default widget's text (for example: QMessageBox's buttons)
    QTranslator qtTranslator;
    qtTranslator.load("qt_"+locale,QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

#if defined(Q_OS_WIN)
    SetDebugPrivileges();
#endif


    MainWindow window;
    window.resize(800,600);
    window.show();

    return app.exec();
}

