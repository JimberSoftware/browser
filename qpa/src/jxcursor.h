#ifndef JIMBERX264CURSOR_H
#define JIMBERX264CURSOR_H

#include <QtGui/qpa/qplatformcursor.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

class JxWsServer;

class JxCursor : public QPlatformCursor
{
  public:
    JxCursor();

    void changeCursor(QCursor *windowCursor, QWindow *window) override;

  private:
    bool first;
};

QT_END_NAMESPACE

#endif