#ifndef JXBACKINGSTORE_H
#define JXBACKINGSTORE_H

#include "jxwsserver.h"
#include "videostreaming/JStreamer.h"
#include <qpa/qplatformbackingstore.h>
#include <qpa/qplatformwindow.h>
#include <QtGui/QImage>
#include <QMap>

QT_BEGIN_NAMESPACE

typedef QSharedPointer<JxStreamer> JxStreamerPtr;

class JxBackingStore : public QPlatformBackingStore
{
public:
  JxBackingStore(QWindow *window);
  ~JxBackingStore();
  static QMap<int, JxStreamerPtr>& streamers(){
    static QMap<int, JxStreamerPtr> streamersMap;
    // qInfo() << "Getting streamers , count is " << streamersMap.size() << " address is " << &streamersMap;
    return streamersMap;

  };
  
  QPaintDevice *paintDevice() override;
  void flush(QWindow *window, const QRegion &region, const QPoint &offset) override;
  void resize(const QSize &size, const QRegion &staticContents) override;

private:
  QWindow *_window;
  JxStreamerPtr _streamer;
  QImage _image;
};

QT_END_NAMESPACE

#endif