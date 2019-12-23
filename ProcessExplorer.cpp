#include "ProcessExplorer.h"
#include "ui_ProcessExplorer.h"

#include <QFile>
#include <QDir>

ProcessExplorer::ProcessExplorer(int pid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessExplorer),
    _pid(pid)
{
    ui->setupUi(this);

    fillFields(_pid);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeToContents);

    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(close()));
}

ProcessExplorer::~ProcessExplorer(){
    delete ui;
}




void ProcessExplorer::fillFields(int pid){
    ui->label_7->setText("");
    ui->label_5->setText("");
    ui->label_6->setText("");


    // general
    QString procPath = "/proc/"+QString::number(pid);

    // path exe
    QFile filePathExe(procPath+"/exe");
    ui->label_5->setText(filePathExe.symLinkTarget());

    // command line
    QFile fileCmdLine(procPath+"/cmdline");
    if (fileCmdLine.open(QIODevice::ReadOnly)){
        QByteArray fileContents = fileCmdLine.readAll();
        fileContents.replace(QChar(0x00)," ");
        ui->label_6->setText(fileContents);
        fileCmdLine.close();
    }

    // path exe
    QFile fileCurrentDir(procPath+"/cwd");
    ui->label_7->setText(fileCurrentDir.symLinkTarget());

    QMap<QString, QString> mapFileProcStatus =  mapFromFile("/proc/"+QString::number(pid)+"/status");

    ui->label_8->setText(mapFileProcStatus["Pid"]);
    ui->label_10->setText(mapFileProcStatus["PPid"]);
    ui->label_12->setText(mapFileProcStatus["State"]);

    // Environment
    QFile fileEnviron(procPath+"/environ");
    if (fileEnviron.open(QIODevice::ReadOnly)){
        QByteArray fileContents = fileEnviron.readAll();
        fileContents.replace(QChar(0x00),"\n");
        ui->textEdit->setText(fileContents);
        fileEnviron.close();
    }

    // Files
    QTableWidgetItem *tmpWidgetItem = nullptr;
    QDir dir(procPath+"/fd");
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QString Path = fileInfo.symLinkTarget();

        QMap<QString, QString> mapFile =  mapFromFile("/proc/"+QString::number(pid)+"/fdinfo/"+fileInfo.fileName());
        int pos = mapFile["pos"].toInt();
        QString flags = mapFile["flags"];

        ui->tableWidget->setRowCount(i+1);

        tmpWidgetItem = new QTableWidgetItem(fileInfo.fileName());
        ui->tableWidget->setItem(i, 0, tmpWidgetItem);

        tmpWidgetItem = new QTableWidgetItem(Path);
        tmpWidgetItem->setToolTip(Path);
        ui->tableWidget->setItem(i, 1, tmpWidgetItem);

        tmpWidgetItem = new QTableWidgetItem(QString::number(pos));
        ui->tableWidget->setItem(i, 2, tmpWidgetItem);

        tmpWidgetItem = new QTableWidgetItem(flags);
        ui->tableWidget->setItem(i, 3, tmpWidgetItem);

    }
}


QMap<QString, QString> ProcessExplorer::mapFromFile(QString path){
    QMap<QString, QString> result;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        return result;
    }

    QByteArray contents = file.readAll();
    QStringList lines = QString(contents).split(QRegExp("[\r|\n]"));
    for (QString line:lines){
        QStringList pairs = line.split(":");
        if (pairs.size()==2){
            QString key=pairs.at(0);
            QString value = pairs.at(1);
            value.replace(QRegExp("^\\s+"),"");
            value.replace(QRegExp("\\s+$"),"");

            result[key]=value;
        }
    }

    file.close();


    return result;
}
