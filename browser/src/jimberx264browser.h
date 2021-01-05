#include <QWebEngineSettings>
#include <QWebEngineView>
#include <QWebEngineCookieStore>
#include <QMap>
#include <QPrinter>
#include <QApplication>

#include "jxfiledownload.h"
#include "socketcommunication.h"

#include "JCommandHandler.h"
#include <QMainWindow>
#include "jimberx264webview.h"

class JimberX264Browser : public QMainWindow
{

private:
  bool first;
  bool previousSite;
  DownloadManager *m_downloadManager;
  JimberX264WebView m_webEngineView;
  QNetworkAccessManager manager;

  void load(const QUrl &url);
  void search(const QString &searchTerm);
  void handleResize(JCommand command);
  void handleZoom(JCommand command);
  void setUserAgent(const QString &userAgent);
  void grantPermission(const JCommand &command);
  void denyPermission(const JCommand &command);
  void uploadStarted(const JCommand &);
  void handleMouseWheel(const JCommand &command);
  void handleWindowCloseEvent();

  void reload();
  void forward();
  void back();
  void copyImage();
  void processUploadCommand(QByteArray data);
  void print();
  JimberX264WebPage *page()
  {
    return reinterpret_cast<JimberX264WebPage *>(m_webEngineView.page());
  }

public:
  static bool isMobile;
  QWebEngineView *webview();
  JimberX264Browser(QWidget *parent = Q_NULLPTR, DownloadManager *manager = Q_NULLPTR, QWebEngineProfile *profile = Q_NULLPTR);
  ~JimberX264Browser() override;
  void onBinaryMessage(const QByteArray &message);
  void handleCopyEvent(JIncomingCommand &command);
  void runJs(const QString &script)
  {
    m_webEngineView.page()->runJavaScript(script);
  }
private slots:

public slots:
  bool event(QEvent *event) override;
  void onMessage(QString message, WId winId);
  void urlChanged(const QUrl &url);
  void titleChanged(const QString &title);
  void iconChanged(const QIcon &icon);
  void selectionChanged();

  void loadFinished(bool ok);
  void loadStarted();
  void loadProgress(int progress);
  void pdfPrintingFinished(const QString &filePath, bool success);
  void featurePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature);
  void handleInit(const JCommand &command);
};

typedef QSharedPointer<JimberX264Browser> JTabWindowPtr;

int createBrowser(QString language);
void setupProfile(QWebEngineProfile *, const DownloadManager &, const QString &);