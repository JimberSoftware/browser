#include <QTimer>
#include <QIcon>
#include <QBuffer>
#include <QWebEngineDownloadItem>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QInputMethodQueryEvent>
#include <QWebEngineProfile>
#include <QClipboard>

#include "jimberx264browser.h"
#include "jimberx264webview.h"
#include "jimberx264webpage.h"
#include "JCommandHandler.h"
#include "jxfiledownload.h"
#include "jxwindowcontroller.h"

#define ZOOM 1

bool JimberX264Browser::isMobile = 0;

JimberX264Browser::JimberX264Browser(QWidget *parent, DownloadManager *downloadManager, QWebEngineProfile* profile)
    : QMainWindow(parent),
      first(true), previousSite(false), m_downloadManager(downloadManager), m_webEngineView(profile, this)
{
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    this->resize(800, 600);
    // m_webEngineView.page()
    // m_webEngineView.page()->profile()->setHttpAcceptLanguage(language);

    m_webEngineView.settings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    setCentralWidget(&m_webEngineView);
    m_webEngineView.page()->profile()->clearHttpCache();
    m_webEngineView.resize(800, 600);
    // qsrand(time(0))
    // webEngineView->resize(1, 1);
    // this->webEngineView->load("chrome://gpu");
    //-vf unsharp=3:3:0.6:5:5:0.0
    //connect(wss, &JxWsServer::textMessageReceived, this, &JimberX264Browser::onMessage);

    connect(&m_webEngineView, &QWebEngineView::loadStarted, this, &JimberX264Browser::loadStarted);
    connect(&m_webEngineView, &QWebEngineView::loadProgress, this, &JimberX264Browser::loadProgress);
    connect(&m_webEngineView, &QWebEngineView::loadFinished, this, &JimberX264Browser::loadFinished);
    connect(&m_webEngineView, &QWebEngineView::urlChanged, this, &JimberX264Browser::urlChanged);
    connect(&m_webEngineView, &QWebEngineView::titleChanged, this, &JimberX264Browser::titleChanged);
    connect(&m_webEngineView, &QWebEngineView::selectionChanged, this, &JimberX264Browser::selectionChanged);
    connect(&m_webEngineView, &QWebEngineView::iconChanged, this, &JimberX264Browser::iconChanged);

    QPACommunication &instance = QPACommunication::instance();
    connect(&instance, &QPACommunication::textMessageReceived, this, &JimberX264Browser::onMessage);
    connect(&instance, &QPACommunication::binaryMessageReceived, this, &JimberX264Browser::onBinaryMessage);

    connect(m_webEngineView.page(), &QWebEnginePage::pdfPrintingFinished, this,
            &JimberX264Browser::pdfPrintingFinished);
    connect(m_webEngineView.page(), &QWebEnginePage::featurePermissionRequested, this,
            &JimberX264Browser::featurePermissionRequested);

    // QString userAgent("Mozilla/5.0 (X11; Linux x86_64; rv:75.0) Gecko/20100101 Firefox/75.0");
    // setUserAgent(userAgent);

    connect(m_webEngineView.page(), &QWebEnginePage::windowCloseRequested,
            this, &QWidget::close);
}

JimberX264Browser::~JimberX264Browser() {}

QWebEngineView *JimberX264Browser::webview()
{
    return &(this->m_webEngineView);
}

void JimberX264Browser::selectionChanged()
{
    JCommand(OC::SELECTION, this->winId()) << m_webEngineView.page()->selectedText().toUtf8().toBase64();
}

void JimberX264Browser::setUserAgent(const QString &userAgent)
{
    m_webEngineView.page()->profile()->setHttpUserAgent(userAgent);
    //webEngineView->page()->profile()->setHttpUserAgent(QString("Mozilla/5.0 (Linux; Android 7.1.2; Redmi5 Plus) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.90 Mobile Safari/537.36"));
}

void JimberX264Browser::loadFinished(bool)
{
}

void JimberX264Browser::urlChanged(const QUrl &url)
{
    JCommand(OC::URL, this->winId()) << url.toDisplayString();
}

void JimberX264Browser::titleChanged(const QString &title)
{

    JCommand(OC::TITLE, this->winId()) << title;
    // SocketCommunication::instance().sendToAllClients(OC::TITLE + ';' + title);
}

void JimberX264Browser::iconChanged(const QIcon &icon)
{
    QImage image(icon.pixmap(20, 20).toImage());
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    image.save(&buffer, "PNG");
    QString iconBase64 = QString::fromLatin1(byteArray.toBase64().data());
    //  SocketCommunication::instance().sendToAllClients("i;" + iconBase64);
    JCommand(OC::ICON, this->winId()) << iconBase64;
}

