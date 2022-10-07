#include "dialog.h"
#include "ui_dialog.h"
#include <QColor>
//
#include <QDateTime>
#include <QDoubleValidator>

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->line->setObjectName(QString::fromUtf8("line"));
    ui->line->setGeometry(QRect(320, 150, 118, 3));
    ui->line->setFrameShape(QFrame::HLine);
    ui->line->setFrameShadow(QFrame::Sunken);
    ui->txtDCV->setValidator(new QDoubleValidator(0, 10000, 3, this));
    ui->txtDCA->setValidator(new QDoubleValidator(0, 1000, 3, this));
    ui->txtDiodeDrop->setValidator(new QDoubleValidator(0, 25, 3, this));
    ui->txtDummyR->setValidator(new QDoubleValidator(0, 1000, 3, this));
    ui->txtPKRFV->setValidator(new QDoubleValidator(0, 10000, 3, this));
    appendHistory("App started.");
}

Dialog::~Dialog()
{
    delete ui;
}
static inline bool isNumeric(const QString &s, bool check_positive = true)
{
    bool ok = false;
    const auto d = s.toDouble(&ok);
    if (ok && check_positive)
        ok = d >= 0;
    return ok;
}

static void reselect(QLineEdit *ptr)
{
    ptr->setFocus();
    ptr->setSelection(0, ptr->text().length());
}

bool Dialog::CheckInputs()
{
    const auto allinputs = Dialog::findChildren<QLineEdit *>();
    for (auto le : allinputs) {
        if (le == ui->txtNotes)
            continue;
        if (!isNumeric(le->text())) {
            const auto s = displayInputError(le);
            appendHistory(s, true);
            reselect(le);
            return false;
            break;
        }
    }

    return true;
}

void Dialog::appendHistory(const QString &s, bool is_error)
{
    if (is_error) {
        QColor c;
        ui->txtHistory->setTextColor(c.red());
    } else {
        auto def_color = ui->txtHistory->palette().color(QWidget::foregroundRole());
        ui->txtHistory->setTextColor(def_color);
    }

    const auto notes = ui->txtNotes->text();
    QString mys(s);
    if (!notes.isEmpty()) {
        mys = mys + ("\r\nNOTES:\t\t" + notes);
    }

    QString now = QDateTime::currentDateTime().toString("dd/MM/yy hh::mm:ss") + '\t';
    ui->txtHistory->append(now + mys);
    ui->txtHistory->append("\r\n");
}

QString Dialog::displayInputError(QLineEdit *le)
{
    if (le == ui->txtDCA) {
        return "BAD VALUE FOR SUPPLY CURRENT AMPS";
    } else if (le == ui->txtDCV) {
        return "BAD VALUE FOR SUPPLY VOLTS DC";
    } else if (le == ui->txtDiodeDrop) {
        return "BAD VALUE FOR DIODE DROP VOLTAGE";
    } else if (le == ui->txtDummyR) {
        return "BAD VALUE FOR DUMMY LOAD RESISTANCE";
    } else if (le == ui->txtPKRFV) {
        return "BAD VALUE FOR PEAK RF MEASURED VOLTAGE";
    } else {
        return "UNKNOWN BAD VALUE. CHECK ALL INPUT VALUES.";
    }
}

QString Dialog::calc()
{
    const auto Vin = ui->txtDCV->text().toDouble();
    const auto Iin = ui->txtDCA->text().toDouble();
    const auto Pin = Vin * Iin;
    const auto sPin = QString::number(Pin, 'd', 3);

    const auto vpk = ui->txtPKRFV->text().toDouble() + ui->txtDiodeDrop->text().toDouble();
    const auto vrms = vpk * 0.707;
    const auto rdummy = ui->txtDummyR->text().toDouble();
    const auto vrms_squared = vrms * vrms;
    const auto output_power = vrms_squared / rdummy;
    const auto output_val = QString::number(output_power, 'd', 3);

    QString ipret("Input power = Vdc*Idc = " + sPin + "Watts" + "\r\n\t\t");
    const auto eff = output_power / Pin;
    QString seff = "\r\n\t\tEfficiency = Input Power/Output Power ="
                   + QString::number(eff * 100.0, 'd', 3) + "%.";

    QString ret(ipret + "Power=V(rms) squared / RDummy, so output power=" + output_val + " Watts."
                + seff);

    const auto rdc = vrms / Iin;
    const QString srdc = QString::number(rdc, 'd', 2) + "R";
    ui->lblRDC->setText(srdc);

    ui->lblOutputPower->setText(output_val + "W");
    ui->lblInputPower->setText(sPin + "W");

    ui->lblEff->setText(QString::number(eff * 100.0, 'd', 3) + "%");

    return ret;
}

void Dialog::on_btnCalc_clicked()
{
    ui->lblInputPower->setText("");
    ui->lblOutputPower->setText("");
    ui->lblEff->setText("");

    if (!CheckInputs()) {
    } else {
        appendHistory(calc());
    }
}
