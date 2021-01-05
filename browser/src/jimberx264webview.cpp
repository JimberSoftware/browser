#include <QTimer>
#include "jimberx264webview.h"

#include <QContextMenuEvent>
#include "JCommandHandler.h"
#include <QWebEngineContextMenuData>
#include <QWebEngineHistory>
#include <QToolTip>
#include "jimberx264browser.h"

typedef OutgoingCommand OC;

JimberX264WebView::JimberX264WebView(QWebEngineProfile* profile, QWidget *parent) : QWebEngineView(parent), m_page(profile, this) {
    this->setAttribute(Qt::WA_AlwaysShowToolTips, true);
    this->setPage(&m_page);
    this->setAttribute(Qt::WA_AcceptTouchEvents, true);
    this->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    this->window()->setWindowState(this->window()->windowState() ^ Qt::WindowFullScreen);
    connect(this->page(), &QWebEnginePage::fullScreenRequested, this, &JimberX264WebView::fullScreenRequest);
    this->winId(); //Do this to create a platform window
}

int JimberX264WebView::getParentId() {
    return reinterpret_cast<JimberX264Browser *>(parent())->winId();
}

//void JimberX264WebView::grabGestures(const QVector<Qt::GestureType> &gestures)
//{
//    qInfo() << "[JimberX264WebView::grabGestures]";
//    //! [enable gestures]
//    for (Qt::GestureType gesture : gestures)
//        grabGesture(gesture);
//    //! [enable gestures]
//}

void JimberX264WebView::fullScreenRequest(QWebEngineFullScreenRequest request) {
    request.accept();
}

void JimberX264WebView::showTouchSelectionMenu(bool cut, bool copy, bool paste, const QRect &rect, const QSize &) {
    JCommand(OC::SHOWSELECTIONMENU, getParentId()) << cut << copy << paste << rect.x() << rect.y() << rect.width()
                                                   << rect.height();
}

void JimberX264WebView::hideTouchSelectionMenu() {
    JCommand(OC::HIDESELECTIONMENU, getParentId());
}

void JimberX264WebView::setTouchHandleBounds(int orientation, const QRect &rect) {
    JCommand(OC::SETTOUCHHANDLEBOUNDS, getParentId()) << orientation << rect.x() << rect.y() << rect.width()
                                                      << rect.height();
};

void JimberX264WebView::setTouchHandleVisibility(int orientation, bool visible) {
    JCommand(OC::SETTOUCHHANDLEVISIBILITY, getParentId()) << orientation << visible;
};

void JimberX264WebView::setTouchHandleOpacity(int orientation, float opacity) {
    JCommand(OC::SETTOUCHHANDLEOPACITY, getParentId()) << orientation << opacity;
};

bool JimberX264WebView::event(QEvent *ev) {
    if (ev->type() == QEvent::ContextMenu) {
        if (contextMenuPolicy() == Qt::NoContextMenu) {
            // We forward the contextMenu event to the parent widget
            ev->ignore();
            return false;
        }
        // We swallow spontaneous contextMenu events and synthesize those back later on when we get the
        // HandleContextMenu callback from chromium
        ev->accept();
        return true;
    }
    if (ev->type() == QEvent::ToolTip) {
        if (!toolTip().isEmpty()) {
            QToolTip::showText(dynamic_cast<QHelpEvent *>(ev)->pos(), toolTip(), this,
                               QRect(),
                               toolTipDuration()); //Originally they use globalPos(), we use pos(). I don't know why it's different (yet)
        } else {
            // qInfo() << "HIDE";
            QToolTip::hideText();
        }
        ev->accept();
        return true;
    }
    return QWebEngineView::event(ev);
}

void JimberX264WebView::contextMenuEvent(QContextMenuEvent *event) {
    QWebEngineContextMenuData data = page()->contextMenuData();
    QWebEngineContextMenuData::EditFlags editFlags = data.editFlags();
    JCommand(OC::SHOWCONTEXTMENU, getParentId()) << event->globalX() << event->globalY() << editFlags
                                                 << data.isContentEditable() << page()->history()->canGoBack()
                                                 << page()->history()->canGoForward() << data.linkUrl().toString()
                                                 << data.mediaUrl().toString() << data.mediaType();
}
