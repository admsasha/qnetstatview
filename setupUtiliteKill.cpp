#include "setupUtiliteKill.h"
#include "ui_setupUtiliteKill.h"
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>

setupUtiliteKill::setupUtiliteKill(QWidget *parent) : QDialog(parent),  ui(new Ui::setupUtiliteKill){
    ui->setupUi(this);

    this->setWindowTitle(tr("Utilite for close connection"));
    this->setWindowIcon(QIcon(QString(PATH_USERDATA)+"/images/qnetstatview.png"));

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(hide()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(actionApply()));

    QSettings conf(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)+"/qnetstatview.ini", QSettings::IniFormat);
    conf.setPath(QSettings::IniFormat, QSettings::UserScope, QDir::currentPath());

    ui->lineEdit->setText(conf.value("setup/prockill","").toString());

}

setupUtiliteKill::~setupUtiliteKill(){
    delete ui;
}

void setupUtiliteKill::actionApply(){

    QSettings conf(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)+"/qnetstatview.ini", QSettings::IniFormat);
    conf.setPath(QSettings::IniFormat, QSettings::UserScope, QDir::currentPath());

    conf.setValue("setup/prockill",ui->lineEdit->text());

    hide();
}
