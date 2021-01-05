#include "jxcursor.h"
#include "jxintegration.h"
#include <QtGui/private/qguiapplication_p.h>
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

JxCursor::JxCursor()
    : QPlatformCursor(), first(true)

{
}

void JxCursor::changeCursor(QCursor *windowCursor, QWindow *window)
{
    if (first) // This makes sure cursor changed works on first website; apparantly a mouse event is needed
    {
        QWindowSystemInterface::handleMouseEvent(window, QPoint(10, 10), QPoint(10, 10),
                                                 Qt::MouseButtons(Qt::NoButton), Qt::NoModifier);
        first = false;
    }
    if (windowCursor == nullptr || window == nullptr)
    {
        return;
    }

    int topLevelId = window->winId();
    if(window->parent()) {
        topLevelId = window->parent()->winId();
    }

    JCommand(OC::CURSOR, topLevelId) << QString::number(windowCursor->shape());
}