void JimberX264Browser::reload()
{
    m_webEngineView.page()->triggerAction(QWebEnginePage::Reload);
}

void JimberX264Browser::forward()
{
    m_webEngineView.page()->triggerAction(QWebEnginePage::Forward);
}

void JimberX264Browser::back()
{
    m_webEngineView.page()->triggerAction(QWebEnginePage::Back);
}

void JimberX264Browser::copyImage()
{
    m_webEngineView.page()->triggerAction(QWebEnginePage::CopyImageToClipboard);
}

void JimberX264Browser::loadStarted()
{
    JCommand(OC::PAGELOADINGSTARTED, this->winId());
}

void JimberX264Browser::search(const QString &searchTerm)
{
    m_webEngineView.page()->findText(searchTerm);
}

bool JimberX264Browser::event(QEvent *event)
{
    return QMainWindow::event(event);
}

void JimberX264Browser::loadProgress(int progress)
{
    if (progress < 100)
    {
        return;
    }
    JCommand(OC::PAGELOADINGFINISHED, this->winId());
}

void JimberX264Browser::load(const QUrl &url)
{
    // qInfo() << "[JimberX264Browser::load]";
    previousSite = true;
    QUrl withHttp = url;
    if (!url.toString().mid(0, 4).contains("http") && url.toString() != "about:blank")
    {
        withHttp = QUrl(QString("http://") + url.toString()); // add http in front if url does not have it
    }
    //On refresh sometimes we had no view, this forces a red page and back to the first url. Force reload of ffmpeg.
    // JxLog() << "Loading!" << (withHttp.toString()).toStdString();
    this->m_webEngineView.load(withHttp);

    // qInfo() << "[JimberX264Browser::load] done";
    // this->webEngineView->load(QString("chrome://m_downloads"));
    // this->webEngineView->load(QString("chrome://gpu"));

    //webEngineView->page()->runJavaScript("var head = document.getElementsByTagName('head')[0];var meta = document.createElement('meta');meta.setAttribute('charset', 'utf-8');head.appendChild(meta);alert('appended');");
}

void JimberX264Browser::handleCopyEvent(JIncomingCommand &)
{
    QGuiApplication::clipboard()->setText(m_webEngineView.page()->selectedText());
}

void JimberX264Browser::onMessage(QString message, WId winId)
{
    if (winId != this->winId())
    {
        return;
    }
    auto jxCommand = JIncomingCommand(message);

    this->activateWindow(); // Set this to change the 'main' focus
    switch (jxCommand.getCommand())
    {
    case IncomingCommand::RESIZE:
        handleResize(jxCommand);
        break;
    case IncomingCommand::CHANGEZOOM:
        handleZoom(jxCommand);
        break;
    case IncomingCommand::PRINT:
        print();
        break;
    case IncomingCommand::CHANGEURL:
        load(QUrl(jxCommand.arg(1)));
        break;
    case IncomingCommand::FILEDOWNLOADPAUSE:
        m_downloadManager->downloadPause(jxCommand.arg(1).toInt());
        break;
    case IncomingCommand::FILEDOWNLOADRESUME:
        m_downloadManager->downloadResume(jxCommand.arg(1).toInt());
        break;
    case IncomingCommand::FILEDOWNLOADCANCEL:
        m_downloadManager->downloadCancel(jxCommand.arg(1).toInt());
        break;
    case IncomingCommand::INIT:
        handleInit(jxCommand);
        break;
    case IncomingCommand::RELOAD:
        reload();
        break;
    case IncomingCommand::BACK:
        back();
        break;
    case IncomingCommand::FORWARD:
        forward();
        break;
    case IncomingCommand::FILEUPLOADSTART:
        uploadStarted(jxCommand);
        break;
    case IncomingCommand::FILEUPLOADCANCEL:
        page()->getUpload()->cancel();
        break;
    case IncomingCommand::GRANTPERMISSION:
        grantPermission(jxCommand);
        break;
    case IncomingCommand::DENYPERMISSION:
        denyPermission(jxCommand);
        break;
    case IncomingCommand::MOUSEWHEELEVENT:
        handleMouseWheel(jxCommand);
        break;
    case IncomingCommand::COPYEVENT:
        handleCopyEvent(jxCommand);
        break;
    case IncomingCommand::WINDOWCLOSED:
        handleWindowCloseEvent();
        break;
    case IncomingCommand::COPYIMAGE:
        copyImage();
        break;
    default:
        // qDebug() << "Some unknown command" << message;
        break;
    }
}

void JimberX264Browser::onBinaryMessage(const QByteArray &message)
{
    if (message.startsWith("f")) // for fileupload
    {
        // is it possible to name a file /etc/passwd or ../../../etc/passwd?
        // passwd is not really a target, but some other stuff might be, maybe
        processUploadCommand(message);
    }
}

