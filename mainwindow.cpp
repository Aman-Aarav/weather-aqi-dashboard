#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QGraphicsDropShadowEffect"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    WeatherObj = new WeatherClient();

    gauge = new GaugeWidget();
    ui->Gaugeaqilayout->addWidget(gauge);

    QGraphicsDropShadowEffect* Gaugeshadow = new QGraphicsDropShadowEffect;
    Gaugeshadow->setBlurRadius(50);           // Softness of the shadow
    Gaugeshadow->setOffset(0, 0);
    Gaugeshadow->setColor(Qt::black);
    gauge->setGraphicsEffect(Gaugeshadow);

    // slider to change value interactively
    gaugeslider = new QSlider(Qt::Horizontal);
    gaugeslider->setRange(0, 500);
    gaugeslider->setValue(120);


    //  animate to a target value on startup
    QPropertyAnimation *anim = new QPropertyAnimation(gauge, "value", this);
    anim->setDuration(1200);
    anim->setStartValue(0);
    anim->setEndValue(200);
    anim->start(QAbstractAnimation::DeleteWhenStopped);


    // Clock timer
    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    clockTimer->start(1000);
    updateClock();




    QTime time = QTime::currentTime();
    ui->lcdNumber->display(time.toString("hh:mm:ss"));



    ui->lineEditPlace->setPlaceholderText("🔍  Search for your preferred city...");

    connect(WeatherObj, &WeatherClient::WeatherCurrentDataReady, this, &MainWindow::onCurrentReceived);
    connect(WeatherObj, &WeatherClient::AQIDataReady, this, &MainWindow::onAQIReceived);
    connect(WeatherObj, &WeatherClient::ForecastDataReady, this, &MainWindow::onForecastDataReceived);


    red = new CircleIcons(QColor(200,30,30));
    yellow = new CircleIcons(QColor(QColor(240, 200, 70)));
    orange = new CircleIcons(QColor(QColor(240,140,40)));
    green = new CircleIcons(QColor("#56D905"));
    maroon = new CircleIcons(QColor(140,18,18));
    reddishorange = new CircleIcons(QColor(235,75,50));
    ui->SatisfactoryConditionLayout->insertWidget(0, yellow);
    ui->PoorConditionLayout->insertWidget(0, reddishorange);
    ui->GoodConditionLayout->insertWidget(0, green);
    ui->VPoorConditionLayout->insertWidget(0, red);
    ui->HazardousConditionLayout->insertWidget(0, maroon);
    ui->ModerateConditionLayout->insertWidget(0, orange);


    dayButtons = {ui->Day1btn, ui->Day2btn, ui->Day3btn, ui->Day4btn, ui->Day5btn};

    for (int i = 0; i < dayButtons.size(); ++i) {
        dayButtons[i]->setProperty("dayIndex", i); // store index
        connect(dayButtons[i], &QPushButton::clicked, this, &MainWindow::onDayButtonClicked);
    }


    // Create 8 widgets
    for (int i = 0; i < 8; ++i) {
        ForecastWidget *w = new ForecastWidget();
        dayWidgets.append(w);
        ui->houlylayout->addWidget(w);
        QString StyleWeatherhourwidget = R"(
            background-color: rgba(255, 255, 255, 0.15);
            border-radius: 15px;
            border: 1px solid rgba(255, 255, 255, 0.3);
        )";
        w->setStyleSheet(StyleWeatherhourwidget);
    }

    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);
    buttonGroup->addButton(ui->Day1btn);
    buttonGroup->addButton(ui->Day2btn);
    buttonGroup->addButton(ui->Day3btn);
    buttonGroup->addButton(ui->Day4btn);
    buttonGroup->addButton(ui->Day5btn);

    ui->Day4btn->hide(); // initially, will be visible on data received
    ui->Day2btn->hide();
    ui->Day3btn->hide();
    ui->Day1btn->hide();
    ui->Day5btn->hide();

    connect(WeatherObj, &WeatherClient::enablebtn, this, &MainWindow::setBtnEnabled);
    ui->ForecastWidget->hide();




}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setBtnEnabled(){
    ui->Searchbtn->setEnabled(true);
}
void MainWindow::on_Searchbtn_clicked()
{
    if (isLoading) return;  // already fetching ignore extra clicks

    ui->Searchbtn->setEnabled(false);
    QString city = ui->lineEditPlace->text();
    isLoading = true;
    WeatherObj->fetchWeather(city);

}




