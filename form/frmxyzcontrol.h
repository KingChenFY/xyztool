#ifndef FRMXYZCONTROL_H
#define FRMXYZCONTROL_H

#include <QWidget>

namespace Ui {
class frmXyzControl;
}

class frmXyzControl : public QWidget
{
    Q_OBJECT

public:
    explicit frmXyzControl(QWidget *parent = nullptr);
    ~frmXyzControl();

private:
    Ui::frmXyzControl *ui;

private slots:
    void initForm();
//    void initConfig();
//    void saveConfig();
};

#endif // FRMXYZCONTROL_H
