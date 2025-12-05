#include "forecastwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include "QGraphicsDropShadowEffect"
ForecastWidget::ForecastWidget(QFrame *parent) : QFrame(parent)
{
    setFrameStyle(QFrame::NoFrame);


    QVBoxLayout *layout = new QVBoxLayout(this);
    timeLabel = new QLabel(this);
    tempLabel = new QLabel(this);
    descLabel = new QLabel(this);
    windLabel = new QLabel(this);
    humidityLabel = new QLabel(this);
    IconLabel = new QLabel(this);
    visibility = nullptr; //Not using
    MinTemp = new QLabel(this);
    MaxTemp = new QLabel(this);
    feels_like = new QLabel(this);

    layout->addWidget(IconLabel);
    layout->addWidget(timeLabel);
    layout->addWidget(tempLabel);
    layout->addWidget(descLabel);
    layout->addWidget(windLabel);
    layout->addWidget(humidityLabel);
    layout->addWidget(MinTemp);
    layout->addWidget(MaxTemp);
    layout->addWidget(feels_like);

    layout->setStretch(0,20);
    layout->setStretch(1,10);
    layout->setStretch(2,10);
    layout->setStretch(3,10);
    layout->setStretch(4,10);
    layout->setStretch(6,10);
    layout->setStretch(7,10);
    layout->setStretch(8,10);
    layout->setStretch(9,10);

    setLayout(layout);

    QString labelStyle =
        "QLabel {"
        "   background-color: transparent;"
        "border: none;"
        "   color: white;"
        "   font-size: 12px;"

        "}";

    timeLabel->setStyleSheet(labelStyle);
    tempLabel->setStyleSheet(labelStyle);
    descLabel->setStyleSheet(labelStyle);
    windLabel->setStyleSheet(labelStyle);
    humidityLabel->setStyleSheet(labelStyle);
    IconLabel->setStyleSheet(labelStyle);
    feels_like->setStyleSheet(labelStyle);
    MinTemp->setStyleSheet(labelStyle);
    MaxTemp->setStyleSheet(labelStyle);

}

void ForecastWidget::clear()
{
    timeLabel->clear();
    tempLabel->clear();
    descLabel->clear();
    windLabel->clear();
    humidityLabel->clear();
    feels_like->clear();
    MinTemp->clear();
    MaxTemp->clear();
    IconLabel->clear();

}


void ForecastWidget::setData(const ForecastEntry &entry, int TimeZone)
{

    timeLabel->setText("Time: " + entry.dt_txt.right(8));
    tempLabel->setText("Temp: " + QString::number(entry.Weathermain.temp) + "°C");
    descLabel->setText("Weather: " + entry.weather.description);
    windLabel->setText("Wind: " + QString::number(entry.wind.speed) + " m/s");
    humidityLabel->setText("Humidity: " + QString::number(entry.Weathermain.humidity) + "%");
    feels_like->setText("Feels like: " + QString::number(entry.Weathermain.feels_like) + "°C");
    MinTemp->setText("Min. Temperature: " + QString::number(entry.Weathermain.temp_min) + "°C");
    MaxTemp->setText("Max. Temperature: " + QString::number(entry.Weathermain.temp_max) + "°C");


    QString timeStr = entry.dt_txt.right(8);   // "18:30:00
    // Parse the HH:mm:ss time safely
    QTime time = QTime::fromString(timeStr, "HH:mm:ss");
    // In case parsing fails
    if (!time.isValid()) {
        qWarning() << "Invalid time format:" << timeStr;
    }
    // Build UTC datetime using the date from entry.dt_txt
    QDate date = QDate::fromString(entry.dt_txt.left(10), "yyyy-MM-dd");
    QDateTime utcTime(date, time, Qt::UTC);

    // Converting UTC  Local time using timezone offset (OpenWeather gives seconds offset)
    QDateTime localTime = utcTime.addSecs(TimeZone);

    // Extract hour
    int hour = localTime.time().hour();

    // Day / Night detection
    bool isDay = (hour >= 5 && hour < 18);  // 5 AM → 5:59 PM = Day

    // Clean description
    QString d = entry.weather.description.toLower().trimmed();

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
void ForecastWidget::setIcon(const QString &path)
{
    if(path.isEmpty())
        IconLabel->clear();
    QPixmap pix(path);
    IconLabel->setPixmap(
        pix.scaled(IconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    IconLabel->setAlignment(Qt::AlignHCenter);
    QGraphicsDropShadowEffect* weathershadow = new QGraphicsDropShadowEffect;
    weathershadow->setBlurRadius(80);           // Softness of the shadow
    weathershadow->setOffset(0, 0);
    weathershadow->setColor(QColor(98, 221, 255));
    IconLabel->setGraphicsEffect(weathershadow);
}
