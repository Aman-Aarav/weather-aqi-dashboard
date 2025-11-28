#include "weatherclient.h"
#include "QMessageBox"
WeatherClient::WeatherClient(QObject *parent) : QObject(parent), networkManager(new QNetworkAccessManager(this)) {
    apiKey = "7f8e532475ee9c9a810c40a62c93ac7b"; // OpenWeatherMap API key
    AqiToken= "01661bcf-a226-4f4c-b82e-1d2d5341343c";


    connect(networkManager, &QNetworkAccessManager::finished,
            this, [this](QNetworkReply *reply) {
                QString url = reply->url().toString();
                if (url.contains("weather?")) {
                    onWeatherDataReceived(reply);
                } else if (url.contains("forecast?")) {
                    onForecastDataReceived(reply);
                }
                else if (url.contains("airvisual")) {
                    onAQIDataReceived(reply);


                }
    });
}
void WeatherClient::fetchWeather(QString city) {


    if (city.isEmpty()) {
        //QMessageBox::warning(this, "Input Error", "Please enter a city name.");

        return;
    }

    if (apiKey == "YOUR_API_KEY_HERE") {
       // QMessageBox::warning(this, "API Key Required",
         //                    "Please get a free API key from https://openweathermap.org/api");

        return;
    }

    QString weatherUrl = QString("https://api.openweathermap.org/data/2.5/weather?q=%1&appid=%2&units=metric")
                             .arg(city, apiKey);
    networkManager->get(QNetworkRequest(QUrl(weatherUrl)));

    QString forecastUrl = QString("https://api.openweathermap.org/data/2.5/forecast?q=%1&appid=%2&units=metric")
                              .arg(city, apiKey);
    networkManager->get(QNetworkRequest(QUrl(forecastUrl)));


}

void WeatherClient::onWeatherDataReceived(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObj = jsonDoc.object();

        if (jsonObj.contains("cod") && jsonObj["cod"].toInt() == 200) {
            updateCurrentWeather(jsonObj);

        }
    }
    reply->deleteLater();
}

void WeatherClient::onForecastDataReceived(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObj = jsonDoc.object();


        if (jsonObj.contains("cod") && jsonObj["cod"].toString() == "200") {
            QJsonArray forecastList = jsonObj["list"].toArray();
            //updateHourlyForecast(jsonObj);
            //updateDailyForecast(jsonObj);
           // createTemperatureChart(forecastList);
            //createWeeklyChart(forecastList);
        }
    }
    reply->deleteLater();
}
void WeatherClient::onAQIDataReceived(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        qInfo()<<"fgdfgsh";
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

    if (!jsonDoc.isObject()) {
        reply->deleteLater();
        qInfo()<<"fgdfgshsdfs3";
        return;
    }

    QJsonObject root = jsonDoc.object();
    if (root["status"].toString() != "success") {
        reply->deleteLater();
        return;
    }

    AqiData data;

    QJsonObject dataObj = root["data"].toObject();
    data.city = dataObj["city"].toString();
    data.state = dataObj["state"].toString();
    data.country = dataObj["country"].toString();

    // coordinates
    QJsonArray coords = dataObj["location"].toObject()["coordinates"].toArray();
    data.lon = coords.at(0).toDouble();
    data.lat = coords.at(1).toDouble();

    // pollution
    QJsonObject pollution = dataObj["current"].toObject()["pollution"].toObject();
    data.aqi_us = pollution["aqius"].toInt();   // USA AQI
    data.aqi_cn = pollution["aqicn"].toInt();   // China AQI
    data.main_pollutant = pollution["mainus"].toString();

    QString polluntant = data.main_pollutant;
    if (polluntant == "p2") polluntant = "PM2.5";
    else if (polluntant == "p1") polluntant = "PM10";
    else if (polluntant == "o3") polluntant = "Ozone (O₃)";
    else if (polluntant == "n2") polluntant = "Nitrogen Dioxide (NO₂)";
    else if (polluntant == "s2") polluntant = "Sulphur Dioxide (SO₂)";
    else if (polluntant == "co") polluntant = "Carbon monoxide (CO)";
    data.mainPollutantReadable = polluntant;


    qInfo()<<data.aqi_us;
    qInfo()<<data.main_pollutant;

    emit AQIDataReady(data);

    reply->deleteLater();
}


void WeatherClient::updateCurrentWeather(const QJsonObject &data) {
    WeatherData wd;

    // --- Location ---
    QJsonObject coord = data["coord"].toObject();
    wd.latitude = coord["lat"].toDouble();
    wd.longitude = coord["lon"].toDouble();
    wd.city = data["name"].toString();
    wd.country = data["sys"].toObject()["country"].toString();
    qInfo()<<wd.city;

    QString AQIUrl = QString("https://api.airvisual.com/v2/nearest_city?lat=%1&lon=%2&key=%3")
                            .arg(wd.latitude)
                            .arg(wd.longitude)
                            .arg(AqiToken);

    networkManager->get(QNetworkRequest(QUrl(AQIUrl)));

    // --- Weather ---
    QJsonArray weatherArray = data["weather"].toArray();
    if (!weatherArray.isEmpty()) {
        QJsonObject weather = weatherArray.first().toObject();
        wd.Weathermain = weather["main"].toString();
        wd.description = weather["description"].toString();
        wd.icon = weather["icon"].toString();
    }

    // --- Temperature & Atmosphere ---
    QJsonObject main = data["main"].toObject();
    wd.temp = main["temp"].toDouble();
    wd.feelsLike = main["feels_like"].toDouble();
    wd.tempMin = main["temp_min"].toDouble();
    wd.tempMax = main["temp_max"].toDouble();
    wd.humidity = main["humidity"].toInt();
    wd.pressure = main["pressure"].toInt();
    wd.seaLevel = main.value("sea_level").toInt();
    wd.groundLevel = main.value("grnd_level").toInt();
    // --- Wind ---
    QJsonObject wind = data["wind"].toObject();
    wd.windSpeed = wind["speed"].toDouble();
    wd.windDeg = wind["deg"].toInt();

    // --- Clouds & Visibility ---
    wd.cloudiness = data["clouds"].toObject()["all"].toInt();
    wd.visibility = data["visibility"].toInt();

    // --- Sun Info ---
    QJsonObject sys = data["sys"].toObject();
    wd.sunrise = QDateTime::fromSecsSinceEpoch(sys["sunrise"].toVariant().toLongLong());
    wd.sunset = QDateTime::fromSecsSinceEpoch(sys["sunset"].toVariant().toLongLong());

    // --- Timestamp & timezone ---
    wd.timestamp = QDateTime::fromSecsSinceEpoch(data["dt"].toVariant().toLongLong());
    wd.timezone = data["timezone"].toInt();

    // --- Example: use the data ---
    qDebug() << "City:" << wd.city << wd.country;
    qDebug() << "Temp:" << wd.temp << "Feels Like:" << wd.feelsLike;
    qDebug() << "Weather:" << wd.Weathermain << "(" << wd.description << ")";
    emit WeatherCurrentDataReady(wd);
}


