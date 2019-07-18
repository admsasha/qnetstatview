#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QLibraryInfo>
#include <QDir>
#include <QStandardPaths>

#include "mainwindow.h"
#include "config_qnetstatview.h"

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
     
    QCoreApplication::setOrganizationName("DanSoft");
    QCoreApplication::setOrganizationDomain("dansoft.ru");
    QCoreApplication::setApplicationVersion(QNETSTATVIEW_VERSION);
    QCoreApplication::setApplicationName("QNetStatView");

    QDir dirConfig(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    if (dirConfig.exists()==false) dirConfig.mkpath(dirConfig.path());

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

    QTranslator translator;
    QString locale = QLocale::system().name();

    translator.load(QString(PATH_USERDATA)+QString("/langs/qnetstatview_") + locale);
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

