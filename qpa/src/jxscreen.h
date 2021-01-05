#include <QtGui/qpa/qplatformnativeinterface.h>

QT_BEGIN_NAMESPACE

typedef void (*PixelRatioChanged)();

class JxScreen : public QPlatformScreen
{
public:
  JxScreen(int width, int height)
      : _depth(32), _format(QImage::Format_RGB888), _geometry(QRect(0, 0, width, height))
  {
    _cursor.reset(new JxCursor());
  }

  QRect geometry() const override
  {
    return _geometry;
  }
  int depth() const override { return _depth; }
  QImage::Format format() const override { return _format; }

  QPlatformCursor *cursor() const
  {
    return _cursor.data();
  }
  void setGeometry(QRect geometry) 
  {
    qInfo() << "Screen size is " << geometry;
    _geometry = geometry;
  }

public:
  int _depth;
  QImage::Format _format;
  QRect _geometry;
  QSize _physicalSize;

private:
  QScopedPointer<JxCursor> _cursor;
};

QT_END_NAMESPACE
