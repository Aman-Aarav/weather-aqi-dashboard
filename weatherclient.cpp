#include "weatherclient.h"
#include "QMessageBox"
WeatherClient::WeatherClient(QObject *parent) : QObject(parent), networkManager(new QNetworkAccessManager(this)) {
    apiKey = "7f8e532475ee9c9a810c40a62c93ac7b"; // OpenWeatherMap API key
    AqiToken= "01661bcf-a226-4f4c-b82e-1d2d5341343c";


    connect(networkManager, &QNetworkAccessManager::finished,
            this, [this](QNetworkReply *reply) {

        //   Checking network error
                if (reply->error() != QNetworkReply::NoError) {
                    qDebug() << "Network error:" << reply->errorString()
                             << "URL:" << reply->url();

                    handleNetworkError(reply);
                    reply->deleteLater();
                    emit enablebtn();

                    return;
                }
                //   Checking HTTP status code
                int statusCode =
                    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

                QString reason =
                    reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

                qDebug() << "HTTP Status:" << statusCode << reason;

                if (statusCode < 200 || statusCode >= 300) {
                    qDebug() << "HTTP error from server:" << statusCode << reply->url();
                    handleHttpError(statusCode, reply);
                    reply->deleteLater();
                    emit enablebtn();

                    return;
                }


                QString url = reply->url().toString();
                if (url.contains("weather?")) {
                    onWeatherDataReceived(reply);
                } else if (url.contains("forecast?")) {
                    onForecastDataReceived(reply);
                }
                else if (url.contains("airvisual")) {
                    onAQIDataReceived(reply);
                }
                else {
                    qDebug() << "Unknown API endpoint:" << url;
                }
    });
}
void WeatherClient::handleNetworkError(QNetworkReply *reply)
{
    switch (reply->error())
    {
    case QNetworkReply::HostNotFoundError:
        qDebug() << "Host not found. Check internet or URL.";
        break;

    case QNetworkReply::TimeoutError:
        qDebug() << "Request timed out.";
        break;

    case QNetworkReply::ConnectionRefusedError:
        qDebug() << "Server refused connection.";
        break;

    default:
        qDebug() << "Other network error:" << reply->errorString();
        break;
    }
}
void WeatherClient::handleHttpError(int statusCode, QNetworkReply *reply)
{
    if (statusCode == 401)
        qDebug() << "Unauthorized - check API key";

    else if (statusCode == 404)
        qDebug() << "Endpoint not found:" << reply->url();

    else if (statusCode >= 500)
        qDebug() << "Server error:" << statusCode;

    else
        qDebug() << "HTTP error:" << statusCode;
}


void WeatherClient::fetchWeather(QString city) {


    if (city.isEmpty()) {
        QMessageBox::information(nullptr, "Input Error", "Please enter a city name.");
        emit enablebtn();

        return;
    }

    if (apiKey == "YOUR_API_KEY_HERE") {

        emit enablebtn();
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

        QVector<ForecastEntry> forecast = parseDataForecast(response);

        for (const ForecastEntry &f : forecast) {
            qDebug() << f.dt_txt << f.Weathermain.temp << f.weather.description;
        }
        QString output;

        for (const ForecastEntry &f : forecast) {
            output += QString("Time: %1 | Temp: %2°C | Description: %3\n")
                        .arg(f.dt_txt)
                        .arg(f.Weathermain.temp)
                        .arg(f.weather.description);
        }
        qDebug() <<"====================================" ;
        qDebug() <<"Forecast Data:" <<  output;   // Print the full string
        qDebug() <<"====================================" ;

    }

    reply->deleteLater();
}
void WeatherClient::onAQIDataReceived(QNetworkReply *reply)
{

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

    if (!jsonDoc.isObject()) {
        reply->deleteLater();
        qInfo()<<"empty json aqi";
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
    qDebug() << "==============Current weather Data=============";
    qDebug() << "City:" << wd.city << wd.country;
    qDebug() << "Temp:" << wd.temp << "Feels Like:" << wd.feelsLike;
    qDebug() << "Weather:" << wd.Weathermain << "(" << wd.description << ")";
    qDebug() << "==============Current weather Data=============";
    emit WeatherCurrentDataReady(wd);
}

QVector<ForecastEntry> WeatherClient::parseDataForecast(const QByteArray &data)
{
    QVector<ForecastEntry> result;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject())
        return result;

    QJsonObject root = doc.object();

    QJsonArray listArray = root.value("list").toArray();


    for (const QJsonValue &v : listArray) {
        ForecastEntry entry;
        QJsonObject obj = v.toObject();

        entry.dt = obj.value("dt").toVariant().toLongLong();
        entry.dt_txt = obj.value("dt_txt").toString();
       // qInfo()<<entry.dt_txt;
        entry.visibility = obj.value("visibility").toInt();

        // ----- MAIN -----
        QJsonObject m = obj["main"].toObject();
        entry.Weathermain.temp        = m.value("temp").toDouble();
        entry.Weathermain.feels_like  = m.value("feels_like").toDouble();
        entry.Weathermain.temp_min    = m.value("temp_min").toDouble();
        entry.Weathermain.temp_max    = m.value("temp_max").toDouble();
        entry.Weathermain.pressure    = m.value("pressure").toInt();
        entry.Weathermain.humidity    = m.value("humidity").toInt();
        entry.Weathermain.sea_level   = m.value("sea_level").toInt();
        entry.Weathermain.grnd_level  = m.value("grnd_level").toInt();

        // ----- WEATHER -----
        QJsonArray wArr = obj["weather"].toArray();
        if (!wArr.isEmpty()) {
            QJsonObject w = wArr.at(0).toObject();
            entry.weather.id          = w.value("id").toInt();
            entry.weather.Weathermain        = w.value("main").toString();
            entry.weather.description = w.value("description").toString();
            entry.weather.icon        = w.value("icon").toString();
        }

        // ----- WIND -----
        QJsonObject wn = obj["wind"].toObject();
        entry.wind.speed = wn.value("speed").toDouble();
        entry.wind.deg   = wn.value("deg").toInt();
        entry.wind.gust  = wn.value("gust").toDouble();  // may be missing

        // ----- CLOUDS -----
        entry.clouds.all = obj["clouds"].toObject().value("all").toInt();

        // ----- SYS -----
        QJsonObject s = obj["sys"].toObject();
        entry.sys.country = s.value("country").toString();
        entry.sys.sunrise = s.value("sunrise").toVariant().toLongLong();
        entry.sys.sunset  = s.value("sunset").toVariant().toLongLong();

        result.append(entry);
    }
    emit ForecastDataReady(result);
    return result;
}
