#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "weatherclient.h"
#include "QTimer"
#include "QDateTime"
#include "gauge.h"
#include <QSlider>
#include <QPropertyAnimation>
#include "circleicons.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Searchbtn_clicked();
    void updateClock();
    void on_dayNightBtn_toggled(bool checked);
    void onCurrentReceived(const WeatherData &data);
    void onAQIReceived(const AqiData &data);

private:
    Ui::MainWindow *ui;
    QTimer *clockTimer;

    GaugeWidget *gauge;
    QSlider *gaugeslider;
    CircleIcons *red;
    CircleIcons *yellow;
    CircleIcons *green;
    CircleIcons *maroon;
    CircleIcons *orange;
    CircleIcons *reddishorange;


    WeatherClient *WeatherObj;
};
#endif // MAINWINDOW_H
