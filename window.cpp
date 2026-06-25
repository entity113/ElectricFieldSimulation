#include "window.h"
#include "ui_window.h"

Window::Window(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Window)
{
    ui->setupUi(this);
    ui->addButton->installEventFilter(this);
    ui->linesSpinBox->setValue(simulation.fieldLinesNumber);
    layout()->replaceWidget(ui->dummy, &simulation);
    ui->dummy->hide();
    ui->dummy->deleteLater();
    
    QHeaderView *header = ui->chargeTableWidget->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setStretchLastSection(false);
    header->setStyleSheet("QHeaderView { font-weight: normal; }");
    ui->chargeTableWidget->setColumnWidth(1, 90);
    
    connect(ui->chargeTableWidget, &QTableWidget::cellClicked, [this](int row, int col){
        if (ui->removeButton->text() == ACCEPT)
        {
            ui->chargeTableWidget->clearSelection();
            if (col == 0)
            {
                QTableWidgetItem *item = ui->chargeTableWidget->item(row, col);
                if (item) item->setSelected(true);
            }
        }
        else
        {
            if (col != 0) return;
            CoordinatesDialog dialog(simulation.charges.at(row).x, simulation.charges.at(row).y, this);
            dialog.exec();
            if (dialog.accepted)
            {
                simulation.charges[row].x = dialog.x;
                simulation.charges[row].y = dialog.y;
                simulation.update();
                updateTable();
            }
        }
        ui->chargeTableWidget->horizontalHeader()->setHighlightSections(false);
    });
    
    connect(ui->addButton, &QPushButton::clicked, [this](){
        ui->removeButton->setText("Remove");
        simulation.charges.append(Charge());
        simulation.update();
        updateTable();
    });
    
    connect(ui->removeButton, &QPushButton::clicked, [this](){
        if (simulation.charges.isEmpty())
        {
            QMessageBox::warning(this, "Removing charge", "There are no charges");
            return;
        }
        
        if (ui->removeButton->text() != ACCEPT)
        {
            ui->removeButton->setText(ACCEPT);
        }
        else
        {
            int currentRow = -1;
            for (int row = 0; row < ui->chargeTableWidget->rowCount(); row++)
            {
                if (ui->chargeTableWidget->item(row, 0)->isSelected())
                {
                    currentRow = row;
                    break;
                }
            }
            
            if (currentRow == -1)
            {
                QMessageBox::warning(this, "Removing charge", "Nothing selected");
                return;
            }
            
            simulation.charges.removeAt(currentRow);
            simulation.update();
            updateTable();
            ui->removeButton->setText("Remove");
        }
    });
    
    connect(ui->removeButton, &QPushButton::customContextMenuRequested, [this](const QPoint &pos){
        if (ui->removeButton->text() == ACCEPT)
        {
            ui->chargeTableWidget->clearSelection();
            ui->removeButton->setText("Remove");
            return;
        }
        
        QMenu contextMenu(this);
        QAction *removeAll = contextMenu.addAction("Remove all charges");
        connect(removeAll, &QAction::triggered, [this](){
            if (simulation.charges.isEmpty())
            {
                QMessageBox::warning(this, "Removing charge", "There are no charges");
                return;
            }
            if (QMessageBox::question(this, "Remove all charges", "Are you sure?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                simulation.charges.clear();
                simulation.update();
                updateTable();
            }
        });
        
        contextMenu.exec(ui->removeButton->mapToGlobal(pos));
    });
    
    connect(ui->linesSpinBox, &QSpinBox::valueChanged, [this](double value){
        simulation.fieldLinesNumber = value;
        simulation.update();
    });
    
    connect(&simulation, &Simulation::chargeMoved, this, &Window::updateTable);
}

Window::~Window()
{
    delete ui;
}

void Window::updateTable()
{
    ui->chargeTableWidget->setRowCount(0);
    for (int i = 0; i < simulation.charges.count(); i++)
    {
        int rowCount = ui->chargeTableWidget->rowCount();
        ui->chargeTableWidget->insertRow(rowCount);
        ui->chargeTableWidget->setItem(
            rowCount, 0,
            new QTableWidgetItem(
                QString::number(simulation.charges.at(i).x, 'f', 4) +
                " " +
                QString::number(simulation.charges.at(i).y, 'f', 4)
            )
        );
        
        class MyDoubleSpinBox : public QDoubleSpinBox
        {
        protected:
            QString textFromValue(double val) const override
            {
                if (qRound(val * qPow(10, decimals())) == 0)
                    return QDoubleSpinBox::textFromValue(0.0);
                return QDoubleSpinBox::textFromValue(val);
            }
        };
        
        MyDoubleSpinBox *spinBox = new MyDoubleSpinBox();
        spinBox->setRange(-99.999, 99.999);
        spinBox->setDecimals(3);
        spinBox->setSingleStep(0.05);
        spinBox->setValue(simulation.charges.at(rowCount).q);
        spinBox->setContextMenuPolicy(Qt::NoContextMenu);
        connect(spinBox, &MyDoubleSpinBox::valueChanged, [this, rowCount, spinBox](double value){
            simulation.charges[rowCount].q = qRound(value * qPow(10, spinBox->decimals())) / qPow(10, spinBox->decimals());
            simulation.update();
        });
        
        ui->chargeTableWidget->setCellWidget(rowCount, 1, spinBox);
    }
    ui->chargeTableWidget->verticalHeader()->setVisible(false);
}

void Window::closeEvent(QCloseEvent *event)
{
    if (QMessageBox::question(this, "Exit", "Are you sure?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        event->accept();
    }
    else event->ignore();
}

bool Window::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->addButton && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton)
        {
            ui->removeButton->setText("Remove");
            Charge charge;
            charge.q = -charge.q;
            simulation.charges.append(charge);
            simulation.update();
            updateTable();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}
