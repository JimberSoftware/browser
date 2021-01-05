#include "JStreamer.h"

#include "qscreen.h"
#include <QtCore/qdebug.h>
#include <qpa/qplatformscreen.h>
#include <private/qguiapplication_p.h>
#include "../jxwindow.h"
#include "closeeventfilter.h"
#include <QBuffer>
#include <QPainter>
#include <iostream> //@todo remove
#define EXTRAFLUSH 70
//#define TARGET "tcp://127.0.0.1:8000 &"
#define TARGET "nalfile.nal"
QT_BEGIN_NAMESPACE

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

JxStreamer::JxStreamer(QWindow *window) : firstFlush(true), _encoder(EncoderPtr::create(window, 800, 600, 23, TARGET)), m_crf(23), m_width(800), m_height(600), m_devicePixelRatio(1)
{
    _window = window;
    lastFlush.start();

    connect(_encoder.data(), &Encoder::DataWritten, &JxWsServer::instance(), &JxWsServer::sendVideo);
}

QString randString(int len)
{
    QString str;
    str.resize(len);
    for (int s = 0; s < len; ++s)
        str[s] = QChar('A' + char(qrand() % ('Z' - 'A')));

    return str;
}

void JxStreamer::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    if (_restart)
    {
        _restart = false;
    }
}
void JxStreamer::readyReadStandardOutput()
{
    qInfo() << _ffmpeg.readAllStandardOutput();
}

void JxStreamer::readyReadStandardError()
{
    qInfo() << _ffmpeg.readAllStandardError();
}
JxStreamer::~JxStreamer()
{
    qInfo() << "Streamer destroyed " << this;
}

void JxStreamer::flush()
{
    if (__ffimage.height() > 0)
    {
        // qInfo() << "Flushing";
        _encoder->write(_encoder->convertRgbTYuv(__ffimage.bits()));
    }
}

void JxStreamer::composite(QWindow *window, const QRegion &region, const QImage &image)
{
    Q_UNUSED(region);

    // qInfo() << _window->objectName() << window->objectName();

    if (window->objectName() == "QMainWindowClassWindow" || window->objectName() == "JPopupWindowClassWindow")
    {
        _lastBrowserImage = image.copy();
    }
    else
    {
        _windows.insert(window, image.copy());
    }

    __ffimage = _lastBrowserImage.copy();
    QPainter painter;
    painter.begin(&__ffimage);

    for (int i = 0; i < _windows.keys().size(); i++)
    {
        QPoint movedWindowPos = QPoint(_windows.keys().at(i)->x() * m_devicePixelRatio, (_windows.keys().at(i)->y() * m_devicePixelRatio));
        painter.drawImage(movedWindowPos, _windows.values().at(i));
    }

    painter.end();
}

void JxStreamer::windowClosed(QWindow *window)
{
    if (_windows.contains(window))
    {
        _windows.remove(window);
    }
    _lastTopImage = QImage();
    __ffimage = _lastBrowserImage.copy();
    this->flush();
}

void JxStreamer::extraFlush(QImage &image)
{
    if (lastFlush.elapsed() > EXTRAFLUSH - 15)
    {
        //@todo copy packet instead of convert/encode all the time
        _encoder->write(_encoder->convertRgbTYuv(image.bits()));
        _encoder->write(_encoder->convertRgbTYuv(image.bits()));
        _encoder->write(_encoder->convertRgbTYuv(image.bits()));
    }
}
void JxStreamer::resize(const QSize &size)
{

    m_width = size.width();   // devicescaleratio
    m_height = size.height(); // devicescaleratio

    int mod = m_width % 4;

    if (mod == 1)
    {
        m_width -= 1;
    }
    else if (mod == 2)
    {
        m_width += 2;
    }
    else if (mod == 3)
    {
        m_width += 1;
    }

    if (m_height % 2 != 0)
    {
        m_height++;
    }

    restartEncoder();
}

void JxStreamer::restartEncoder()
{
    _encoder->disconnect(); //should disconnect all signals
    _encoder = EncoderPtr::create(_window, m_width, m_height, m_crf, TARGET);
    connect(_encoder.data(), &Encoder::DataWritten, &JxWsServer::instance(), &JxWsServer::sendVideo);
}
void JxStreamer::changeQuality(int quality)
{
    // quality:crf
    // 0:35 - 5:23 - 10:8
    if (0 <= quality && quality <= 10)
    {
        m_crf = (3 * (10 - quality) + 8);
        restartEncoder();
    }
    else
    {
        qInfo() << "quality is not valid:" << quality;
    }
}
void JxStreamer::pause()
{
    _paused = true;
}

void JxStreamer::resume()
{
    _paused = false;
}

void JxStreamer::setPixelRatio(float ratio)
{
    qInfo() << "[JxStreamer::setPixelRatio]" << ratio;

    if (ratio <= 0 || ratio > 10)
    {
        qInfo() << "ratio should be `0 < ratio && ratio < 10`:" << ratio;
        return;
    }

    // TODO: disabled for now, ask alex
    m_devicePixelRatio = ratio;
}

QT_END_NAMESPACE