void MainWindow::updateClock() {
    QDateTime now = QDateTime::currentDateTime();
    //timeLabel->setText(now.toString("hh:mm"));
    //dateLabel->setText(now.toString("dddd, d MMM"));
}
void MainWindow::onCurrentReceived(const WeatherData &data)
{
    ui->labelPlace->setText(data.city + ", " + data.country);
    ui->WeatherType->setText(data.description);

    ui->TempLabel->setText(QString("%1°C").arg(qRound(data.temp)));
    ui->Feellikelabel->setText(QString("Feels like: %1°C").arg(qRound(data.feelsLike)));
    timezone = data.timezone; // for sinding to forecast


    QDateTime utcTime = data.timestamp.toUTC();
    QDateTime localTime = utcTime.addSecs(data.timezone);
    QString dayName = localTime.toString("dddd");
    QString dateStr = localTime.toString("dd/MM/yyyy");
    QString timeStr = localTime.toString("hh:mm AP");
    ui->Daylabel->setText(dayName + ",  " + dateStr);
    ui->lcdNumber->display(timeStr);


    ui->label_Pressure->setText(QString("%1 hPa").arg(data.pressure));
    ui->labelHumidity->setText(QString("%1 %").arg(data.humidity));
    ui->label_visiblilty->setText(QString("%1 m").arg(data.visibility));
    ui->label_wind->setText(QString("%1 km/h").arg(data.windSpeed));
    ui->MaxTemplabel->setText(QString("Max Temp.: %1°C ").arg(data.tempMax));
    ui->MinTemplabel->setText(QString("Min Temp.: %1°C ").arg(data.tempMin));
    ui->SunriseLabel->setText(QString("Sunrise: %1 ").arg(data.sunrise.time().toString()));
    ui->SunsetLabel->setText(QString("Sunset: %1 ").arg(data.sunset.time().toString()));

    // ui->SunriseLabel->setText(QString("Sunrise: %1°C ").arg(data.));
    // ui->SunsetLabel->setText(QString("Sunset: %1°C ").arg(data.tempMin));
    int hour = localTime.time().hour();
    bool isDay = (hour >= 5 && hour < 18);  // 6 AM – 6 PM is day
    QString d = data.description.toLower().trimmed();

    if (d == "clear sky") {
        if (isDay)
            setIcon(":/Images/clear-sky.png");
        else
            setIcon(":/Images/night (2).png");
    }
    else if (d == "few clouds") {
        setIcon(":/Images/cloudy (2).png");
    }
    else if (d == "scattered clouds") {
        setIcon(":/Images/overcast.png");
    }
    else if (d == "broken clouds" || d == "overcast clouds" || d.contains("clouds")) {
        setIcon(":/Images/stratuscumulus.png");
    }
    else if (d == "light rain" || d == "moderate rain" || d == "shower rain" || d == "light intensity shower rain") {
        setIcon(":/Images/icons8-light-rain-96.png");
    }
    else if (d == "rain" || d.contains("rain")) {
        setIcon(":/Images/icons8-cloud-with-rain-96.png");
    }

    else if (d == "thunderstorm" || d.contains("thunderstorm") || d.contains("storm")) {
        setIcon(":/Images/icons8-storm-96.png");
    }
    else if (d == "snow" || d.contains("snow") || d == "sleet" || d.contains("sleet")) {
        setIcon(":/Images/snowfall.png");
    }
    else if (d == "mist" || d.contains("Ash") || d.contains("volcanic ash") || d.contains("squalls") ) {
        setIcon(":/Images/mist.png");
    }
    else if (d == "fog" || d.contains("fog")) {
        setIcon(":/Images/fog.png");
    }
    else if (d.contains("haze") || d == "haze") {
        setIcon(":/Images/icons8-haze-100.png");
    }
    else if (d == "smoke" || d.contains("smoke")) {
        setIcon(":/Images/icons8-smoke-96.png");
    }
    else if (d == "dust" || d == "sand" || d.contains("dust") || d.contains("sand")) {
        setIcon(":/Images/icons8-dust-100.png");
    }
    else if (d == "drizzle" || d.contains("drizzle")) {
        setIcon(":/Images/icons8-light-rain-96.png");
    }




}
void MainWindow::setIcon(const QString &path)
{
    QPixmap pix(path);
    ui->IconLabel->setPixmap(
        pix.scaled(ui->IconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    ui->IconLabel->setAlignment(Qt::AlignHCenter);
    QGraphicsDropShadowEffect* weathershadow = new QGraphicsDropShadowEffect;
    weathershadow->setBlurRadius(80);           // Softness of the shadow
    weathershadow->setOffset(0, 0);
    weathershadow->setColor(QColor(98, 221, 255));
    ui->IconLabel->setGraphicsEffect(weathershadow);
}
void MainWindow::onAQIReceived(const AqiData &data)
{
    ui->labelAqiUs->setText(QString::number(data.aqi_us));
    ui->label_MailPollutant->setText(data.main_pollutant);

    gaugeslider->setValue(data.aqi_us);
    QPropertyAnimation *anim = new QPropertyAnimation(gauge, "value", this);
    anim->setDuration(1200);
    anim->setStartValue(0);
    anim->setEndValue(data.aqi_us);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    ui->label_MailPollutant->setText(data.mainPollutantReadable);

     isLoading = false;
    ui->Searchbtn->setEnabled(true);


}

void MainWindow::onForecastDataReceived(const QVector<ForecastEntry> &data)
{
    forecastData= data;
    ui->ForecastWidget->show();

    // CLEAR OLD CHARTS To AVOID MULTIPLE CHARTS
    clearLayout(ui->MinMaxLayout);

    MinMaxchartView = createMinMaxChart(data);
    ui->MinMaxLayout->addWidget(MinMaxchartView);

    ui->Day3btn->show();
    ui->Day4btn->show();
    ui->Day5btn->show();
    ui->Day2btn->show();
    ui->Day1btn->show();
    ui->Day3btn->setText(forecastData[0].dt_txt.first(10));
    ui->Day4btn->setText(forecastData[8].dt_txt.first(10));
    ui->Day5btn->setText(forecastData[16].dt_txt.first(10));

}

QChartView* MainWindow::createFeelsLikeChart(const QVector<ForecastEntry> &forecast)
{
    QLineSeries *actualSeries = new QLineSeries();
    actualSeries->setName("Actual Temp");
    actualSeries->setPointsVisible(true);


    QLineSeries *feelsSeries = new QLineSeries();
    feelsSeries->setName("Feels Like");
    feelsSeries->setPointsVisible(true);


    // Colors
    actualSeries->setColor(Qt::red);
    feelsSeries->setColor(QColor("#f5c542"));   // soft yellow

    // Fill the series from forecast data
    for (int i = 0; i < forecast.size(); i++) {
        double x = i;
        actualSeries->append(x, forecast[i].Weathermain.temp);
        feelsSeries->append(x, forecast[i].Weathermain.feels_like);
    }

    // Chart setup
    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    chart->setTitle("Actual vs Feels-like Temperature");
    chart->addSeries(actualSeries);
    chart->addSeries(feelsSeries);

    // X Axis (use time labels)
    QCategoryAxis *axisX = new QCategoryAxis();
    axisX->setTitleText("Time");

    int step = qMax(1, forecast.size() / 6);

    for (int i = 0; i < forecast.size(); i += step) {
        axisX->append(forecast[i].dt_txt.split(" ").last(), i);
    }
    axisX->setLabelsAngle(45);

    // Y Axis (Temperature)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Temperature (°C)");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    actualSeries->attachAxis(axisX);
    actualSeries->attachAxis(axisY);
    feelsSeries->attachAxis(axisX);
    feelsSeries->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);

    return view;
}
QVector<DailyTemp> MainWindow::computeDailyMinMax(const QVector<ForecastEntry> &forecast)
{
    QMap<QString, DailyTemp> map;

    for (const ForecastEntry &f : forecast) {
        QString date = f.dt_txt.split(" ").first();  // "2025-11-12"
        double tmin = f.Weathermain.temp_min;
        double tmax = f.Weathermain.temp_max;

        if (!map.contains(date)) {
            map[date] = { date, tmin, tmax };
        } else {
            map[date].minTemp = qMin(map[date].minTemp, tmin);
            map[date].maxTemp = qMax(map[date].maxTemp, tmax);
        }
    }

    return map.values().toVector();
}
QChartView* MainWindow::createMinMaxChart(const QVector<ForecastEntry> &forecast)
{
    QVector<DailyTemp> days = computeDailyMinMax(forecast);

    QBarSet *minSet = new QBarSet("Min Temp");
    QBarSet *maxSet = new QBarSet("Max Temp");

    minSet->setColor(QColor("#4fa3f7"));   // Blue
    maxSet->setColor(QColor("#f5c542"));   // Yellow

    QStringList categories;

    for (const DailyTemp &d : days) {
        *minSet << d.minTemp;
        *maxSet << d.maxTemp;
        categories << d.date.right(8);  // "11-12"
    }

    QBarSeries *series = new QBarSeries();
    series->append(minSet);
    series->append(maxSet);

    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    chart->addSeries(series);
    chart->setTitle("Daily Min–Max Temperature");

    // X-axis (Dates)
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Y-axis (Temperature)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Temperature (°C)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);

    return view;
}

