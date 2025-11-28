#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>

class WeatherWidget : public QWidget {
    Q_OBJECT
public:
    WeatherWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(576, 292);

        auto *cityLabel = new QLabel("Some City");
        cityLabel->setStyleSheet("color:white; font-size:28px;");

        auto *tempLabel = new QLabel("+20 °C");
        tempLabel->setStyleSheet("color:white; font-size:64px;");

        auto *dayLabel = new QLabel("Monday");
        dayLabel->setStyleSheet("color:white; font-size:24px;");

        auto *icon = new QLabel;
        icon->setPixmap(QPixmap(":/icons/sun_cloud.png").scaled(200,200, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        auto *humidity = new QLabel("💧 50%");
        humidity->setStyleSheet("color:white; font-size:18px;");

        auto *wind = new QLabel("🌬️ East, 20 km/h");
        wind->setStyleSheet("color:white; font-size:18px;");

        auto *pressure = new QLabel("🔘 1010 hPa");
        pressure->setStyleSheet("color:white; font-size:18px;");

        auto *detailsLayout = new QVBoxLayout;
        detailsLayout->addWidget(dayLabel);
        detailsLayout->addWidget(humidity);
        detailsLayout->addWidget(wind);
        detailsLayout->addWidget(pressure);

        auto *mainLayout = new QHBoxLayout;
        mainLayout->addLayout(detailsLayout);
        mainLayout->addWidget(icon);
        mainLayout->addWidget(tempLabel);

        setLayout(mainLayout);
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        QLinearGradient grad(rect().topLeft(), rect().bottomRight());
        grad.setColorAt(0, QColor("#5cb0f3"));
        grad.setColorAt(1, QColor("#2a6fb8"));
        p.fillRect(rect(), grad);
    }
};
