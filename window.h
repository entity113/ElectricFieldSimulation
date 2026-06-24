#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMenu>
#include <QAction>

#include "simulation.h"
#include "coordinatesdialog.h"

const QString ACCEPT = "Accept";

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();
    
    void updateTable();
    
    Simulation simulation;

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::Window *ui;
};

#endif // WINDOW_H
