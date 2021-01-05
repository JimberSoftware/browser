#ifndef JIMBERX264WEBVIEW_H
#define JIMBERX264WEBVIEW_H

#include <QWebEngineView>
#include <QWebEngineFullScreenRequest>
#include <QWebEngineSettings>
#include <QTouchEvent>
#include <QGestureEvent>
#include <QCommandLineParser>

#include "jimberx264webpage.h"
#include "socketcommunication.h"
#define OVERRIDEIFCUSTOMWEBENGINE override
QT_BEGIN_NAMESPACE

class JimberX264WebView : public QWebEngineView {
Q_OBJECT
public:
  explicit JimberX264WebView(QWebEngineProfile* profile = Q_NULLPTR, QWidget *parent = Q_NULLPTR);
  ~JimberX264WebView() override = default;
  void contextMenuEvent(QContextMenuEvent *event) override;

  void setTouchHandleVisibility(int orientation, bool visible) OVERRIDEIFCUSTOMWEBENGINE;
  void setTouchHandleOpacity(int orientation, float opacity) OVERRIDEIFCUSTOMWEBENGINE;
  void setTouchHandleBounds(int orientation, const QRect &rect) OVERRIDEIFCUSTOMWEBENGINE;
  void showTouchSelectionMenu(bool cut, bool copy, bool paste, const QRect &rect, const QSize &size) OVERRIDEIFCUSTOMWEBENGINE;
  void hideTouchSelectionMenu() OVERRIDEIFCUSTOMWEBENGINE;
  // virtual QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);

//    void grabGestures(const QVector<Qt::GestureType> &gestures);
  bool event(QEvent *) ;
public slots:
  void fullScreenRequest(QWebEngineFullScreenRequest request);

private:
  int getParentId();
  JimberX264WebPage m_page;
};

QT_END_NAMESPACE

#endif