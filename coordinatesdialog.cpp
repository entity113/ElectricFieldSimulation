#include "coordinatesdialog.h"
#include "ui_coordinatesdialog.h"

CoordinatesDialog::CoordinatesDialog(double xDef, double yDef, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CoordinatesDialog)
{
    ui->setupUi(this);
    ui->xSpinBox->setValue(xDef);
    ui->ySpinBox->setValue(yDef);
    
    connect(ui->acceptButton, &QPushButton::clicked, [this](){
        x = ui->xSpinBox->value();
        y = ui->ySpinBox->value();
        accepted = true;
        close();
    });
    
    connect(ui->cancelButton, &QPushButton::clicked, [this](){
        close();
    });
}

CoordinatesDialog::~CoordinatesDialog()
{
    delete ui;
}
