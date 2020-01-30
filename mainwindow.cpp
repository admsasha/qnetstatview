#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QScrollBar>
#include <QClipboard>
#include <QHeaderView>
#include <QFileDialog>
#include <QVector>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QProcess>
#include <QStyledItemDelegate>

#include <unistd.h>
#include <sys/types.h>

#include "FormAbout.h"
#include "ProcessExplorer.h"
#include "DialogPasswordPrompt.h"
#include "config_qnetstatview.h"

Q_DECLARE_METATYPE(QVector<sNetStat>)

class elideLeftItemC : public QStyledItemDelegate
{
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem opt = option;
        opt.textElideMode=Qt::ElideLeft;
        QStyledItemDelegate::paint(painter, opt, index);
    }
} elideLeftItem;

MainWindow::MainWindow(QWidget *parent) :    QMainWindow(parent),    ui(new Ui::MainWindow){
    ui->setupUi(this);

    qRegisterMetaType < QVector<sNetStat> > ("QVector<sNetStat>");

    // date start 19.02.2013
    this->setWindowTitle(QString("QNetStatView  %1 (%2)").arg(QNETSTATVIEW_VERSION).arg(QNETSTATVIEW_DATEBUILD));
    this->setWindowIcon(QIcon(QString(PATH_USERDATA)+"/images/qnetstatview.png"));

    conf =  new QSettings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)+"/qnetstatview.ini", QSettings::IniFormat);
    conf->setPath(QSettings::IniFormat, QSettings::UserScope, QDir::currentPath());


    // Создание ContextMenu
    initPopupMenu();

    connect(ui->tableWidget->horizontalHeader(),&QHeaderView::sectionClicked,this,&MainWindow::tableWidget_sectionClicked);
    connect(ui->tableWidget, &QWidget::customContextMenuRequested, this, &MainWindow::popupCustomMenu );

    connect(ui->actionCopy,&QAction::triggered,this,&MainWindow::CopyToClipboard);
    connect(ui->actionSave_As,&QAction::triggered,this,&MainWindow::CopyToFile);
    connect(ui->actionAbout,&QAction::triggered,this,&MainWindow::showAbout);

    connect(ui->actionRefresh,&QAction::triggered,this,&MainWindow::timerUpdate_timeout);
    connect(ui->actionRestart_as_Root,&QAction::triggered,this,&MainWindow::restartAsRoot);
    connect(ui->actionExit,&QAction::triggered,this,&QWidget::close);

    connect(ui->actionTCP,&QAction::triggered,this,&MainWindow::timerUpdate_timeout);
    connect(ui->actionTCP6,&QAction::triggered,this,&MainWindow::timerUpdate_timeout);
    connect(ui->actionUDP,&QAction::triggered,this,&MainWindow::timerUpdate_timeout);
    connect(ui->actionUDP6,&QAction::triggered,this,&MainWindow::timerUpdate_timeout);

    connect(ui->actionESTABLISHED,&QAction::triggered,this,&MainWindow::timerUpdate_timeout);
    connect(ui->actionLISTEN,&QAction::triggered,this,&MainWindow::timerUpdate_timeout);
    connect(ui->actionCLOSE,&QAction::triggered,this,&MainWindow::timerUpdate_timeout);
    connect(ui->actionAllOther,&QAction::triggered,this,&MainWindow::timerUpdate_timeout);

    connect(ui->actionUtilite_for_kill,&QAction::triggered,this,&MainWindow::showSetupKill);
    connect(ui->actionResolve_Addresses,&QAction::triggered,this,&MainWindow::Resolve_Addresses);

    connect(ui->action1_second,&QAction::triggered,this,&MainWindow::timer_speed1);
    connect(ui->action3_second,&QAction::triggered,this,&MainWindow::timer_speed3);
    connect(ui->action5_second,&QAction::triggered,this,&MainWindow::timer_speed5);
    connect(ui->actionPause,&QAction::triggered,this,&MainWindow::timer_pause);


    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);

    // init tableWidget
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->setColumnCount(8);
    ui->tableWidget->sortByColumn(1);
    ui->tableWidget->setColumnHidden(0,true);
    ui->tableWidget->setItemDelegateForColumn(6, &elideLeftItem);

    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "" << tr("prot") << tr("local address") << tr("rem address") << tr("state") << tr("pid") << tr("program") << tr("cmdline"));

    ui->tableWidget->horizontalScrollBar()->setMaximum(ui->tableWidget->columnCount());

    ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setContextMenuPolicy( Qt::CustomContextMenu );
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // default value
    ui->tableWidget->setColumnWidth(1,41);
    ui->tableWidget->setColumnWidth(2,160);
    ui->tableWidget->setColumnWidth(3,160);
    ui->tableWidget->setColumnWidth(5,45);
    ui->tableWidget->setColumnWidth(7,345);


