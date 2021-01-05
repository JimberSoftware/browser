#include "jxbackingstore.h"

#include "qscreen.h"
#include <QtCore/qdebug.h>
#include <qpa/qplatformscreen.h>
#include <private/qguiapplication_p.h>
#include "jxwindow.h"

#include <QBuffer>

#include <QPainter>
#include <QBackingStore>
#define EXTRAFLUSH 70

QT_BEGIN_NAMESPACE

JxBackingStore::JxBackingStore(QWindow *window)
    : QPlatformBackingStore(window)
{
    _window = window;

    QWindow *parent = QGuiApplication::topLevelAt(QPoint(window->x() + 5, window->y() + 5));
    //I don't know of any other way to get the top window of this Widget. It has no "parent"...

    if (window->objectName() == "QMainWindowClassWindow" || window->objectName() == "JPopupWindowClassWindow")
    {
        // qInfo() << "Creating streamer for " << window->objectName() << " winid is " << window->winId();
        _streamer = JxStreamerPtr::create(window);
        JxBackingStore::streamers().insert(window->winId(), _streamer);
    }
    else
    {
        if (streamers().contains(parent->winId()))
        {
            _streamer = JxStreamerPtr(streamers().value(parent->winId()));
        }
    }
}

JxBackingStore::~JxBackingStore()
{
    int winId = _window->winId();
    if (streamers().contains(winId))
    {
        //delete streamers().value(winId);
        streamers().remove(winId);
    }
}

QPaintDevice *JxBackingStore::paintDevice()
{
    return &_image;
}

void JxBackingStore::flush(QWindow *window, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(offset);
    Q_UNUSED(window);

    if (_window->objectName() == "JimberX264WebViewClassWindow")
        return; // gray surface, something we don't understand yet
    if (_window->objectName().isEmpty())
        return; // draggable stuff, something we don't understand yet

    _streamer->composite(_window, region, _image.copy());
    _streamer->flush();
}

void JxBackingStore::resize(const QSize &size, const QRegion &staticContents)
{
    Q_UNUSED(staticContents);
    QImage::Format format = QGuiApplication::primaryScreen()->handle()->format();
    if (_image.size() != size)
    {
        auto width = size.width();
        auto height = size.height();
        int mod = width % 4;
        if (mod == 1)
        {
            width -= 1;
        }
        else if (mod == 2)
        {

            width += 2;
        }
        else if (mod == 3)
        {
            width += 1;
        }
        if (height % 2 != 0)
        {
            height++;
        }

        if (_window->objectName() == "QMainWindowClassWindow" || _window->objectName() == "JPopupWindowClassWindow")
        {
            _streamer->resize(QSize(width, height));
            _image = QImage(QSize(width, height), format);
        }
        else
        {   
            _image = QImage(QSize(size.width(), size.height()), format);
        }
    }
}
QT_END_NAMESPACE