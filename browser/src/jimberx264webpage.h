#ifndef JIMBERX264WEBPAGE_H
#define JIMBERX264WEBPAGE_H
#include <QWebEnginePage>
#include "jxfileupload.h"
#include <QMainWindow>
#include <memory>
#include <QWebEngineNotification>


QT_BEGIN_NAMESPACE

typedef QSharedPointer<Upload> UploadPtr;

class JimberX264WebPage : public QWebEnginePage
{

  Q_OBJECT


public:
  JimberX264WebPage(QWebEngineProfile *profile = Q_NULLPTR, QWidget *parent = Q_NULLPTR);
  virtual ~JimberX264WebPage(){};
  void confirmDialog(bool value);
  void onNotification(std::unique_ptr<QWebEngineNotification> notification);
  bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;
  void triggerAction(WebAction action, bool b) override;
  QStringList chooseFiles(FileSelectionMode mode, const QStringList &oldFiles, const QStringList &acceptedMimeTypes) override;
  void javaScriptAlert(const QUrl &securityOrigin, const QString &msg) override;
  bool javaScriptConfirm(const QUrl &securityOrigin, const QString &msg) override;
  QWebEnginePage* createWindow(QWebEnginePage::WebWindowType type) override;
  int getParentId();
  UploadPtr getUpload();
  bool certificateError(const QWebEngineCertificateError &certificateError) override;
  void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override;
private:
  bool m_confirmClicked;
  bool m_confirmValue;
  UploadPtr m_currentUpload;

};

QT_END_NAMESPACE
#endif