#include "simulation.h"

QPointF fieldStrength(double x, double y, QList<Charge> charges)
{
    double Ex = 0.0;
    double Ey = 0.0;
    const double k = 9e9;
    
    for (int i = 0; i < charges.count(); i++)
    {
        double dx = x - charges.at(i).x;
        double dy = y - charges.at(i).y;
        double r = std::sqrt(dx * dx + dy * dy);
        
        if (r < 0.1) r = 0.1;
        
        double E = (k * charges.at(i).q) / (r * r);
        
        Ex += E * (dx / r);
        Ey += E * (dy / r);
    }
    
    return QPointF(Ex, Ey);
}

Simulation::Simulation(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(418, 465);
}

void Simulation::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), Qt::white);
    
    QPen pen(Qt::black, 1, Qt::SolidLine);
    painter.setPen(pen);
    
    painter.drawLine(width() / 2, 0, width() / 2, height());
    painter.drawLine(0, height() / 2, width(), height() / 2);
    
    pen.setColor(Qt::gray);
    pen.setWidthF(0.2);
    painter.setPen(pen);
    
    for (int i = width() / 2 + 40; i < width(); i += 40) painter.drawLine(i, 0, i, height());
    for (int i = width() / 2 - 40; i > 0; i -= 40) painter.drawLine(i, 0, i, height());
    for (int i = height() / 2 + 40; i < height(); i += 40) painter.drawLine(0, i, width(), i);
    for (int i = height() / 2 - 40; i > 0; i -= 40) painter.drawLine(0, i, width(), i);
    
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);
    
    for (int i = 0; i < charges.count(); ++i)
    {
        bool is_positive = charges.at(i).q > 0;
        if (charges.at(i).q == 0) continue;
        
        double cx = width() / 2 + charges.at(i).x * 20;
        double cy = height() / 2 - charges.at(i).y * 20;
        
        for (int j = 0; j < fieldLinesNumber; ++j)
        {
            double angle = (2.0 * M_PI * j) / fieldLinesNumber;
            
            double curX = cx + 8 * std::cos(angle);
            double curY = cy + 8 * std::sin(angle);
            
            QPainterPath path;
            path.moveTo(curX, curY);
            
            bool stop_line = false;
            
            for (int step = 0; step < 1000 && !stop_line; ++step)
            {
                double Ex = 0.0;
                double Ey = 0.0;
                
                for (int k = 0; k < charges.count(); ++k)
                {
                    double chX = width() / 2 + charges[k].x * 20;
                    double chY = height() / 2 - charges[k].y * 20;
                    
                    double dx = curX - chX;
                    double dy = curY - chY;
                    double dist_2 = dx * dx + dy * dy;
                    double dist = std::sqrt(dist_2);
                    
                    if (dist < 1.0) continue;
                    
                    double E_mag = charges[k].q / dist_2;
                    Ex += E_mag * (dx / dist);
                    Ey += E_mag * (dy / dist);
                }
                
                double E_total = std::sqrt(Ex * Ex + Ey * Ey);
                if (E_total < 1e-9) break;
                
                double direction = is_positive ? 1.0 : -1.0;
                double nextX = curX + direction * (Ex / E_total) * 2;
                double nextY = curY + direction * (Ey / E_total) * 2;
                
                if (nextX < 0 || nextX > width() || nextY < 0 || nextY > height())
                {
                    stop_line = true;
                }
                
                for (int ch = 0; ch < charges.count(); ++ch)
                {
                    if (ch == i && step < 3) continue; 
                    
                    double ch_x = width() / 2 + charges[ch].x * 20;
                    double ch_y = height() / 2 - charges[ch].y * 20;
                    
                    double dx = nextX - ch_x;
                    double dy = nextY - ch_y;
                    if ((dx * dx + dy * dy) < (8 * 8))
                    {
                        stop_line = true;
                        break;
                    }
                }
                
                path.lineTo(nextX, nextY);
                curX = nextX;
                curY = nextY;
            }
            painter.drawPath(path);
        }
    }
    
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    for (int i = 0; i < charges.count(); i++)
    {
        if (charges.at(i).q > 0)
        {
            pen.setColor(Qt::red);
            brush.setColor(Qt::red);
            painter.setBrush(brush);
            painter.setPen(pen);
        }
        else if (charges.at(i).q < 0)
        {
            pen.setColor(Qt::blue);
            brush.setColor(Qt::blue);
            painter.setBrush(brush);
            painter.setPen(pen);
        }
        else
        {
            pen.setColor(Qt::gray);
            brush.setColor(Qt::transparent);
            painter.setBrush(brush);
            painter.setPen(pen);
        }
        
        painter.drawEllipse(QPointF(width() / 2 + charges.at(i).x * 20, height() / 2 - charges.at(i).y * 20), 0.2 * 40, 0.2 * 40);
    }
}

void Simulation::mousePressEvent(QMouseEvent *event)
{
    for (int i = charges.count() - 1; i >= 0; i--)
    {
        if ((event->position().x() >= width() / 2 + charges.at(i).x * 20 - 0.2 * 40) &&
            (event->position().x() <= width() / 2 + charges.at(i).x * 20 + 0.2 * 40) &&
            (event->position().y() >= height() / 2 - charges.at(i).y * 20 - 0.2 * 40) &&
            (event->position().y() <= height() / 2 - charges.at(i).y * 20 + 0.2 * 40))
        {
            currentCharge = &charges[i];
            break;
        }
    }
    m_lastPos = event->position();
    event->accept();
}

void Simulation::mouseReleaseEvent(QMouseEvent *event)
{
    currentCharge = nullptr;
    event->accept();
}

void Simulation::mouseMoveEvent(QMouseEvent *event)
{
    if (!currentCharge)
    {
        event->ignore();
        return;
    }
    QPointF m_delta = event->position() - m_lastPos;
    m_lastPos = event->position();
    
    if (!rect().contains(event->position().toPoint()))
    {
        currentCharge = nullptr;
        update();
        event->accept();
        return;
    }
    
    currentCharge->x += m_delta.x() / 20;
    currentCharge->y -= m_delta.y() / 20;
    update();
    emit chargeMoved();
    event->accept();
}