#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    if (getuid()==0){
        ui->actionRestart_as_Root->setVisible(false);
    }
#elif defined(Q_OS_WIN)
    ui->actionRestart_as_Root->setVisible(false);
#endif


    netstat = new cNetStat();
    connect(netstat,SIGNAL(endReadStat(QVector<sNetStat>)),this,SLOT(drawTable(QVector<sNetStat>)));

    sortcol=2;

    // Загрузка services
    QFile file(":services");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        while (!in.atEnd()) {
            QStringList service = in.readLine().split(QRegExp("\\s+"));
            if (service.size()<2) continue;
            ServicesCache[service.at(1).toUpper()]=service.at(0);
        }
        file.close();
    }

    timerUpdate_timeout();
    timerUpdate = new QTimer(this);
    connect (timerUpdate,SIGNAL(timeout()),this,SLOT(timerUpdate_timeout()));
    timerUpdate->start(3000);


    // restore from config
    this->resize(conf->value("form/width",this->width()).toInt(),conf->value("form/height",this->height()).toInt());

    for (int tableCol=1;tableCol<ui->tableWidget->columnCount();tableCol++){
        ui->tableWidget->setColumnWidth(tableCol,conf->value("tableWidget/column"+QString::number(tableCol),ui->tableWidget->columnWidth(tableCol)).toInt());
    }

    ui->actionResolve_Addresses->setChecked(conf->value("view/resolve","false").toBool());
    ui->actionTCP->setChecked(conf->value("view/TCP","true").toBool());
    ui->actionUDP->setChecked(conf->value("view/UDP","true").toBool());
    ui->actionTCP6->setChecked(conf->value("view/TCP6","true").toBool());
    ui->actionUDP6->setChecked(conf->value("view/UDP6","true").toBool());

    ui->actionLISTEN->setChecked(conf->value("view/status_listen","true").toBool());
    ui->actionESTABLISHED->setChecked(conf->value("view/status_established","true").toBool());
    ui->actionCLOSE->setChecked(conf->value("view/status_close","true").toBool());
    ui->actionAllOther->setChecked(conf->value("view/status_other","true").toBool());


    int speed = conf->value("setup/speed",3).toInt();
    switch (speed) {
        case 0:
            timer_pause();
            break;
        case 1:
            timer_speed1();
            break;
        case 5:
            timer_speed5();
            break;
        default:
            timer_speed3();
            break;
    }

}

MainWindow::~MainWindow(){
    timerUpdate->stop();

    conf->setValue("form/width",this->width());
    conf->setValue("form/height",this->height());

    for (int tableCol=1;tableCol<ui->tableWidget->columnCount();tableCol++){
        conf->setValue("tableWidget/column"+QString::number(tableCol),ui->tableWidget->columnWidth(tableCol));
    }

    conf->setValue("view/resolve",ui->actionResolve_Addresses->isChecked());

    conf->setValue("view/TCP",ui->actionTCP->isChecked());
    conf->setValue("view/UDP",ui->actionUDP->isChecked());
    conf->setValue("view/TCP6",ui->actionTCP6->isChecked());
    conf->setValue("view/UDP6",ui->actionUDP6->isChecked());

    conf->setValue("view/status_listen",ui->actionLISTEN->isChecked());
    conf->setValue("view/status_established",ui->actionESTABLISHED->isChecked());
    conf->setValue("view/status_close",ui->actionCLOSE->isChecked());
    conf->setValue("view/status_other",ui->actionAllOther->isChecked());

    int speed=3;
    if (ui->actionPause->isChecked()) speed=0;
    if (ui->action1_second->isChecked()) speed=1;
    if (ui->action5_second->isChecked()) speed=5;
    conf->setValue("setup/speed",speed);

    delete timerUpdate;
    delete conf;
    delete ui;
}


