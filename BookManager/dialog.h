#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
     Dialog(QWidget *parent = nullptr);
    ~Dialog();
    void setMessage(QString mess);
    Ui::Dialog *ui;
signals:
    void Ensure();
};

#endif // DIALOG_H
