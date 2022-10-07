#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_btnCalc_clicked();

private:
    Ui::Dialog *ui;
    bool CheckInputs();
    void appendHistory(const QString &s, bool is_error = false);
    QString displayInputError(QLineEdit *le);
    QString calc();
};
#endif // DIALOG_H
