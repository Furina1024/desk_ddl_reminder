#include "desktoppet.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDialog>
#include <QMouseEvent>
#include <QMenu>
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "untitled_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    DesktopPet p;
    p.show();
    return a.exec();
}
