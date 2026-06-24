#ifndef SIMULATION_H
#define SIMULATION_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>

#include <cmath>

const double k = 9 * std::pow(10, 9);

struct Charge
{
    double x, y = 0.0;
    double q = 1.0;
};

class Simulation : public QWidget
{
    Q_OBJECT
public:
    explicit Simulation(QWidget *parent = nullptr);
    
    QList<Charge> charges;
    int fieldLinesNumber = 30;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Charge *currentCharge = nullptr;
    QPointF m_lastPos;

signals:
    void chargeMoved();
};

#endif // SIMULATION_H
