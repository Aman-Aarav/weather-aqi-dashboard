#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "weatherclient.h"
#include <QPushButton>
#include "QTimer"
#include "QDateTime"
#include "gauge.h"
#include <QSlider>
#include <QPropertyAnimation>
#include "circleicons.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include "forecastwidget.h"
#include "QButtonGroup"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct DailyTemp {
    QString date;
    double minTemp;
    double maxTemp;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Searchbtn_clicked();
    void updateClock();
    //void on_dayNightBtn_toggled(bool checked);
    void onCurrentReceived(const WeatherData &data);
    void onAQIReceived(const AqiData &data);
    void onForecastDataReceived(const QVector<ForecastEntry> &data);
    void onDayButtonClicked();  // connected to all 5 day buttons
    void clearLayout(QLayout *layout);
    void setBtnEnabled();


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
    void setIcon(const QString &path);
    QChartView* createFeelsLikeChart(const QVector<ForecastEntry> &forecast);
    QVector<DailyTemp> computeDailyMinMax(const QVector<ForecastEntry> &forecast);
    QChartView* createMinMaxChart(const QVector<ForecastEntry> &forecast);
    QChartView *TempchartView=nullptr;

    QChartView *MinMaxchartView=nullptr;
    QVector<ForecastWidget*> dayWidgets;
    QVector<QPushButton*> dayButtons; // store your 5 buttons
     QVector<ForecastEntry> forecastData;
    int timezone;
     QButtonGroup *buttonGroup;

     bool isLoading = false;


    WeatherClient *WeatherObj;
};
#endif // MAINWINDOW_H
