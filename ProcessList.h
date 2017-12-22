#ifndef PROCESSLIST_H
#define PROCESSLIST_H

#include <QString>
#include <QStringList>
#include <QTime>


struct sProccess {
    QString pid;
    QStringList sockets;
    QString program;
    QString cmdline;
};

struct sNetStat {
    QString prot;
    QString local_address;
    QString rem_address;
    QString stat;
    sProccess process;
    int operation;
    QTime timeEvent;
};



class ProcessList{
    public:
        ProcessList();
        static void killProcess(QString pid);
        static void closeConnection(QString from, QString to);
};

#endif // PROCESSLIST_H
