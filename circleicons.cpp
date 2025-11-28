#include "circleicons.h"
#include "qpainter.h"

CircleIcons::CircleIcons(QColor color, QWidget *parent)
    : QWidget(parent), baseColor(color)
{
    setFixedSize(30, 30);
}

void CircleIcons::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QLinearGradient gradient(0, 0, ledSize, ledSize);

    QColor light = baseColor.lighter(180);
    QColor dark  = baseColor.darker(200);


        gradient.setColorAt(0.0, light);
        gradient.setColorAt(0.4, baseColor);
        gradient.setColorAt(1.0, dark);


    p.setBrush(QBrush(gradient));
    p.setPen(Qt::NoPen);
    p.drawEllipse(0, 0, ledSize, ledSize);
}



