#ifndef FORECASTWIDGET_H
#define FORECASTWIDGET_H

#include <QWidget>
#include "weatherclient.h"
#include "qlabel.h"

class ForecastWidget : public QFrame
{
    Q_OBJECT
public:
    explicit ForecastWidget(QFrame *parent = nullptr);
    void setData(const ForecastEntry &entry, int TimeZone);
    void setIcon(const QString &path);
    void clear();


private:
    QLabel *timeLabel;
    QLabel *tempLabel;
    QLabel *descLabel;
    QLabel *windLabel;
    QLabel *humidityLabel;
    QLabel *IconLabel;
    QLabel *MinTemp;
    QLabel *MaxTemp;
    QLabel *visibility;
    QLabel *feels_like;

};

#endif // FORECASTWIDGET_H
