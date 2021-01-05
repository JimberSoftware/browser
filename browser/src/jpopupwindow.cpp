#include "jpopupwindow.h"
#include <QCloseEvent>

#include "JCommandHandler.h"
#include "jimberx264browser.h"
#include "jimberx264webview.h"


typedef OutgoingCommand OC;

JPopupWindow::JPopupWindow(QWebEngineProfile *profile, int parentWinId, QWidget *parent )
    : QWidget(parent) ,m_webview(profile, this), m_page(profile, &m_webview)
{
  m_webview.setPage(&m_page);
  m_layout.setMargin(0);
  setLayout(&m_layout);
  m_layout.addWidget(&m_webview);
  this->activateWindow();
  m_webview.setFocus();
  m_webview.show();

  connect(page(), &QWebEnginePage::geometryChangeRequested, this,
          &JPopupWindow::adjustGeometry);
  connect(m_webview.page(), &QWebEnginePage::windowCloseRequested, this,
          &QWidget::close);
  this->resize(500, 500);
  qInfo() << "Opening popupwindow with winid " << this->winId();
  JCommand(OC::POPUPOPENED, parentWinId)
      << QString::number(this->winId()) << this->geometry().x()
      << this->geometry().y() << this->geometry().width()
      << this->geometry().height();
};

QWebEnginePage *JPopupWindow::page() const { return m_webview.page(); }

void JPopupWindow::closeEvent(QCloseEvent *) //event
{
  JCommand(OC::POPUPCLOSED, this->winId());
};

void JPopupWindow::resizeEvent(QResizeEvent *event)
{
  auto newSize = event->size();
  JCommand(OC::POPUPRESIZED, this->winId())
      << newSize.width() << newSize.height();
}

void JPopupWindow::adjustGeometry(const QRect &newGeometry)
{
  const int x1 = frameGeometry().left() - geometry().left();
  const int y1 = frameGeometry().top() - geometry().top();
  const int x2 = frameGeometry().right() - geometry().right();
  const int y2 = frameGeometry().bottom() - geometry().bottom();
  setGeometry(newGeometry.adjusted(x1, y1, x2, y2));
  qInfo() << x1 << " " << y1 << " " << x2 << " " << y2;
  this->m_webview.resize(newGeometry.width(), newGeometry.height());
};

