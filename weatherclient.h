#ifndef WEATHERCLIENT_H
#define WEATHERCLIENT_H

#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QDateTime>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
struct WeatherData {
    // Location
    QString city;
    QString country;
    double latitude;
    double longitude;

    // Weather
    QString Weathermain;
    QString description;
    QString icon;

    // Temperature & Atmosphere
    double temp;
    double feelsLike;
    double tempMin;
    double tempMax;
    int humidity;
    int pressure;
    int seaLevel;
    int groundLevel;

    // Wind
    double windSpeed;
    int windDeg;

    // Clouds & Visibility
    int cloudiness;
    int visibility;

    // Sun info
    QDateTime sunrise;
    QDateTime sunset;

    // Timestamp & timezone
    QDateTime timestamp;
    int timezone; // in seconds
};

struct AqiData
{
    // Coordinates
    double lat = 0.0;
    double lon = 0.0;

    // AQI (Air Quality Index)
    int aqi_us = 0;     // USA AQI scale (0–500)
    int aqi_cn = 0;     // China AQI scale (0–500)
    QString main_pollutant; // e.g. "pm25", "pm10", "o3", etc.

    // Timestamp
    qint64 dt = 0;

    QString mainPollutantReadable;

    // Location names (optional)
    QString city;
    QString state;
    QString country;
};


class WeatherClient: public QObject
{
    Q_OBJECT
public:
    explicit WeatherClient(QObject *parent = nullptr);
    void fetchWeather(QString City);

private:
    QNetworkAccessManager *networkManager;

    QString apiKey;
    QString AqiToken;
    void updateCurrentWeather(const QJsonObject &data);
    int calcAqiUS_PM25(double c);


private slots:

    //void fetchWeatherByLocation();
    void onWeatherDataReceived(QNetworkReply *reply);
    void onForecastDataReceived(QNetworkReply *reply);
    void onAQIDataReceived(QNetworkReply *reply);

signals:
    void WeatherCurrentDataReady(const WeatherData &data);
    void AQIDataReady(const AqiData &data);




};

#endif // WEATHERCLIENT_H