void MainWindow::initPopupMenu(){
    menu = new QMenu("my", this);

    actionPropertiesProcess = new QAction(tr("Properties..."),menu);
    connect(actionPropertiesProcess,SIGNAL(triggered()),this,SLOT(PropertiesProcess()));

    actionKillProcess = new QAction(tr("Kill process"),menu);
    connect(actionKillProcess,SIGNAL(triggered()),this,SLOT(killProcess()));

    actionCloseConnection = new QAction(tr("Close connection"),menu);
    connect(actionCloseConnection,SIGNAL(triggered()),this,SLOT(closeConnection()));


#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    menu->addAction(actionPropertiesProcess);
#endif
    menu->addAction(actionKillProcess);
    menu->addAction(actionCloseConnection);


}
void MainWindow::popupCustomMenu( const QPoint &pos ){
    if (ui->tableWidget->currentRow()==-1) return;

    if (ui->tableWidget->item(ui->tableWidget->currentRow(),5)->text().isEmpty() or ui->tableWidget->item(ui->tableWidget->currentRow(),5)->text()=="0"){
        actionPropertiesProcess->setEnabled(false);
        actionKillProcess->setEnabled(false);
    }else{
        actionPropertiesProcess->setEnabled(true);
        actionKillProcess->setEnabled(true);
    }
    menu->popup(ui->tableWidget->mapToGlobal(pos));
}

void MainWindow::killProcess(){
    ProcessList::killProcess(ui->tableWidget->item(ui->tableWidget->currentRow(),5)->text());
}

void MainWindow::PropertiesProcess(){
    int pid = ui->tableWidget->item(ui->tableWidget->currentRow(),5)->text().toInt();

    ProcessExplorer form(pid,this);
    form.exec();
}

void MainWindow::closeConnection(){
#if defined(Q_OS_WIN)
    QMessageBox::critical(this,tr("close connection"),tr("Your OS does not support close connection"));
    return;
#else
    if (getuid()==0){
        if (ui->tableWidget->item(ui->tableWidget->currentRow(),4)->text() == "ESTABLISHED"){
            ProcessList::closeConnection(ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text().split("/").at(0),ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text().split("/").at(1));
        }else{
            QMessageBox::critical(this,tr("close connection"),tr("Only ESTABLISHED"));
        }
    }else{
        QMessageBox::critical(this,tr("close connection"),tr("Only root can do this"));
    }
#endif

}

void MainWindow::tableWidget_sectionClicked(int col){
    sortcol=col;
    timerUpdate_timeout();
}

void MainWindow::showAbout(){
    FormAbout form(this);
    form.exec();
}

void MainWindow::CopyToClipboard(){
    if (ui->tableWidget->currentRow()==-1){
        QMessageBox::critical(this,tr("Copy to clipboard"),tr("First select the line for copying"));
        return;
    }


    QString buffer="";
    QClipboard *clipboard = QApplication::clipboard();

    for (int i=0;i<ui->tableWidget->columnCount();i++){
        buffer+=ui->tableWidget->item(ui->tableWidget->currentRow(),i)->text() +";";
    }

    clipboard->setText(buffer);

    QMessageBox::information(this,tr("Copy to clipboard"),tr("Line was successfully copied to the clipboard"));
}

void MainWindow::CopyToFile(){
    int interval = timerUpdate->interval();
    timerUpdate->stop();
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save to file"), "", tr("All Files (*.*)"));
    if (fileName.size() > 0){
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&file);

            for (int r=0;r<ui->tableWidget->rowCount();r++){
                for (int i=0;i<ui->tableWidget->columnCount();i++){
                    out << ui->tableWidget->item(r,i)->text() +"\t";
                }
                out << "\n";
            }

            file.close();
        }
    }

    timerUpdate->start(interval);
}

void MainWindow::Resolve_Addresses(){
    timerUpdate_timeout();
}

void MainWindow::lookedUp(const QHostInfo &host){
    if (host.error() != QHostInfo::NoError) {
        //qDebug() << "Lookup failed:" << host.errorString();
        return;
    }

    foreach (const QHostAddress &address, host.addresses())
        DNSCache[address.toString()]=host.hostName();
}

QString MainWindow::resolve(QString ipaddr,QString prot){
    QString result=ipaddr;

    if (ipaddr.split(":").size()<1) return ipaddr;
    if (ipaddr=="*") return ipaddr;

    if (DNSCache[ipaddr.split(":").at(0)] != ""){
        ipaddr=DNSCache[ipaddr.split(":").at(0)]+":"+ipaddr.split(":").at(1);
    }
    if (ServicesCache[ipaddr.split(":").at(1)+"/"+prot] != ""){
        ipaddr=ipaddr.split(":").at(0)+":"+ServicesCache[ipaddr.split(":").at(1)+"/"+prot];
    }


    return ipaddr;
}

