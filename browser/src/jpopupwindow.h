#pragma once

#include <QMainWindow>
#include "jimberx264webview.h"
#include <QWebEnginePage>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE

class JimberX264WebView;

class JPopupWindow : public QWidget {
Q_OBJECT

public:
  JPopupWindow(QWebEngineProfile *profile, int parentWinId, QWidget* parent = nullptr) ;
  void closeEvent(QCloseEvent *event) override;
  QWebEnginePage *page() const;
  virtual ~JPopupWindow(){ qInfo() << "Popup window destroyed!!!"; }
  
private:
  JimberX264WebView m_webview;
  QWebEnginePage m_page;
  QVBoxLayout m_layout;

private Q_SLOTS:
  void adjustGeometry(const QRect &newGeometry);

protected:
  void resizeEvent(QResizeEvent *event) override;

};

QT_END_NAMESPACE
