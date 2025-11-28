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


    // optional: animate to a target value on startup
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



    // QGraphicsBlurEffect *blur1 = new QGraphicsBlurEffect;
    // blur1->setBlurRadius(10);
    // ui->widget_11->setGraphicsEffect(blur1);
    // QGraphicsDropShadowEffect* shadow1 = new QGraphicsDropShadowEffect;
    // shadow1->setBlurRadius(100);           // Softness of the shadow
    // shadow1->setOffset(0, 0);
    // shadow1->setColor(Qt::black);
    // ui->widget_10->setGraphicsEffect(shadow1);
    // QPixmap pixmap(":/Images/sun.png");
    // ui->IconLabel->setPixmap(
    //     pixmap.scaled(ui->IconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
    //     );

    QTime time = QTime::currentTime();
    ui->lcdNumber->display(time.toString("hh:mm:ss"));

    ui->dayNightBtn->setCheckable(true);  // makes it toggle
    ui->dayNightBtn->setFixedSize(80, 40);
    ui->dayNightBtn->setText("☀");       // Sun for day by default

   ui->lineEditPlace->setPlaceholderText("🔍  Search for your preferred city...");

    ui->dayNightBtn->setStyleSheet(R"(
    QPushButton {
        border-radius: 20px;
        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0 #f5f5f5, stop:1 #e0e0e0);
        color: #ffcc00;
        font-size: 20px;
    }
    QPushButton:checked {
        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0 #333333, stop:1 #121212);
        color: #ffffff;
    }
    QPushButton:hover {
        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0 #fafafa, stop:1 #dcdcdc);
    }
    QPushButton:checked:hover {
        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0 #444444, stop:1 #1a1a1a);
    }
    )");


    connect(WeatherObj, &WeatherClient::WeatherCurrentDataReady, this, &MainWindow::onCurrentReceived);
    connect(WeatherObj, &WeatherClient::AQIDataReady, this, &MainWindow::onAQIReceived);

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

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Searchbtn_clicked()
{
    QString city = ui->lineEditPlace->text();
    WeatherObj->fetchWeather(city);

}


void MainWindow::on_dayNightBtn_toggled(bool checked)
{
    if(checked) {
        ui->dayNightBtn->setText("🌙"); // Moon for night
       // qApp->setStyleSheet("QWidget { background-color: #121212; color: white; }");
    } else {
        ui->dayNightBtn->setText("☀"); // Sun for day
       // qApp->setStyleSheet("QWidget { background-color: #ffffff; color: black; }");
    }
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
    ui->Feellikelabel->setText(QString("%1°C").arg(qRound(data.feelsLike)));


    QDateTime utcTime = data.timestamp.toUTC();
    QDateTime localTime = utcTime.addSecs(data.timezone);
    QString dayName = localTime.toString("dddd");
    QString dateStr = localTime.toString("dd/MM/yyyy");
    QString timeStr = localTime.toString("hh:mm AP");
    ui->Daylabel->setText(dayName + ",  " + dateStr);
    ui->lcdNumber->display(timeStr);

    ui->label_Pressure->setText(QString("%1 hPa").arg(data.pressure));
    ui->labelHumidity->setText(QString("%1 %").arg(data.humidity));
   // ui->label_visiblilty->setText(QString("%1°C").arg());
    ui->label_wind->setText(QString("%1 km/h").arg(data.windSpeed));

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


}

