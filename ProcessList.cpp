#include "ProcessList.h"

#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
#include <signal.h>
#include <sys/wait.h>
#endif

#if defined(Q_OS_WIN)
#include <windows.h>
#include <psapi.h>
#endif

#include <QDebug>

ProcessList::ProcessList(){

}

void ProcessList::killProcess(QString pid){

    if (pid=="" or pid.toInt()==0){
        QMessageBox::critical(nullptr,"KillProcess",QObject::tr("You cannot kill this process"));
        return;
    }

#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    if (kill(pid.toInt(),SIGTERM)){
        QMessageBox::critical(nullptr,"KillProcess",QObject::tr("Killing of process was not successful"));
    }
#elif defined(Q_OS_WIN)
    HANDLE tmpHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid.toInt());
    if (NULL != tmpHandle){
        TerminateProcess(tmpHandle, 0);
    }
#else
    QMessageBox::critical(nullptr,"KillProcess",QObject::tr("Your OS does not support killProcess"));
#endif

}


void ProcessList::closeConnection(QString from, QString to){
    QSettings conf(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)+"/qnetstatview.ini", QSettings::IniFormat);
    conf.setPath(QSettings::IniFormat, QSettings::UserScope, QDir::currentPath());

    QString utiliteProcKill=conf.value("setup/prockill","").toString();

    if (utiliteProcKill==""){
        QMessageBox::critical(nullptr,"closeConnection",QObject::tr("set utilities for kill connections"));
    }else{
        QStringList fromList = from.split(":");
        QStringList toList = to.split(":");

        utiliteProcKill.replace("%LOCAL_IP%",fromList.at(0));
        utiliteProcKill.replace("%LOCAL_PORT%",fromList.at(1));
        utiliteProcKill.replace("%REMOTE_IP%",toList.at(0));
        utiliteProcKill.replace("%REMOTE_PORT%",toList.at(1));

        system(utiliteProcKill.toStdString().c_str());
    }

}
