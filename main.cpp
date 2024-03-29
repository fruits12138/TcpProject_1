#include "tcpclient.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
//#include "friend.h"
//#include "opewidget.h"
//#include <online.h>
//#include <sharefile.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TcpClient_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

//    Friend fr;
//    fr.show();


//    OpeWidget opeWidget;
//    opeWidget.show();

//    Online w;
//    w.show();
//    TcpClient w;
//    w.show();
//    ShareFile file;
//    file.show();
    TcpClient::getInstance().show();

    return a.exec();
}
