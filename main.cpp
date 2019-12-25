#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QLibraryInfo>
#include <QDir>
#include <QStandardPaths>
#include <QMessageLogger>
#include <QProcess>
#include <QMessageBox>

#include "mainwindow.h"
#include "config_qnetstatview.h"
#include "DialogPasswordPrompt.h"

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

void printUsage(){
    qInfo("%s",qPrintable(QObject::tr("Usage: qnetstatview [options]")));
    qInfo("%s",qPrintable(QCoreApplication::applicationName() +" - "+QObject::tr("Shows detailed listings of all TCP and UDP endpoints")));
    qInfo() << "";
    qInfo() << "Options:";
    qInfo("%s",qPrintable("-h, --help                 "+QObject::tr("Displays this help.")));
    qInfo("%s",qPrintable("-v, --version              "+QObject::tr("Displays version information.")));
    qInfo("%s",qPrintable("--run-as-root              "+QObject::tr("Run as root.")));
}

void runAsRoot(){
    QString appForEnterRoot = "";

    QStringList possibleApps {"pkexec","kdesu5","kdesu","gksu"};

    for (QString app:possibleApps){
        if (QStandardPaths::findExecutable(app)!=""){
            appForEnterRoot=app;
            break;
        }
    }

    if (appForEnterRoot=="pkexec"){
        QFile poolkitPolicy("/usr/share/polkit-1/actions/org.pkexec.qnetstatview.policy");
        if (poolkitPolicy.exists()){
            system(QString("pkexec  "+QApplication::applicationDirPath()+"/"+qAppName()+" ").toStdString().data());
        }else{
            system(QString("pkexec env DISPLAY=$DISPLAY XAUTHORITY=$XAUTHORITY "+QApplication::applicationDirPath()+"/"+qAppName()+" ").toStdString().data());
        }
        exit(0);
    }


    if (appForEnterRoot=="kdesu5" or appForEnterRoot=="kdesu"){
        QProcess::startDetached(appForEnterRoot+" "+QApplication::applicationDirPath()+"/"+qAppName()+" 2> /dev/null &");
        exit(0);
    }

    if (appForEnterRoot=="gksu"){
        QProcess::startDetached("gksu -u root "+QApplication::applicationDirPath()+"/"+qAppName()+" 2> /dev/null &");
        exit(0);
    }


    DialogPasswordPrompt ps;
    if (ps.exec()){
        QString password = ps.getPassword();

        int code = system(QString("echo \""+password+"\" | sudo -S -b "+QApplication::applicationDirPath()+"/"+qAppName()+" &> /dev/null").toStdString().c_str());
        if (code!=0){
            QMessageBox::critical(nullptr,QObject::tr("Restart as root"),QObject::tr("Application startup failed. The password may have been typed incorrectly or you is not in the sudoers file. Restart as root canceled"));
        }
        exit(0);
    }
}

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


    for (QString arg:QCoreApplication::arguments()){
        if (arg=="--help" or arg=="-h"){
            printUsage();
            return 0;
        }

        if (arg=="--version" or arg=="-v"){
            qInfo("QNetStatView: %s",QNETSTATVIEW_VERSION);
            return 0;
        }

        if (arg.toLower()=="--run-as-root"){
            runAsRoot();
            return 0;
        }
    }



    MainWindow window;
    window.resize(800,600);
    window.show();

    return app.exec();
}

