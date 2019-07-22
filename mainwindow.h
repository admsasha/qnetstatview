#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QHostInfo>
#include <QVector>
#include <QSettings>

#include "cNetStat.h"
#include "setupUtiliteKill.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT
    
    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;

        void initPopupMenu();
        QString resolve(QString ipaddr, QString prot);

        QTimer *timerUpdate;
        cNetStat *netstat;
        QMenu *menu;

        QSettings *conf;

        int sortcol;

        QMap<QString,QString> DNSCache;
        QMap<QString,QString> ServicesCache;


    private slots:
        void popupCustomMenu( const QPoint &pos );
        void killProcess();
        void closeConnection();

        void timerUpdate_timeout();
        void drawTable(QVector<sNetStat> newNetStat);

        void CopyToClipboard();
        void CopyToFile();
        void restartAsRoot();

        void tableWidget_sectionClicked(int col);

        void Resolve_Addresses();
        void lookedUp(const QHostInfo &host);

        void timer_speed1();
        void timer_speed3();
        void timer_speed5();
        void timer_pause();

        void showAbout();
        void showSetupKill();
};

#endif // MAINWINDOW_H
