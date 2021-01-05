#ifndef QBACKINGSTORE_MINIMAL_H
#define QBACKINGSTORE_MINIMAL_H

#include "../jxwsserver.h"

#include <qpa/qplatformbackingstore.h>
#include <qpa/qplatformwindow.h>
#include <QtGui/QImage>
#include <QTimer>
#include <QMutex>
#include <QElapsedTimer>
#include <QProcess>
#include <QThread>
#include "Encoder.h"

QT_BEGIN_NAMESPACE

typedef QSharedPointer<Encoder> EncoderPtr;

class JxStreamer : public QObject
{
    Q_OBJECT
  public:
  JxStreamer(QWindow *window);
  virtual ~JxStreamer();
  void flush();
  void composite(QWindow *window, const QRegion &region, const QImage &image);

  void resize(const QSize &size);
  void windowClosed(QWindow *window);

  // static JxStreamer &instance()
  // {
  //   static JxStreamer instance; // Guaranteed to be destroyed.
  //                               // Instantiated on first use.
  //   return instance;
  // }
  void changeQuality(int);
  void setPixelRatio(float);

public slots:
  void pause();
  void resume();
  void readyReadStandardOutput();

  void readyReadStandardError();
  void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
  Q_DISABLE_COPY(JxStreamer)
  bool firstFlush;

  QWindow *_window;

  QMap<QWindow*, QImage> _windows;
  QImage _lastBrowserImage;
  QImage _lastTopImage;
  QPoint _lastTopFrameWindowPos;
  QWindow *_lastTopWindow;

  EncoderPtr _encoder;

  QImage __ffimage;

  QElapsedTimer lastFlush;
  QElapsedTimer flushTime;

  
  void extraFlush(QImage &image);
  void restartEncoder();
  bool _paused = false;
  QProcess _ffmpeg;
  bool _restart = false;

  QThread *_streamThread;

  int m_crf;
  int m_width;
  int m_height;
  float m_devicePixelRatio;
};

QT_END_NAMESPACE

#endif