void JimberX264Browser::handleMouseWheel(const JCommand &command)
{
    int deltaY = static_cast<int>(command.arg(6).toDouble());
    Qt::KeyboardModifiers modifiers = Qt::KeyboardModifiers(command.arg(7).toInt());

    if (modifiers.testFlag(Qt::ControlModifier))
    {
        if (deltaY < 0)
        { //zoom in
            this->webview()->setZoomFactor(this->webview()->zoomFactor() + 0.25);
        }
        else
        { //zoom out
            this->webview()->setZoomFactor(this->webview()->zoomFactor() - 0.25);
        }
    }
}

void JimberX264Browser::grantPermission(const JCommand &command)
{
    m_webEngineView.page()->setFeaturePermission(QUrl(command.arg(1)), QWebEnginePage::Feature(command.arg(2).toInt()),
                                                 QWebEnginePage::PermissionGrantedByUser);
}

void JimberX264Browser::denyPermission(const JCommand &command)
{
    m_webEngineView.page()->setFeaturePermission(QUrl(command.arg(1)), QWebEnginePage::Feature(command.arg(2).toInt()),
                                                 QWebEnginePage::PermissionDeniedByUser);
}

void JimberX264Browser::handleResize(JCommand command)
{
    return; // wait why? @Johnkarl
    int width = command.arg(1).toInt();
    int height = command.arg(2).toInt();
    this->m_webEngineView.resize(width, height);
    this->resize(width, height);
}

void JimberX264Browser::handleZoom(JCommand command)
{
    float zoom = command.arg(1).toFloat();
    this->m_webEngineView.setZoomFactor(zoom);
}

void JimberX264Browser::uploadStarted(const JCommand &command)
{
    int amountOfFiles = command.arg(1).toInt();
    page()->getUpload()->setAmountOfFiles(amountOfFiles);
}

void JimberX264Browser::processUploadCommand(QByteArray message)
{
    int nameLength = message.at(1);
    QString name(message.mid(2, nameLength));
    QByteArray fileData = message.mid(2 + nameLength);
    page()->getUpload()->addFile(name, fileData);
}

void JimberX264Browser::print()
{
    page()->printToPdf(QString("page.pdf"),
                       QPageLayout(QPageSize(QPageSize::A4), QPageLayout::Portrait,
                                   QMarginsF()));
}

void JimberX264Browser::pdfPrintingFinished(const QString &filepath, bool success)
{
    JxLog() << filepath;
    if (success)
    {
        QFile file(filepath);
        if (!file.open(QIODevice::ReadOnly))
            return;
        QByteArray blob = file.readAll();
        QString b64blob(blob.toBase64());
        JCommand(OutgoingCommand::PDF, this->winId()) << b64blob;
    }
    else
    {
        JxLog() << "Printing PDF failed";
    }
}

void JimberX264Browser::featurePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
{
    JCommand(OC::REQUESTPERMISSION, this->winId()) << securityOrigin.toString() << feature;
}

void JimberX264Browser::handleInit(const JCommand &command)
{
    bool mobile = command.arg(1).toInt();
    JimberX264Browser::isMobile = mobile;
    if (mobile)
    {
        QString userAgent(
            "Mozilla/5.0 (Linux; Android 7.1.2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.90 Mobile Safari/537.36");
        setUserAgent(userAgent);
    }
}

void JimberX264Browser::handleWindowCloseEvent()
{
}

DownloadManager downloadManager;
int createBrowser(QString language)
{
    auto profile = QWebEngineProfile::defaultProfile();
    setupProfile(profile, downloadManager, language);

    auto window = JTabWindowPtr::create(nullptr, &downloadManager, profile);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
    window->setFocus();
    window->setFocus();
    window->raise();
    window->activateWindow();
    JxWindowController::instance().addTabWindow(window);
    return window->winId();
}

void setupProfile(QWebEngineProfile *profile, const DownloadManager &manager, const QString& language)
{
    QWebEngineProfile::defaultProfile()
        ->setPersistentCookiesPolicy(QWebEngineProfile::PersistentCookiesPolicy::AllowPersistentCookies);
    
    if (QWebEngineProfile::defaultProfile()->httpAcceptLanguage().isEmpty())
    {
        if(language.isEmpty()) {
            profile->setHttpAcceptLanguage("en-US,en;q=0.9");
        }else {
            profile->setHttpAcceptLanguage(language); // HTTP header injection?
        }

    }
    QObject::connect(profile, &QWebEngineProfile::downloadRequested, &manager, &DownloadManager::downloadRequested);

    //if dev profile->clearHttpCache();
}