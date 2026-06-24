#ifndef COORDINATESDIALOG_H
#define COORDINATESDIALOG_H

#include <QDialog>

namespace Ui {
class CoordinatesDialog;
}

class CoordinatesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CoordinatesDialog(double xDef, double yDef, QWidget *parent = nullptr);
    ~CoordinatesDialog();
    
    double x, y;
    bool accepted = false;

private:
    Ui::CoordinatesDialog *ui;
};

#endif // COORDINATESDIALOG_H
