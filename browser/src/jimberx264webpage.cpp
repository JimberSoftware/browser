#include <QBuffer>
#include <QCoreApplication>
#include <QThread>
#include <QWebEngineProfile>
#include <QWebEngineCertificateError>

#include <functional>
// #include "socketcommunication.h"
#include "JCommandHandler.h"
#include "jimberx264browser.h"
#include "jimberx264webpage.h"
#include "jpopupwindow.h"
#include "jxwindowcontroller.h"

typedef OutgoingCommand OC;

JimberX264WebPage::JimberX264WebPage(QWebEngineProfile *profile, QWidget *parent) : QWebEnginePage(profile, parent),
                                                        m_confirmClicked(false), m_confirmValue(false)
{
  using std::placeholders::_1;
  std::function<void(std::unique_ptr<QWebEngineNotification> notification)>
      onNotification = std::bind(&JimberX264WebPage::onNotification, this, _1);
  profile->setNotificationPresenter(onNotification);



};


bool JimberX264WebPage::certificateError(const QWebEngineCertificateError &certificateError){
  Q_UNUSED(certificateError);
  return false;
}

int JimberX264WebPage::getParentId()
{
  return reinterpret_cast<JimberX264Browser *>(parent()->parent())->winId();
}

void JimberX264WebPage::onNotification(
    std::unique_ptr<QWebEngineNotification> notification)
{
  QImage image = notification->icon();
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  image.save(&buffer, "PNG");
  QString iconBase64 = QString::fromLatin1(byteArray.toBase64().data());
  QString origin = notification->origin().toEncoded();
  JCommand(OC::NOTIFICATION, getParentId())
      << notification->title() << origin << notification->message()
      << iconBase64;
}

bool JimberX264WebPage::acceptNavigationRequest(
    const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
  return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

QWebEnginePage *
JimberX264WebPage::createWindow(QWebEnginePage::WebWindowType type)
{
  // can check if tab
  qInfo() << "[JimberX264WebPage::createWindow] window type " << type;
  if (type == WebDialog)
  {
    qInfo() << "Popup";

    auto popup = JPopupWindowPtr::create(this->profile(), this->getParentId());
    popup->setAttribute(Qt::WA_DeleteOnClose);
    popup->show();
    JxWindowController::instance().addPopupWindow(popup);
    // QObject::connect(popup->page(), &QObject::destroyed, [](QObject *obj) {
    //   qDebug() << obj << "PAGE was destroyed";
    // });

    return popup->page();
  }
  else // new tab
  {
    qInfo() << "opening new tab";
    auto window = JTabWindowPtr::create();
    qInfo() << "open" << window->winId();
    window->setAttribute(Qt::WA_DeleteOnClose);
    JxWindowController::instance().addTabWindow(window);
    JCommand(OC::WINDOWOPENED, getParentId()) << window->winId();
    window->show();
    window->setFocus();
    window->raise();
    window->activateWindow();
    return window->webview()->page();
  }
}

void JimberX264WebPage::triggerAction(WebAction action, bool b)
{
  return QWebEnginePage::triggerAction(action, b);
}

UploadPtr JimberX264WebPage::getUpload() { return m_currentUpload; }

QStringList
JimberX264WebPage::chooseFiles(FileSelectionMode mode,
                               const QStringList &,
                               const QStringList &acceptedMimeTypes)
{

  m_currentUpload = UploadPtr::create();
  int parentId = getParentId();
  bool multiple = (mode == QWebEnginePage::FileSelectOpenMultiple);
  JCommand(OC::FILEUPLOAD, parentId)
      << multiple << acceptedMimeTypes.join(", ");
  while (!m_currentUpload->isFinished() && !m_currentUpload->isCancelled())
  {
    QCoreApplication::processEvents();
    QThread::sleep(0.1);
  }

  QStringList files;
  if (m_currentUpload->isCancelled())
  {
    return QStringList(files);
  }

  JCommand(OC::FILEUPLOADFINISHED, parentId);
  files = m_currentUpload->getFiles();
  m_currentUpload = nullptr;
  return QStringList(files);
};

void JimberX264WebPage::javaScriptAlert(const QUrl &, // securityOrigin
                                        const QString &msg)
{
  JCommand(OC::ALERT, getParentId()) << msg.toUtf8().toBase64();
}

bool JimberX264WebPage::javaScriptConfirm(const QUrl &, // securityOrigin
                                          const QString &msg)
{
  JCommand(OC::CONFIRM, getParentId()) << msg.toUtf8().toBase64();

  while (!this->m_confirmClicked)
  {
    QCoreApplication::processEvents();
    QThread::sleep(0.1);
  }

  this->m_confirmClicked = false;
  return this->m_confirmValue;
}

void JimberX264WebPage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID){

  // qInfo() << level  << " the Message: " << message << " Linenr: " << lineNumber << " srcId " << sourceID;
}

void JimberX264WebPage::confirmDialog(bool value)
{
  this->m_confirmClicked = true;
  this->m_confirmValue = value;
}