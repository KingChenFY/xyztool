#include "frmxyzcontrol.h"
#include "ui_frmxyzcontrol.h"

frmXyzControl::frmXyzControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmXyzControl)
{
    ui->setupUi(this);
    this->initForm();
}

frmXyzControl::~frmXyzControl()
{
    delete ui;
}

void frmXyzControl::initForm()
{
}
