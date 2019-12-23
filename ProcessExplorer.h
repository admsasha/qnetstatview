#ifndef PROCESSEXPLORER_H
#define PROCESSEXPLORER_H

#include <QDialog>

namespace Ui {
    class ProcessExplorer;
}

class ProcessExplorer : public QDialog
{
    Q_OBJECT

    public:
        explicit ProcessExplorer(int pid,QWidget *parent = 0);
        ~ProcessExplorer();

    private:
        Ui::ProcessExplorer *ui;

        int _pid;

        void fillFields(int pid);

        QMap<QString,QString> mapFromFile(QString path);

};

#endif // PROCESSEXPLORER_H
