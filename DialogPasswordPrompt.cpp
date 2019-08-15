#include "DialogPasswordPrompt.h"
#include "ui_DialogPasswordPrompt.h"

#include "config_qnetstatview.h"

DialogPasswordPrompt::DialogPasswordPrompt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPasswordPrompt)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(QString(PATH_USERDATA)+"/images/qnetstatview.png"));
    ui->label->setPixmap(QPixmap(QString(PATH_USERDATA)+"/images/key.png"));

    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(close()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(accept()));
}

DialogPasswordPrompt::~DialogPasswordPrompt(){
    delete ui;
}

QString DialogPasswordPrompt::getPassword(){
    return ui->lineEdit->text();
}
