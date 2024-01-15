#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Noter_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    ////if want to add stylesheet .qss
    // QFile stylesheetFile("C:/Users/saura/OneDrive/Documents/Noter/Medize.qss");//don't know why relative path dont work
    // stylesheetFile.open(QFile::ReadOnly);
    // QString styleSheet =QLatin1String(stylesheetFile.readAll());
    // a.setStyleSheet(styleSheet);

    MainWindow w;
    w.show();
    return a.exec();
}
