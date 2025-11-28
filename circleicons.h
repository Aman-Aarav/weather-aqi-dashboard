#ifndef CIRCLEICONS_H
#define CIRCLEICONS_H

#include <QWidget>
#include <QObject>

class CircleIcons : public QWidget
{
    Q_OBJECT
public:
    explicit CircleIcons(QColor baseColor = Qt::red, QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;


private:
    int ledSize = 25;
    QColor baseColor;    // << store the custom color
};

#endif // CIRCLEICONS_H
