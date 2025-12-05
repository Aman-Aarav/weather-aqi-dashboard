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
struct WeatherInfo {
    int id;
    QString Weathermain;
    QString description;
    QString icon;
};

struct MainInfo {
    double temp;
    double feels_like;
    double temp_min;
    double temp_max;
    int pressure;
    int humidity;
    int sea_level;
    int grnd_level;
};

struct WindInfo {
    double speed;
    int deg;
    double gust;   // may be missing → set to 0
};

struct CloudsInfo {
    int all;
};

struct SysInfo {
    QString country;
    long sunrise;
    long sunset;
};

struct ForecastEntry {
    long dt; //timestamp
    QString dt_txt;       // datetime string
    MainInfo Weathermain;
    WeatherInfo weather;
    WindInfo wind;
    CloudsInfo clouds;
    SysInfo sys;
    int visibility;
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
    static QVector<ForecastEntry> parseForecast(const QByteArray &data);
    void handleNetworkError(QNetworkReply *reply);
    void handleHttpError(int statusCode, QNetworkReply *reply);



private slots:

    //void fetchWeatherByLocation();
    void onWeatherDataReceived(QNetworkReply *reply);
    void onForecastDataReceived(QNetworkReply *reply);
    void onAQIDataReceived(QNetworkReply *reply);
    QVector<ForecastEntry> parseDataForecast(const QByteArray &data);

signals:
    void WeatherCurrentDataReady(const WeatherData &data);
    void AQIDataReady(const AqiData &data);
    void ForecastDataReady(const QVector<ForecastEntry> &data);
    void enablebtn();




};

#endif // WEATHERCLIENT_H
