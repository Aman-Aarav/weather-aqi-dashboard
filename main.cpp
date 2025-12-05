#include "mainwindow.h"
#include "weatherwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Weather-AQI-Dashboard");
    a.setWindowIcon(QIcon(":/Images/weather_43796 (2).ico"));
    MainWindow w;
    w.setWindowTitle("Weather-AQI-Dashboard");
    w.showMaximized();

    return a.exec();
}
