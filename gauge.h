#ifndef GAUGEWIDGET_H
#define GAUGEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
class GaugeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit GaugeWidget(QWidget *parent = nullptr);

    int value() const { return m_value; }
    QSize minimumSizeHint() const override { return QSize(300, 180); }
    QSize sizeHint() const override { return QSize(500, 260); }

public slots:
    void setValue(int v);

signals:
    void valueChanged(int);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal valueToAngleDeg(int v) const; // maps 0..maxValue to 180..0 degrees (left -> right)
    void drawColoredSegments(QPainter &p, const QRectF &rect);
    void drawTicksAndLabels(QPainter &p, const QRectF &rect);
    void drawNeedle(QPainter &p, const QRectF &rect);

    int m_value;
    const int m_maxValue = 500;
};
#endif // GAUGEWIDGET_H
