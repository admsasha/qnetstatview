#ifndef DIALOGPASSWORDPROMPT_H
#define DIALOGPASSWORDPROMPT_H

#include <QDialog>

namespace Ui {
class DialogPasswordPrompt;
}

class DialogPasswordPrompt : public QDialog {
    Q_OBJECT

    public:
        explicit DialogPasswordPrompt(QWidget *parent = 0);
        ~DialogPasswordPrompt();
        QString getPassword();

    private:
        Ui::DialogPasswordPrompt *ui;
};

#endif // DIALOGPASSWORDPROMPT_H