void MainWindow::clearLayout(QLayout *layout)
{
    if (!layout) return;

    while (QLayoutItem *item = layout->takeAt(0)) { //Removes the first item
        if (QWidget *w = item->widget()) {
            w->setParent(nullptr);
            delete w; // deletes QChartView → deletes QChart → deletes series & axes
        }
        delete item;
    }
}

void MainWindow::onDayButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    int dayIndex = button->property("dayIndex").toInt();
    QString selectedDate = forecastData[dayIndex*8].dt_txt.left(10);

    // clearing widgets first to clear data, as maybe current day does not have 8 entries
    for (int i = 0; i < 8; i++) {
        dayWidgets[i]->clear();
    }

    // Collecting entries matching the date
    QVector<ForecastEntry> todaysEntries;
    for (const ForecastEntry &f : forecastData) {
        if (f.dt_txt.left(10) == selectedDate)
            todaysEntries.append(f);
    }

    //  Filling widgets based on available entries
    int count = qMin(todaysEntries.size(), 8);
    for (int i = 0; i < count; i++) {
        dayWidgets[i]->setData(todaysEntries[i], timezone);
    }

     //  UPDATING THE CHART BASED ON SELECTED DAY
    // Remove old chart
    if (TempchartView) {
        ui->Tempgraphlayout->removeWidget(TempchartView);
        delete TempchartView;
        TempchartView = nullptr;
    }
    // Creating new chart using only today's forecast entries
    TempchartView = createFeelsLikeChart(todaysEntries);
    ui->Tempgraphlayout->addWidget(TempchartView);



}
