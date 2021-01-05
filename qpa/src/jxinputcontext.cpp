#include <QtCore/qdebug.h>

#include "jxinputcontext.h"
#include "JCommandHandler.h"
#include "jxwsserver.h"
#include <QGuiApplication>
#include <QRectF>
#include <QInputMethodQueryEvent>
#include <QWindow>
JxInputContext::JxInputContext() : QPlatformInputContext() {
}

void JxInputContext::showInputPanel()
{
    this->visible = true;
    JCommand(OC::SHOWINPUT, QGuiApplication::focusWindow()->winId());
    emitInputPanelVisibleChanged();
}
void JxInputContext::hideInputPanel()
{
    this->visible = false;
    JCommand(OC::HIDEINPUT, QGuiApplication::focusWindow()->winId());
    emitInputPanelVisibleChanged();
}

bool JxInputContext::isInputPanelVisible() const {
    return this->visible;
}