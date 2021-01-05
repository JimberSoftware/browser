#include <QUrl>
#include <QApplication>
#include <QCoreApplication>
#include "jimberx264browser.h"
#include <QWebEngineProfile>
#include "jxwindowcontroller.h"

// inline void setupProfile(QWebEngineProfile *profile, const DownloadManager &manager)
// {
//     QWebEngineProfile::defaultProfile()->setPersistentCookiesPolicy(
//         QWebEngineProfile::PersistentCookiesPolicy::AllowPersistentCookies);
//     if (QWebEngineProfile::defaultProfile()->httpAcceptLanguage().isEmpty())
//     {
//         QWebEngineProfile::defaultProfile()->setHttpAcceptLanguage("en-US,en;q=0.9");
//     }
//     QObject::connect(profile, &QWebEngineProfile::downloadRequested, &manager, &DownloadManager::downloadRequested);
// }

int main(int argc, char **argv)
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

    
    QApplication app(argc, argv);
    app.setStyleSheet(
        "QToolTip { color: #000000; background-color: #ffffff; border-width: 1px; font-family: ubuntu, sans-serif; padding: 1px; font-weight:300; font-size:16px;}");

    //IamLoginInformation(parser.value(enableIamOption), parser.value(enableIamUser)); //@todo check if user is provided otherwise error
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    QPACommunication::instance(); // init QPA QPACommunication &instance = 
    return QApplication::exec();
}