void MainWindow::timer_speed1(){
    ui->action1_second->setChecked(true);
    ui->action3_second->setChecked(false);
    ui->action5_second->setChecked(false);
    ui->actionPause->setChecked(false);

    timerUpdate_timeout();
    timerUpdate->start(1000);
}
void MainWindow::timer_speed3(){
    ui->action1_second->setChecked(false);
    ui->action3_second->setChecked(true);
    ui->action5_second->setChecked(false);
    ui->actionPause->setChecked(false);
    timerUpdate_timeout();
    timerUpdate->start(3000);
}
void MainWindow::timer_speed5(){
    ui->action1_second->setChecked(false);
    ui->action3_second->setChecked(false);
    ui->action5_second->setChecked(true);
    ui->actionPause->setChecked(false);
    timerUpdate_timeout();
    timerUpdate->start(5000);
}
void MainWindow::timer_pause(){
    ui->action1_second->setChecked(false);
    ui->action3_second->setChecked(false);
    ui->action5_second->setChecked(false);
    ui->actionPause->setChecked(true);

    timerUpdate->stop();
}


void MainWindow::restartAsRoot(){
    this->hide();
    QProcess::startDetached(QApplication::applicationDirPath()+"/"+qAppName(),QStringList() << "--run-as-root");
    exit(0);
}

