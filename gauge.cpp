#include "gauge.h"
#include <QPainter>
#include <QConicalGradient>
#include <QLinearGradient>
#include <QFontMetrics>
#include <cmath>

GaugeWidget::GaugeWidget(QWidget *parent)
    : QWidget(parent), m_value(120)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
}

void GaugeWidget::setValue(int v)
{
    if (v < 0) v = 0;
    if (v > m_maxValue) v = m_maxValue;
    if (m_value == v) return;
    m_value = v;
    emit valueChanged(m_value);
    update();
}

// Map value [0..maxValue] -> degrees: 180 (left) .. 0 (right)
qreal GaugeWidget::valueToAngleDeg(int v) const
{
    qreal frac = qreal(v) / qreal(m_maxValue);
    return 180.0 - frac * 180.0;
}

void GaugeWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // calculate drawing rect (making it wide but not too tall)
    const int margin = 38;
    int w = width() - 2 * margin;
    int h = height() - 2 * margin;
    // Use a square for the circular part
    int side = qMin(w, h * 2); // ensure semicircle fits nicely
    QRectF arcRect((width() - side) / 2.0, margin, side, side);



    // Draw outer white gauge frame
    QRectF innerRect = arcRect.adjusted(12,12, -12, -12);

    // Colored segments
    drawColoredSegments(p, innerRect);

    // Inner white disc (gives the look in the example)
    QRectF innerDisc = innerRect.adjusted(14,14,-14,-14);
    p.setBrush(QBrush(Qt::transparent));
    p.setPen(Qt::NoPen);
    p.drawEllipse(innerDisc);

    // ticks and labels
    drawTicksAndLabels(p, innerRect);

    // needle and hub
    drawNeedle(p, innerRect);
}

void GaugeWidget::drawColoredSegments(QPainter &p, const QRectF &rect)
{
    // segments defined as pairs {startValue, endValue, color}
    struct Seg { int a,b; QColor c; };
    QVector<Seg> segs = {
        {0, 50, QColor(64, 222, 100)},     // green
        {50,100, QColor(240, 200, 70)},    // yellow
        {100,150, QColor(240,140,40)},     // orange
        {150,200, QColor(235,75,50)},      // red
        {200,300, QColor(200,30,30)},      // deep red
        {300,500, QColor(140,18,18)}       // maroon
    };

    QPen pen;
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidthF(rect.width() * 0.095); // thickness proportional to size

    for (const Seg &s : segs) {
        qreal angleStart = valueToAngleDeg(s.a);
        qreal angleEnd   = valueToAngleDeg(s.b);
        qreal sweep = angleEnd - angleStart; // likely negative (clockwise)
        pen.setColor(s.c);
        p.setPen(pen);

        // arcto expects startAngle degrees measured from 3 O'clock counterclockwise
        // We computed angles measured from 3 O'clock as well, valueToAngleDeg uses that
        // Use QPainterPath::arcTo to draw with a pen
        QPainterPath path;
        QRectF r = rect;
        // we want the arc centered on rect; use arc on a rectangle slightly larger because pen has width
        path.moveTo(rect.center());
        // creating a thin arc path by using stroked path is simpler: draw using drawArc with pen
        // drawArc expects angles in 1/16 deg
        int start16 = int(angleStart * 16.0);
        int span16  = int(sweep * 16.0);
        // drawArc uses rectangle coordinates; we need to use the same rect for arc
        p.drawArc(rect, start16, span16);
    }
}

void GaugeWidget::drawTicksAndLabels(QPainter &p, const QRectF &rect)
{
    // tick values and label values (the example shows 0,50,100,200,300,400,500)
    QVector<int> major = {0,50,100,200,300,400,500};

    QPointF center = rect.center();
    qreal radius = rect.width() / 2.0;
    qreal tickOuter = radius - (rect.width() * 0.095) / 2.0; // outside of colored arc
    qreal tickInnerMajor = tickOuter - (rect.width() * 0.06);
    qreal tickInnerMinor = tickOuter - (rect.width() * 0.035);

    QPen pen(Qt::darkGray);
    pen.setWidthF(2.0);
    p.setPen(pen);

    QFont f = font();
    f.setBold(true);
    f.setPointSizeF(rect.width() * 0.06 / 1.6);
    p.setFont(f);
    QFontMetrics fm(f);

    // draw small ticks between major (optional)
    for (int v = 0; v <= m_maxValue; v += 10) {
        qreal ang = valueToAngleDeg(v);
        qreal rad = ang * M_PI / 180.0;
        qreal cosv = cos(rad);
        qreal sinv = sin(rad);
        QPointF pOut(center.x() + tickOuter * cosv, center.y() - tickOuter * sinv);
        QPointF pIn = (v % 50 == 0) ? QPointF(center.x() + tickInnerMajor * cosv, center.y() - tickInnerMajor * sinv)
                                    : QPointF(center.x() + tickInnerMinor * cosv, center.y() - tickInnerMinor * sinv);
        if (v % 50 == 0) {
            QPen penMajor(Qt::darkGray);
            penMajor.setWidthF(2.4);
            p.setPen(penMajor);
        } else {
            QPen penMinor(Qt::darkGray);
            penMinor.setWidthF(1.0);
            p.setPen(penMinor);
        }
        p.drawLine(pOut, pIn);
    }

}

void GaugeWidget::drawNeedle(QPainter &p, const QRectF &rect)
{
    QPointF center = rect.center();
    qreal radius = rect.width() / 2.0;

    // needle geometry
    qreal needleLen = radius * 0.78;
    qreal needleWidth = rect.width() * 0.045;

    qreal angleDeg = valueToAngleDeg(m_value);
    qreal angleRad = angleDeg * M_PI / 180.0;
    qreal nx = center.x() + needleLen * cos(angleRad);
    qreal ny = center.y() - needleLen * sin(angleRad);


    QPen pen;
    // needle body
    QGradient grad(QGradient::FlyHigh );
    pen.setBrush(grad);
    pen.setWidthF(needleWidth);
    pen.setCapStyle(Qt::RoundCap);

    p.setPen(pen);
    p.drawLine(center, QPointF(nx, ny));

    // hub circleF
    QGradient gradcircle(QGradient::DirtyBeauty);
    p.setBrush(gradcircle);
    p.setPen(QPen(QColor(90,110,140), 3));
    qreal hubR = rect.width() * 0.06;
    p.drawEllipse(center, hubR, hubR);
}