void MainWindow::drawTable(QVector<sNetStat> newNetStat){

    QString key="";
    QString key_scroll="";
    int horizontalScrollBarIndex = 0;
    int listen=0;
    int established=0;
    QMap<int,int> oldSizeCols;

    if (ui->tableWidget->currentRow()>=0){
        key = ui->tableWidget->item(ui->tableWidget->currentRow(),2)->text() +"+"+ui->tableWidget->item(ui->tableWidget->currentRow(),3)->text();
        key_scroll = ui->tableWidget->item(ui->tableWidget->verticalScrollBar()->value(),2)->text()+"+"+ui->tableWidget->item(ui->tableWidget->verticalScrollBar()->value(),3)->text();
        horizontalScrollBarIndex = ui->tableWidget->horizontalScrollBar()->value();
    }
    for (int i=0;i<ui->tableWidget->columnCount();i++) oldSizeCols[i]=ui->tableWidget->columnWidth(i);

    ui->tableWidget->setRowCount(0);

    int row=0;
    for (int i=0;i<newNetStat.size();i++){
        // filter prot
        if (newNetStat.at(i).prot=="TCP"){
            if (ui->actionTCP->isChecked()!=true) continue;
        }else if (newNetStat.at(i).prot=="UDP"){
            if (ui->actionUDP->isChecked()!=true) continue;
        }else if (newNetStat.at(i).prot=="TCP6"){
            if (ui->actionTCP6->isChecked()!=true) continue;
        }else if (newNetStat.at(i).prot=="UDP6"){
            if (ui->actionUDP6->isChecked()!=true) continue;
        }

        // filter state
        if (newNetStat.at(i).stat=="ESTABLISHED" or newNetStat.at(i).stat=="ESTABLISH"){
            if (ui->actionESTABLISHED->isChecked()!=true) continue;
        }else if(newNetStat.at(i).stat=="LISTEN" or newNetStat.at(i).stat=="LISTENING"){
            if (ui->actionLISTEN->isChecked()!=true) continue;
        }else if(newNetStat.at(i).stat=="CLOSE"){
            if (ui->actionCLOSE->isChecked()!=true) continue;
        }else{
            if (ui->actionAllOther->isChecked()!=true) continue;
        }

        QString local_address = newNetStat.at(i).local_address;
        QString rem_address = newNetStat.at(i).rem_address;
        QString pid = newNetStat.at(i).process.pid;

        if (!ui->lineEdit->text().isEmpty()){
            if (ui->lineEdit->text()!=rem_address.split(":").at(0) and
                ui->lineEdit->text()!=local_address.split(":").at(0) and
                ui->lineEdit->text()!=pid and
                newNetStat.at(i).process.program.toLower().indexOf(ui->lineEdit->text().toLower())==-1
            ){
                continue;
            }
        }


        ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);

        QTableWidgetItem *itemKey = new QTableWidgetItem(local_address+"/"+rem_address);
        ui->tableWidget->setItem(row, 0, itemKey);

        if (local_address.split(":").at(0) != "*"){
            if (DNSCache[local_address.split(":").at(0)] == ""){
                QHostInfo::lookupHost(local_address.split(":").at(0),this,SLOT(lookedUp(QHostInfo)));
            }
        }

        if (rem_address != "*"){
            if (DNSCache[rem_address.split(":").at(0)] == ""){
                QHostInfo::lookupHost(rem_address.split(":").at(0),this,SLOT(lookedUp(QHostInfo)));
            }
        }

        if (ui->actionResolve_Addresses->isChecked()==true){
            local_address=resolve(local_address,newNetStat.at(i).prot);
            rem_address=resolve(rem_address,newNetStat.at(i).prot);
        }

        QTableWidgetItem *itemProt = new QTableWidgetItem(newNetStat.at(i).prot);
        ui->tableWidget->setItem(row, 1, itemProt);

        QTableWidgetItem *itemLocalAddress = new QTableWidgetItem(local_address);
        ui->tableWidget->setItem(row, 2, itemLocalAddress);

        QTableWidgetItem *itemRemoteAddress = new QTableWidgetItem(rem_address);
        ui->tableWidget->setItem(row, 3, itemRemoteAddress);

        QTableWidgetItem *itemStat = new QTableWidgetItem(newNetStat.at(i).stat);
        ui->tableWidget->setItem(row, 4, itemStat);

        QTableWidgetItem *itemPID = new QTableWidgetItem(newNetStat.at(i).process.pid);
        ui->tableWidget->setItem(row, 5, itemPID);

        QTableWidgetItem *itemProg = new QTableWidgetItem(newNetStat.at(i).process.program);
        ui->tableWidget->setItem(row, 6, itemProg);
        ui->tableWidget->item(row,6)->setToolTip(itemProg->text());

        QTableWidgetItem *itemCmdLine = new QTableWidgetItem(newNetStat.at(i).process.cmdline);
        ui->tableWidget->setItem(row, 7, itemCmdLine);
        ui->tableWidget->item(row,7)->setToolTip(itemCmdLine->text());

        for (int c=1;c<ui->tableWidget->columnCount();c++){
            if (newNetStat.at(i).operation==1){
                ui->tableWidget->item(row,c)->setBackgroundColor(QColor(0,255,0));
            }
            if (newNetStat.at(i).operation==2){
                ui->tableWidget->item(row,c)->setBackgroundColor(QColor(255,0,0));
            }
        }

        if (local_address+"+"+rem_address ==key){
            ui->tableWidget->selectRow(row);
        }

        ui->tableWidget->setRowHeight(row,18);

        if (newNetStat.at(i).stat=="LISTEN" or newNetStat.at(i).stat=="LISTENING") listen++;
        if (newNetStat.at(i).stat=="ESTABLISHED" or newNetStat.at(i).stat=="ESTABLISH") established++;

        row++;
    }


    // Sorting and restoring the cursor position
    ui->tableWidget->sortByColumn(sortcol);

    for (int tableRow=0;tableRow<ui->tableWidget->rowCount();tableRow++){
        if (key_scroll==ui->tableWidget->item(tableRow,2)->text()+"+"+ui->tableWidget->item(tableRow,3)->text()){
            ui->tableWidget->verticalScrollBar()->setValue(tableRow);
        }
    }

    for (int i=0;i<ui->tableWidget->columnCount();i++) ui->tableWidget->setColumnWidth(i,oldSizeCols[i]);

    if (ui->tableWidget->currentRow()>=0){
        ui->tableWidget->horizontalScrollBar()->setMaximum(ui->tableWidget->columnCount());
        ui->tableWidget->horizontalScrollBar()->setValue(horizontalScrollBarIndex);
    }

    ui->label->setText(tr("Count connections: ")+QString::number(row));
    ui->label_2->setText(tr("Count LISTEN: ")+QString::number(listen));
    ui->label_3->setText(tr("Count ESTABLISHED: ")+QString::number(established));

}

void MainWindow::timerUpdate_timeout(){
    netstat->start();
}


void MainWindow::showSetupKill(){
    ui->tableWidget->horizontalScrollBar()->setValue(ui->tableWidget->horizontalScrollBar()->maximum());

    setupUtiliteKill frm;
    frm.exec();
}
